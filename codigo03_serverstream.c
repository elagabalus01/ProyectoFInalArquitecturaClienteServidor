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
/* the port users will be connecting to */
#define MYPORT 3490

/* how many pending connections queue will hold */
#define BACKLOG 10

#define MAXDATASIZE 10000

#define MAXRESPONSESIZE 10000

void manejador_senales(int sig);
int run_command(char* program, char** arg_list,char **data);

int sockfd;
int main(int argc, char *argv[ ]){
    int aux=0;
    int numbytes=0;
    int hijo_id=0;
    char *command=(char*)malloc(MAX_NAME_SZ);
    char **exec_args;
    char buf[MAXDATASIZE];
    /* listen on sock_fd, new connection on new_fd */
    // int sockfd, new_fd;
    int new_fd;
    char *data=(char*)malloc(MAX_NAME_SZ); //Almacena la respuesta del servidor que ejecuta el comando
    /* my address information */
    struct sockaddr_in my_addr;

    /* connectors address information */
    struct sockaddr_in their_addr;
    int sin_size;

    if( signal(SIGINT, manejador_senales) == SIG_ERR ){
        printf("no puedo cachar SIGINT\n");
        perror("signal");
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("Server-socket() error lol!");
        exit(1);
    }
    else
    printf("Server-socket() sockfd is OK...\n");

    /* host byte order */
    my_addr.sin_family = AF_INET;

    /* short, network byte order */
    my_addr.sin_port = htons(MYPORT);

    /* automatically fill with my IP */
    my_addr.sin_addr.s_addr = INADDR_ANY;
    printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);

    /* zero the rest of the struct */
    memset(&(my_addr.sin_zero), '\0', 8);
    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
        printf("Server-bind() error");
        exit(1);
    }
    else
    printf("Server-bind() is OK...\n");

    if(listen(sockfd, BACKLOG) == -1){
        printf("Server-listen() error");
        exit(1);
    }else
        printf("Server-listen() is OK...Listening...\n");
    sin_size = sizeof(struct sockaddr_in);

    /* clean all the dead processes */
    if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
        printf("Server-accept() error");
        exit(1);
    }
    printf("Server-accept() is OK...\n");
    printf("Server-new socket, new_fd is OK...\n");
    while(1){
        printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));
        //Leyendo
        if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1){
            printf("recv()\n");
            printf("Error:%s\n",strerror(errno));
            exit(1);
        }
        else
            printf("Server-The recv() is OK...\n");
        buf[numbytes] = '\0';
        printf("Procesando comando %s\n",buf);
        command=realloc(command,strlen(buf)*sizeof(char));
        command=strcpy(command,buf);
        printf("Procesando comando %s\n",strerror(errno));
        puts("Paso el strcpy");
        exec_args=split(command);
        printf("Server-Received: %s\n", buf);
        hijo_id=run_command(exec_args[0],exec_args,&data);
        printf("Se ejecuto el comando con el hijo %ld\n",hijo_id);
        printf("Server-sends: Enviando respuesta comando\n %s", data);
        // if(send(new_fd, "This is a test string from server!\n", 37, 0) == -1)
        //   printf("Server-send() error lol!");
        // else
        //   printf("Server-send is OK...!\n");
        if(aux=send(new_fd, data, 10000-1, 0) == -1)
            printf("Server-send() error lol!");
        else{
            printf("Server-send is OK...!\n");
            printf("Se enviaron %d bytes\n",aux);
        }
    }
    close(new_fd);
    printf("Server-new socket, new_fd closed successfully...\n");
    close(sockfd);
    printf("Server-socket, sockfd closed successfully...\n");
    return 0;
}
void manejador_senales(int sig){
    puts("Comprobando excepcion");
    switch (sig) {
        case SIGINT:
            close(sockfd);
            printf("Cerrando el socket %i\n",sockfd);
            puts("Apagando el servidor...");
            puts("BYE");
            exit(0);
        break;
        defaulf:
            puts("Otra señal");
        break;
    }
}
int run_command(char* program, char** arg_list,char **data){
    pid_t child_pid;
    int arg_wait;
    int data_size=0;
    int pipefd[2];//Defines a fd for the ends of pipe (read,write)
    pipe(pipefd);//Asigns the fd of the pipe
    child_pid = fork();
    if (child_pid != 0){ //If is the parent
        close(pipefd[1]); //Close writing end
        char buffer[MAXRESPONSESIZE]={};//Buffer for reading pipe
        read(pipefd[0], buffer, sizeof(buffer));
        fsync(pipefd[0]);
        printf("IMpresión Buffer: %s\n",buffer);
        int retorno_wait =  wait( &arg_wait );
        printf( "Proceso padre, despues de wait\n" );
        printf( "Proceso padre, el valor &arg_wait=%X\n",&arg_wait );
        printf( "Proceso padre, la variable arg_wait=%d\n",arg_wait );
        printf( "Proceso padre, la variable retorno_wait=%d\n",retorno_wait);
        data_size=strlen(buffer);
        *data=(char*)realloc(*data,sizeof(char)*data_size);
        *data=strcpy(*data,buffer);
        // *data=buffer;
        return child_pid;
    }
    else{
        close(pipefd[0]);//Close pipe's read end
        dup2(pipefd[1],1);//Redirects default output to pipe write end
        dup2(pipefd[1],2);//Redirects default error to pipe write end
        close(pipefd[1]);//Close pipe's write end
        execvp(program, arg_list);
        fprintf(stderr, "an error occurred in execvp\n");
        abort();
    }
}
