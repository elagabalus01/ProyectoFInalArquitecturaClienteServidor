/*

    Autores:
        Jiménez Ruiz Gustavo Alfredo
        Santander Martínez Ángel Antonio

    Fecha de finalización:
        24 de noviembre del 2021

    Objetivo:
        Creación de un cliente que simule el comportamiento SSH de forma remota

*/

// Importación de bibliotecas
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

// Puerto al que el cliente se conecta
#define PORT 3490
// Número máximo de bytes que se pueden obtener por envío
#define MAXDATASIZE 10000

// Función main que define el comportamiento del cliente
int main(int argc, char *argv[]){

    /* 
        Variables que utiliza el cliente:
        -descriptor de archivo del socket
        -número de bytes
        -tamaño del comando
        -respuesta
        -tamaño de la respuesta
        -buffer para los mensajes
        -bandera para saber si está conectado el cliente
    */
    int sockfd, numbytes,command_size;
    char *response=(char*)malloc(MAX_INPUT_SIZE);
    int response_size=0;
    char buf[MAXDATASIZE];
    int running=1;

    // Conectores de información de dirección
    struct hostent *he;
    struct sockaddr_in their_addr;

    if(argc != 2){
        fprintf(stderr, "Client-Usage: %s host_servidor\n", argv[0]);
        // Salida
        exit(1);
    }

    // Se obtiene la información del host
    if((he=gethostbyname(argv[1])) == NULL){
        perror("gethostbyname()");
        exit(1);
    }else
        printf("Client-The remote host is: %s\n", argv[1]);

    // Se intenta crear el socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("socket()");
        exit(1);
    }else
    printf("Client-The socket() sockfd is OK...\n");

    // Orden de bytes, red y short del host
    their_addr.sin_family = AF_INET;
    printf("Server-Using %s and port %d...\n", argv[1], PORT);
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    // Variable comando para almacenar el comando que envía el cliente
    char *command;

    // Se termina con caracter nulo el resto de la estructura
    memset(&(their_addr.sin_zero), '\0', 8);

    // Intento de conexión
    if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1){
        printf("connect()\n");
        printf("Error; %s\n",strerror(errno));
        exit(1);
    }else
        printf("Client-The connect() is OK...\n");

    // Bucle para preguntar por comando, enviar y recibir su información
    while(running){
        command=read_prompt();
        if(!command){
            puts("Client-Info: Command is empty");
            command=(char*)realloc(command,sizeof(char)*strlen("echo \"Without command\""));
            strcpy(command,"echo \"Without command\"");
        
        // Si se manda instrucción de salida se termina el bucle
        }else if(strcmp(command,"exit")==0){
            puts("Client-Info: Exiting from server...");
            running=0;
            break;
        }
        
        command_size=strlen(command);
        
        if(send(sockfd,command,command_size,0) == -1){
            printf("Error: Client-send() error lol!\n");
            printf("Error: %s\n",strerror(errno));
        }else
            printf("Client-send is OK...!\n");
        puts("Client-Info: Waiting for answer from server");
        
        if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1){
          printf("Error recv()");
          exit(1);
        }else
          printf("Client-The recv() is OK...\n");
        
        // Se confirma el número de bytes que se recibieron
        printf("Client-Received %d bytes\n",numbytes);
        buf[numbytes] = '\0';
        response_size=strlen(buf);
        response=(char*)realloc(response,sizeof(char)*response_size);
        response=strcpy(response,buf);

        // Se imprimen los bytes recibidos
        printf("Client-Received: \n%s", response);
    }

    // Se cierra el socket que se había creado
    printf("Client-Closing sockfd\n");
    close(sockfd);
    return 0;
}