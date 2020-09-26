/* Toader Sergiu-Cristian, 325CB */

#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));


    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);
    memset(line, 0, LINELEN);

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    memset(line, 0, LINELEN);
    


    // (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        char *cookies_str = calloc(LINELEN, sizeof(char));
        sprintf(line, "Cookie:");
        sprintf(cookies_str, " %s;", cookies[0]);
        strcat(line, cookies_str);
        for(int i = 1; i < cookies_count; i++) {
            sprintf(cookies_str, " %s;", cookies[i]);
            strcat(line, ";");
            strcat(line, cookies_str);
        }
        
        compute_message(message, line);
        memset(line, 0, LINELEN);

        free(cookies_str);
    }
    // (optional): add token
    if(token) {
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    free(line);

    // add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type,
    char **body_data, int body_data_fields_count, char **cookies,
    int cookies_count, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));


    // write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    memset(line, 0, LINELEN);
    

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    memset(line, 0, LINELEN);


    /*  add necessary headers (Content-Type and Content-Length are mandatory)
        compute the message size
    */
    
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    char *aux = calloc(LINELEN, sizeof(char));
    sprintf(aux, "{%s", body_data[0]);
    strcat(body_data_buffer, aux);
    for(int i = 1; i < body_data_fields_count; i++) {
        sprintf(aux, ",%s", body_data[i]);
        strcat(body_data_buffer, aux);
    }
    strcat(body_data_buffer, "}");
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);
    memset(line, 0, LINELEN);


    // (optional): add cookies
    if (cookies != NULL) {
       sprintf(line, "Cookie:");
       sprintf(aux, " %s", cookies[0]);
       strcat(line, aux);

        for(int i = 1; i < cookies_count; i++) {
            sprintf(aux, "; %s", cookies[i]);
            strcat(line, aux);
        }

        compute_message(message, line);
        memset(line, 0, LINELEN);
    }
    // (optional): add token
    if(token) {
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    // add new line at end of header
    compute_message(message, "");


    // add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(aux);
    free(line);
    free(body_data_buffer);

    return message;
}


// trimite un request de tip DELETE
char *compute_delete_request(char *host, char *url, char *token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    // add the token
    sprintf(line, "Authorization: Bearer ");
    strcat(line, token);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    // add new line at end of header
    compute_message(message, "");


    free(line);
    return message;
}
