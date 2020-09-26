#include "skel.h"

// void parse_arp_table() 
// {
// 	FILE *f;
// 	fprintf(stderr, "Parsing ARP table\n");
// 	f = fopen("arp_table.txt", "r");
// 	DIE(f == NULL, "Failed to open arp_table.txt");
// 	char line[100];
// 	int i = 0;
// 	for(i = 0; fgets(line, sizeof(line), f); i++) {
// 		char ip_str[50], mac_str[50];
// 		sscanf(line, "%s %s", ip_str, mac_str);
// 		fprintf(stderr, "IP: %s MAC: %s\n", ip_str, mac_str);
// 		arp_table[i].ip = inet_addr(ip_str);
// 		int rc = hwaddr_aton(mac_str, arp_table[i].mac);
// 		DIE(rc < 0, "invalid MAC");
// 	}
// 	arp_table_len = i;
// 	fclose(f);
// 	fprintf(stderr, "Done parsing ARP table.\n");
// }

struct arp_entry *get_arp_entry(__u32 ip, struct arp_entry *arp_table, int arp_table_len) {
    
	for(int i = 0; i < arp_table_len; i++){

		if(arp_table[i].ip == ip){
			return &arp_table[i];
		}
	}

    return NULL;
}



int read_arp_table(struct arp_entry *arp_table) {
	FILE *input = fopen("arp_table.txt", "rt");
	DIE(input == NULL, "opening arp_table.txt");

	char line[50];
	int count = 0;

	while(fgets(line, 50, input) != NULL) {
		char ip[20];

		sscanf(line, "%s %hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			ip, &arp_table[count].mac[5],
			&arp_table[count].mac[4], &arp_table[count].mac[3],
			&arp_table[count].mac[2], &arp_table[count].mac[1],
			&arp_table[count].mac[0]);

		arp_table[count].ip = inet_addr(ip);

		count++;		
	}

	return count;
}

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	packet m;
	int rc;

	init();

	// calcularea numarului de linii din fisier
	int rTableSize = getRTableSize("rtable.txt");
	
	// 1. Parsarea tabelei de rutare
	routeTable rTable = (routeTable) malloc (rTableSize * sizeof(routeEntry));
	setRouteTable(rTable, rTableSize);

	qsort(rTable, rTableSize, sizeof(routeEntry), cmpfunc);

	// Tabela arp statica
	struct arp_entry *arp_table;
	
	int arp_table_len = getRTableSize("arp_table.txt");
	arp_table = (struct arp_entry *) malloc(arp_table_len * sizeof(struct arp_entry));
	read_arp_table(arp_table);

	printf("inainte de while\n");

	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");
		
		printf("am primit pachetul\n");

		// Identificare tip de pachet primit
		struct ether_header *eth_hdr = (struct ether_header *)m.payload;

		// Pachet IP
		if(ntohs(eth_hdr->ether_type) == ETHERTYPE_IP) {
			printf("pachetul este de tip ETHERTYPE_IP\n");
			struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));

			// Pachet de tip icmp echo request
			struct icmphdr *icmp_hdr = (struct icmphdr *)(m.payload
			  	+ sizeof(struct ether_header) + sizeof(struct iphdr));

			if(icmp_hdr->type == ICMP_ECHO && ntohl(ip_hdr->daddr) ==
				getIP(get_interface_ip(m.interface)) ) {

				packet reply;
				reply.len = sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct icmphdr);
				reply.interface = m.interface; 

				struct ether_header *eth_hdr_reply = (struct ether_header *)reply.payload;
				struct iphdr *ip_hdr_reply = (struct iphdr *)(reply.payload + sizeof(struct ether_header));
				struct icmphdr *icmp_hdr_reply = (struct icmphdr *)(reply.payload +
					sizeof(struct ether_header) + sizeof(struct iphdr));

				// u_char* myMac = NULL;
				// get_interface_mac(m.interface, myMac);

				// ETH Header
				memcpy(eth_hdr_reply->ether_dhost, eth_hdr->ether_shost, 6);
				memcpy(eth_hdr_reply->ether_shost, eth_hdr->ether_dhost, 6);
				eth_hdr_reply->ether_type 			= htons(ETHERTYPE_IP);
				
					// IP Header
				ip_hdr_reply->version 				= 4;
				ip_hdr_reply->ihl 					= 5;
				ip_hdr_reply->tos 					= 0;

				ip_hdr_reply->tot_len 				= htons(reply.len - sizeof(struct ether_header));
				ip_hdr_reply->id 					= htons(getpid() & 0xFF);
				ip_hdr_reply->frag_off 				= 0;
				ip_hdr_reply->ttl 					= 64;
				ip_hdr_reply->protocol 				= IPPROTO_ICMP;

				//char * myIP = get_interface_ip(m.interface);
				ip_hdr_reply->saddr 				= ip_hdr->daddr;
				ip_hdr_reply->daddr 				= ip_hdr->saddr;

				ip_hdr_reply->check 				= 0;
				ip_hdr_reply->check 				= ip_checksum(ip_hdr_reply, sizeof(struct iphdr));

					// ICMP Header
				icmp_hdr_reply->type 				= ICMP_ECHOREPLY;
				icmp_hdr_reply->code 				= 0;
				icmp_hdr_reply->un.echo.id 			= icmp_hdr->un.echo.id;
				icmp_hdr_reply->un.echo.sequence 	= icmp_hdr->un.echo.sequence;
				icmp_hdr_reply->checksum 			= 0;
				icmp_hdr_reply->checksum 			= ip_checksum(icmp_hdr_reply, sizeof(struct icmphdr));

				send_packet(m.interface, &reply);
				continue;
			}

				// packet reply;
			 // 	struct ether_header *eth_hdr_reply = (struct ether_header *)reply.payload;
			 // 	struct iphdr *ip_hdr_reply = (struct iphdr *)(reply.payload
			 // 		+ sizeof(struct ether_header));


			//}
			// Pachet ip normal
			if(ip_checksum(ip_hdr, sizeof(struct iphdr)) != 0){
				// Daca checksum-ul este gresit, arunc pachetul
				printf("ERROR checksum\n");
				continue;
			}

			// 5. Verificare TTL <= 1
 			if(ip_hdr->ttl <=1){
				// ICMP Time Exceeded

				packet reply;
				reply.len = sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct icmphdr);
				reply.interface = m.interface; 

				struct ether_header *eth_hdr_reply = (struct ether_header *)reply.payload;
				struct iphdr *ip_hdr_reply = (struct iphdr *)(reply.payload + sizeof(struct ether_header));
				struct icmphdr *icmp_hdr_reply = (struct icmphdr *)(reply.payload +
					sizeof(struct ether_header) + sizeof(struct iphdr));

				// u_char* myMac = NULL;
				// get_interface_mac(m.interface, myMac);

				// ETH Header
				memcpy(eth_hdr_reply->ether_dhost, eth_hdr->ether_shost, 6);
				memcpy(eth_hdr_reply->ether_shost, eth_hdr->ether_dhost, 6);
				eth_hdr_reply->ether_type 			= htons(ETHERTYPE_IP);
				
					// IP Header
				ip_hdr_reply->version 				= 4;
				ip_hdr_reply->ihl 					= 5;
				ip_hdr_reply->tos 					= 0;

				ip_hdr_reply->tot_len 				= htons(reply.len - sizeof(struct ether_header));
				ip_hdr_reply->id 					= htons(getpid() & 0xFF);
				ip_hdr_reply->frag_off 				= 0;
				ip_hdr_reply->ttl 					= 64;
				ip_hdr_reply->protocol 				= IPPROTO_ICMP;

				//char * myIP = get_interface_ip(m.interface);
				ip_hdr_reply->saddr 				= ip_hdr->daddr;
				ip_hdr_reply->daddr 				= ip_hdr->saddr;

				ip_hdr_reply->check 				= 0;
				ip_hdr_reply->check 				= ip_checksum(ip_hdr_reply, sizeof(struct iphdr));

					// ICMP Header
				icmp_hdr_reply->type 				= ICMP_TIME_EXCEEDED;
				icmp_hdr_reply->code 				= ICMP_EXC_TTL;
				icmp_hdr_reply->un.echo.id 			= htons(getpid() & 0xFF);
				icmp_hdr_reply->un.echo.sequence 	= htons(0);
				icmp_hdr_reply->checksum 			= 0;
				icmp_hdr_reply->checksum 			= ip_checksum(icmp_hdr_reply, sizeof(struct icmphdr));

				send_packet(m.interface, &reply);
				continue;
			}

			// se cauta binar cea mai buna intrare din rTable
			printf("%u\n", ntohl(ip_hdr->daddr));
			routeEntry *best_route = get_best_route_bin(ntohl(ip_hdr->daddr), 0, rTableSize - 1, rTable, rTableSize);
				
			if(best_route == NULL) {
				//printf("ERROR best_route\n");
				// nu exista intrarea in tabela de rutare
				// TODO -> ICMP Destination Unreachable


				// ICMP Destination Unreachable

				packet reply;
				reply.len = sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct icmphdr);
				reply.interface = m.interface; 

				struct ether_header *eth_hdr_reply = (struct ether_header *)reply.payload;
				struct iphdr *ip_hdr_reply = (struct iphdr *)(reply.payload + sizeof(struct ether_header));
				struct icmphdr *icmp_hdr_reply = (struct icmphdr *)(reply.payload +
					sizeof(struct ether_header) + sizeof(struct iphdr));

				// u_char* myMac = NULL;
				// get_interface_mac(m.interface, myMac);

				// ETH Header
				memcpy(eth_hdr_reply->ether_dhost, eth_hdr->ether_shost, 6);
				memcpy(eth_hdr_reply->ether_shost, eth_hdr->ether_dhost, 6);
				eth_hdr_reply->ether_type 				= htons(ETHERTYPE_IP);
				
				// IP Header
				ip_hdr_reply->version 				= 4;
				ip_hdr_reply->ihl 					= 5;
				ip_hdr_reply->tos 					= 0;

				ip_hdr_reply->tot_len 				= htons(reply.len - sizeof(struct ether_header));
				ip_hdr_reply->id 					= htons(getpid() & 0xFF);
				ip_hdr_reply->frag_off 				= 0;
				ip_hdr_reply->ttl 					= 64;
				ip_hdr_reply->protocol 				= IPPROTO_ICMP;

				//char * myIP = get_interface_ip(m.interface);
				ip_hdr_reply->saddr 				= ip_hdr->daddr;
				ip_hdr_reply->daddr 				= ip_hdr->saddr;

				ip_hdr_reply->check 				= 0;
				ip_hdr_reply->check 				= ip_checksum(ip_hdr_reply, sizeof(struct iphdr));

				// ICMP Header
				icmp_hdr_reply->type 				= ICMP_DEST_UNREACH;
				icmp_hdr_reply->code 				= 0;
				icmp_hdr_reply->un.echo.id 			= htons(getpid() & 0xFF);
				icmp_hdr_reply->un.echo.sequence 	= htons(0);
				icmp_hdr_reply->checksum 			= 0;
				icmp_hdr_reply->checksum 			= ip_checksum(icmp_hdr_reply, sizeof(struct icmphdr));

				send_packet(m.interface, &reply);

				continue;
			}

			(ip_hdr->ttl)--;
			ip_hdr->check = 0;
			ip_hdr->check = ip_checksum(ip_hdr, sizeof(struct iphdr));

			// obtine adreasa mac din tabela arp
			struct arp_entry * best_arp = get_arp_entry(ip_hdr->daddr, arp_table, arp_table_len);
				
			if(best_arp == NULL) {
				// TODO - Daca nu s-a gasit nicio valoare in arp table -> se adauga pachetul in coada si se trimite un arp request
				printf("ERROR best_arp\n");
				continue;
			}

			// Setare adresa MAC
			for(int i = 0; i < 6; i++){
				eth_hdr->ether_dhost[i] = best_arp->mac[5-i];
			}
				

			// trimitere pachet
			send_packet(best_route->interface, &m);
		}
		//  Pachet de tip arp
		else if(ntohs(eth_hdr->ether_type) == ETHERTYPE_ARP) {
			printf("pachetul este de tip ETHERTYPE_ARP\n");
			// TODO - raspuns la arp request
			continue;
		} else {
			printf("pachetul este de alt tip\n");
		
		}

	}

	free(rTable);

	return 0;

}
