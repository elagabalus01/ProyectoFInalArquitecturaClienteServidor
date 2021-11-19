#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "LecturaComando.h"
// the port client will be connecting to
#define PORT 3490
// max number of bytes we can get at once
#define MAXDATASIZE 10000

int main(int argc, char *argv[]){
    int sockfd, numbytes,command_size;
    char *response=(char*)malloc(MAX_NAME_SZ);
    int response_size=0;
    char buf[MAXDATASIZE];
    struct hostent *he;
    // connectors address information
    struct sockaddr_in their_addr;
    // if no command line argument supplied
    if(argc != 2){
        fprintf(stderr, "Client-Usage: %s host_servidor\n", argv[0]);

        // just exit
        exit(1);
    }

    // get the host info
    if((he=gethostbyname(argv[1])) == NULL){
        perror("gethostbyname()");
        exit(1);
    }
    else
    printf("Client-The remote host is: %s\n", argv[1]);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("socket()");
        exit(1);
    }
    else
    printf("Client-The socket() sockfd is OK...\n");

    // host byte order
    their_addr.sin_family = AF_INET;
    // short, network byte order
    printf("Server-Using %s and port %d...\n", argv[1], PORT);
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    char *command;
    // zero the rest of the struct
    memset(&(their_addr.sin_zero), '\0', 8);
    if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1){
        printf("connect()\n");
        printf("Error; %s\n",strerror(errno));
        exit(1);
    }
    else{
        printf("Client-The connect() is OK...\n");
    }
    while(1){
        command=read_prompt();
        command_size=strlen(command);
        printf("Echo command: %s\n",command);
        if(send(sockfd,command,command_size,0) == -1){
            printf("Error: Client-send() error lol!\n");
            printf("Error: %s\n",strerror(errno));
        }
        else
            printf("client-send is OK...!\n");
        puts("Esperando respuesta del servidor");
        if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1){
          printf("Error recv()");
          exit(1);
        }
        else
          printf("Client-The recv() is OK...\n");
        buf[numbytes] = '\0';
        response_size=strlen(buf);
        response=(char*)realloc(response,sizeof(char)*response_size);
        response=strcpy(response,buf);
        printf("Client-Received: %s\n", response);
    }
    printf("Client-Closing sockfd\n");
    close(sockfd);
    return 0;
}
