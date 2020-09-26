#include "skel.h"

int interfaces[ROUTER_NUM_INTERFACES];

int get_sock(const char *if_name)
{
	int res;
	int s = socket(AF_PACKET, SOCK_RAW, 768);
	DIE(s == -1, "socket");

	struct ifreq intf;
	strcpy(intf.ifr_name, if_name);
	res = ioctl(s, SIOCGIFINDEX, &intf);
	DIE(res, "ioctl SIOCGIFINDEX");

	struct sockaddr_ll addr;
	memset(&addr, 0x00, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = intf.ifr_ifindex;

	res = bind(s , (struct sockaddr *)&addr , sizeof(addr));
	DIE(res == -1, "bind");
	return s;
}

packet* socket_receive_message(int sockfd, packet *m)
{        
	/* 
	 * Note that "buffer" should be at least the MTU size of the 
	 * interface, eg 1500 bytes 
	 * */
	m->len = read(sockfd, m->payload, MAX_LEN);
	DIE(m->len == -1, "read");
	return m;
}

int send_packet(int sockfd, packet *m)
{        
	/* 
	 * Note that "buffer" should be at least the MTU size of the 
	 * interface, eg 1500 bytes 
	 * */
	int ret;
	ret = write(interfaces[sockfd], m->payload, m->len);
	DIE(ret == -1, "write");
	return ret;
}

int get_packet(packet *m) {
	int res;
	fd_set set;

	FD_ZERO(&set);
	while (1) {
		for (int i = 0; i < ROUTER_NUM_INTERFACES; i++) {
			FD_SET(interfaces[i], &set);
		}

		res = select(interfaces[ROUTER_NUM_INTERFACES - 1] + 1, &set, NULL, NULL, NULL);
		DIE(res == -1, "select");

		for (int i = 0; i < ROUTER_NUM_INTERFACES; i++) {
			if (FD_ISSET(interfaces[i], &set)) {
				socket_receive_message(interfaces[i], m);
				m->interface = i;
				return 0;
			}
		}
	}
	return -1;
}

char *get_interface_ip(int interface)
{
	struct ifreq ifr;
	sprintf(ifr.ifr_name, "r-%u", interface);
	ioctl(interfaces[interface], SIOCGIFADDR, &ifr);
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

int get_interface_mac(int interface, uint8_t *mac)
{
	struct ifreq ifr;
	sprintf(ifr.ifr_name, "r-%u", interface);
	ioctl(interfaces[interface], SIOCGIFHWADDR, &ifr);
	memcpy(mac, ifr.ifr_addr.sa_data, 6);
	return 1;
}

void init()
{
	int s0 = get_sock("r-0");
	int s1 = get_sock("r-1");
	int s2 = get_sock("r-2");
	int s3 = get_sock("r-3");
	interfaces[0] = s0;
	interfaces[1] = s1;
	interfaces[2] = s2;
	interfaces[3] = s3;
}

static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}


// Functia de checksum preluata din laboratorul 4
uint16_t ip_checksum(void* vdata,size_t length) {
	// Cast the data pointer to one that can be indexed.
	char* data=(char*)vdata;

	// Initialise the accumulator.
	uint64_t acc=0xffff;

	// Handle any partial block at the start of the data.
	unsigned int offset=((uintptr_t)data)&3;
	if (offset) {
		size_t count=4-offset;
		if (count>length) count=length;
		uint32_t word=0;
		memcpy(offset+(char*)&word,data,count);
		acc+=ntohl(word);
		data+=count;
		length-=count;
	}

	// Handle any complete 32-bit blocks.
	char* data_end=data+(length&~3);
	while (data!=data_end) {
		uint32_t word;
		memcpy(&word,data,4);
		acc+=ntohl(word);
		data+=4;
	}
	length&=3;

	// Handle any partial block at the end of the data.
	if (length) {
		uint32_t word=0;
		memcpy(&word,data,length);
		acc+=ntohl(word);
	}

	// Handle deferred carries.
	acc=(acc&0xffffffff)+(acc>>32);
	while (acc>>16) {
		acc=(acc&0xffff)+(acc>>16);
	}

	// If the data began at an odd byte address
	// then reverse the byte order to compensate.
	if (offset&1) {
		acc=((acc&0xff00)>>8)|((acc&0x00ff)<<8);
	}

	// Return the checksum in network byte order.
	return htons(~acc);
}

int hex2byte(const char *hex)
{
	int a, b;
	a = hex2num(*hex++);
	if (a < 0)
		return -1;
	b = hex2num(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}
/**
 * hwaddr_aton - Convert ASCII string to MAC address (colon-delimited format)
 * @txt: MAC address as a string (e.g., "00:11:22:33:44:55")
 * @addr: Buffer for the MAC address (ETH_ALEN = 6 bytes)
 * Returns: 0 on success, -1 on failure (e.g., string not a MAC address)
 */
int hwaddr_aton(const char *txt, uint8_t *addr)
{
	int i;
	for (i = 0; i < 6; i++) {
		int a, b;
		a = hex2num(*txt++);
		if (a < 0)
			return -1;
		b = hex2num(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (i < 5 && *txt++ != ':')
			return -1;
	}
	return 0;
}

// Sortarea tabelei de rutare - crescator dupa prefix, apoi crescator dupa masca
int cmpfunc (const void * a, const void * b) {
	routeEntry e1 = *(routeEntry *)a;
	routeEntry e2 = *(routeEntry *)b;
	
	if(e1.prefix > e2.prefix) {
		return 1;
	} else if(e1.prefix == e2.prefix) {
		if(e1.mask > e2.mask) {
			return 1;
		} else if(e1.mask == e2.mask) {
			return 0;
		}
		return -1;
	}

	return -1;
}

// functie care primeste o adresa de tip IP sub forma de string
// si intoarce un uint32_t corespunzator
uint32_t getIP(char * str){

	char * aux = strtok(str, ".");
	uint32_t words[4];
	int i = 0;
	while(aux){
		words[i++] = atoi(aux);
		aux = strtok(NULL, ".");
	}

	return (words[0] << 24) + (words[1] << 16) + (words[2] << 8) + words[3];
}

// functie care seteaza tabela de rutare
void setRouteTable(routeTable rTable, int rTableSize){

	FILE* rfile = fopen("rtable.txt", "r");
	if(!rfile) {
		fprintf(stderr, "Nu exista tabela de rutare\n");
		return;
	}

	//Parsarea tabelei de rutare
	char ** routeTableLines;

	routeTableLines = (char **) malloc (sizeof(char*) * rTableSize);
	if(!routeTableLines) {
		fprintf(stderr, "Nu s-a alocat tabela de rutare\n");
		return;
	}

	for(int i = 0; i < rTableSize; i++){
		routeTableLines[i] = (char *) malloc (50);
		if(!routeTableLines[i]) {
			fprintf(stderr, "Nu s-a alocat tabela de rutare\n");
			return;
		}
	}


	char line[50];
	for(int i = 0; i < rTableSize; i++){
		fgets(line, sizeof(line), rfile);
		strcpy(routeTableLines[i], line);
		
	}

	int idx = 0;
	char ** routeTableWords = (char **) malloc(sizeof(char*) * rTableSize * 4);
	if(!routeTableWords){
		fprintf(stderr, "Nu s-a alocat tabela de rutare\n");
		return;
	}

	for(int i = 0; i < rTableSize * 4; i++){
		routeTableWords[i] = (char *) malloc(16);
		if(!routeTableWords[i]){
			fprintf(stderr, "Nu s-a alocat tabela de rutare\n");
			return;
		}
	}

	for(int i = 0; i < rTableSize; i++){
		char * aux = strtok(routeTableLines[i], " ");
		while(aux){
			memcpy(routeTableWords[idx++], aux, strlen(aux) + 1);
			aux = strtok(NULL, " ");
		}
		// se elimina caracterul '\n' din string-ul de pe ultima coloana
		routeTableWords[idx -1][strlen(routeTableWords[idx-1]) - 1] = 0;
	}

	
	for(int i = 0; i < rTableSize; i++) {
		rTable[i].prefix = getIP(routeTableWords[4*i]);
	 	rTable[i].next_hop = getIP(routeTableWords[4*i + 1]);
	 	rTable[i].mask = getIP(routeTableWords[4*i + 2]);
	 	rTable[i].interface = routeTableWords[4*i + 3][0] - '0';
	}

	// eliberare memorie folosita
	for(int i = 0; i < rTableSize * 4; i++){
		free(routeTableWords[i]);
	}
	free(routeTableWords);
	for(int i = 0; i < rTableSize; i++){
		free(routeTableLines[i]);
	}
	free(routeTableLines);
	fclose(rfile);
}

// intoarce numarul de linii din fisier
// sau -1 daca fisierul nu exista
int getRTableSize(char * fileName) {
	int size = 0;
	char c;

	FILE* rfile = fopen(fileName, "r");
	if(!rfile){
		fprintf(stderr, "Nu exista tabela de rutare\n");
		return -1;
	}

	// Numarul de linii este egal cu numarul de caractere '\n' din fisier
	for (c = getc(rfile); c != EOF; c = getc(rfile)) {
        if (c == '\n') {
        	size++;
        }
		
	}
           
	fclose(rfile);
	return size;
}

routeEntry *get_best_route(__u32 dest_ip, routeTable rTable, int rTableSize) {
	/* TODO 1: Implement the function */

	int i = 0;
	routeEntry * route = NULL;
	uint32_t maskMAX = 0;
	for(i = 0; i < rTableSize; i++){
		if((dest_ip & rTable[i].mask) == rTable[i].prefix){
			if(rTable[i].mask > maskMAX){
				maskMAX = rTable[i].mask;
				route = &rTable[i];
			}
		}
	}

	return route;
}

// functie care determina cea mai buna ruta prin cautare binara
routeEntry *get_best_route_bin (uint32_t daddr, int l, int r,
	routeTable rTable, int rTableSize) {
	
	if(r >= l) {

		int m = l + (r - l) / 2;

		// atunci cand s-a gasit o ruta valida, se tine cont de faptul ca
		// intrarile cu acelasi prefix sunt sortate crescator dupa masca, deci
		// se va parcurge vectorul de intrari pana la ultima valoare valida
		// (cea cu masca cea mai mare, deci cea mai specifica ruta)
		if((daddr & rTable[m].mask) == rTable[m].prefix) {
			while(m + 1 < rTableSize && ((rTable[m+1].mask & daddr) == rTable[m+1].prefix) ) {
				m++;
			}
			return &rTable[m];
		}

		// apel recursiv in jumatatea stanga
		if((daddr & rTable[m].mask) < rTable[m].prefix){
			return get_best_route_bin(daddr, l, m-1, rTable, rTableSize);
		} 

		// apel recursiv in jumatatea dreapta
		return get_best_route_bin(daddr, m+1, r, rTable, rTableSize);
	}

	return NULL;
}


// functie pentru afisarea tabelei de rutare - debug only
void printRTable(routeTable rTable, int rTableSize) {
	for(int i = 0; i < rTableSize; i++){
		printf("%u %u %u %u\n", rTable[i].prefix, rTable[i].next_hop, rTable[i].mask, rTable[i].interface);
	}
}