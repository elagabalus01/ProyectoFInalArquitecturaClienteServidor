/*

    Autores:
        Jiménez Ruiz Gustavo Alfredo
        Santander Martínez Ángel Antonio

    Fecha de finalización:
        24 de noviembre del 2021

    Objetivo:
        Creación de un servidor que simule el comportamiento SSH de forma remota

*/

// Importación de bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "LecturaComando.h"

// Puerto que usarán los usuarios para conectarse
#define MYPORT 3490

// Número de conexiones pendientes que se permitirán
#define BACKLOG 10

// Tamaño máximo por mensaje
#define MAXDATASIZE 10000

// Tamaño máximo de respuesta
#define MAXRESPONSESIZE 10000

/*
    Declaración de funciones auxiliares:
    -manejador_señales: maneja interrupciones que pudieran ser mandadas
    -run_command: se ejecuta el comando que se recibió
*/
void manejador_senales(int sig);
int run_command(char* program, char** arg_list,char **data);

// Variable global que representa el socket que se crea
int sockfd;

// Función main que define el comportamiento del cliente
int main(int argc, char *argv[ ]){

    /*
        Variables que utiliza el servidor:
        -aux de envío de mensajes
        -número de bytes que se reciben
        -pid del proceso hijo que ayuda a copiar el búfer de la ejecución de los comandos
        -comando a ejecutar
        -argumentos que acompañan al comando
        -búfer donde se almacena la información recibida
        -socket que se abre con cliente
        -data para enviar la información que generó el comando
    */
    int aux=0;
    int numbytes=0;
    int hijo_id=0;
    char *command=(char*)malloc(MAX_INPUT_SIZE);
    char **exec_args;
    char buf[MAXDATASIZE];
    int new_fd;
    char *data=(char*)malloc(MAX_INPUT_SIZE);

    // Conectores de información de dirección
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;

    // Se manejan las señales
    if(signal(SIGINT, manejador_senales) == SIG_ERR ){
        printf("Can't catch SIGINT\n");
        perror("signal");
    }

    // Se intenta crear el socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("Server-socket() error lol!");
        exit(1);
    }else
    printf("Server-socket() sockfd is OK...\n");

    // Orden de bytes, red y short del host
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);

    // Se llena la información con la ip actual
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // Se informa al servidor el puerto y la dirección ip
    printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);

    // Se termina con caracter nulo el resto de la estructura
    memset(&(my_addr.sin_zero), '\0', 8);
    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
        printf("Server-bind() error");
        exit(1);
    }else
    printf("Server-bind() is OK...\n");

    // Se mantiene a la escucha al servidor
    if(listen(sockfd, BACKLOG) == -1){
        printf("Server-listen() error");
        exit(1);
    }else
        printf("Server-listen() is OK...Listening...\n");
    
    sin_size = sizeof(struct sockaddr_in);
    
    // Bucle para recibir conexiones
    while(1){
        
        // Se limpian todos los procesos muertos
        if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
            printf("Server-accept() error");
            exit(1);
        }

        printf("Server-accept() is OK...\n");
        printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));
        printf("Server-new socket, new_fd is OK...\n");

        // Bucle para recibir solicitudes de cada cliente
        while(1){

            // Se recibe comando
            if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1){
                printf("recv()\n");
                printf("Error:%s\n",strerror(errno));
                exit(1);
            }else
                printf("Server-The recv() is OK...\n");
            buf[numbytes] = '\0';
            printf("Server-Received bytes %d\n",numbytes);

            // Si se recibe comando con información
            if(numbytes>0){
                printf("Server-Received: %s\n",buf);
                command=realloc(command,strlen(buf)*sizeof(char));
                command=strcpy(command,buf);
                //printf("Error: %s\n",strerror(errno));
                exec_args=split(command);
                printf("Processing command: %s\n", buf);
                hijo_id=run_command(exec_args[0],exec_args,&data);
                printf("Server-Info: The command was executed by child %d\n",hijo_id);
                if(aux=send(new_fd, data, strlen(data), 0) == -1)
                    printf("Server-send() error lol!");
                else{
                    printf("Server-send is OK...!\n");
                    printf("Server-sends: Sending answer... \n%s", data);
                }
            }else{
                puts("Server-Info: Nothing received from client");
                break;
                }
            }
            // Se cierra conexión con el socket del cliente
            close(new_fd);
            printf("Server-new socket, new_fd closed successfully...\n");
    }

    // Se cierra el socket del servidor
    close(sockfd);
    printf("Server-socket, sockfd closed successfully...\n");
    return 0;
}

// Función que maneja señales
void manejador_senales(int sig){
    puts("Checking exception");
    switch (sig) {
        case SIGINT:
            close(sockfd);
            printf("Closing socket %i\n",sockfd);
            puts("Turning off server...");
            puts("Goodbye");
            exit(0);
        break;
        defaulf:
            puts("Other signal");
        break;
    }
}

// Función que corre el comando a través de fork y pipefd
int run_command(char* program, char** arg_list,char **data){
    pid_t child_pid;
    int arg_wait;
    int data_size=0;
    int pipefd[2];
    pipe(pipefd);
    child_pid = fork();
    if (child_pid != 0){
        close(pipefd[1]);
        char buffer[MAXRESPONSESIZE]={};
        read(pipefd[0], buffer, sizeof(buffer));
        fsync(pipefd[0]);
        printf("Buffer printing: %s\n",buffer);
        int retorno_wait =  wait( &arg_wait );
        printf( "Server-processing-parent: value &arg_wait=%p\n",&arg_wait );
        printf( "Server-processing-parent: arg_wait=%d\n",arg_wait );
        printf( "Server-processing-parent: return_wait=%d\n",retorno_wait);
        data_size=strlen(buffer);
        if(data_size>0){
            *data=(char*)realloc(*data,sizeof(char)*data_size);
            *data=strcpy(*data,buffer);
        }else{
            *data=(char*)realloc(*data,sizeof(char)*3);
            *data=strcpy(*data,"ok\n");
        }
        return child_pid;
    }
    else{
        close(pipefd[0]);
        dup2(pipefd[1],1);
        dup2(pipefd[1],2);
        close(pipefd[1]);
        execvp(program, arg_list);
        fprintf(stderr, "An error occurred in execvp\n");
        abort();
    }
}