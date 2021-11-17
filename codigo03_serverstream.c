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

/* the port users will be connecting to */
#define MYPORT 3490

/* how many pending connections queue will hold */
#define BACKLOG 10

#define MAXDATASIZE 300
int main(int argc, char *argv[ ]){
    int numbytes=0;
    char buf[MAXDATASIZE];
    /* listen on sock_fd, new connection on new_fd */
    int sockfd, new_fd;

    /* my address information */
    struct sockaddr_in my_addr;

    /* connectors address information */
    struct sockaddr_in their_addr;
    int sin_size;

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

    /* zero the rest of the struct */memset(&(my_addr.sin_zero), '\0', 8);
    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
        printf("Server-bind() error");
        exit(1);
    }
    else
    printf("Server-bind() is OK...\n");

    if(listen(sockfd, BACKLOG) == -1){
        printf("Server-listen() error");
        exit(1);
    }

    printf("Server-listen() is OK...Listening...\n");
    /* clean all the dead processes */

    sin_size = sizeof(struct sockaddr_in);
    if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
        printf("Server-accept() error");
        exit(1);
    }

    printf("Server-accept() is OK...\n");
    printf("Server-new socket, new_fd is OK...\n");
    printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));
    //Leyendo
    if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1){
        printf("recv()");
        exit(1);
    }
    else
        printf("Server-The recv() is OK...\n");
    buf[numbytes] = '\0';
    printf("Server-Received: %s", buf);
    printf("Server-Closing sockfd\n");
    // if(send(new_fd, "This is a test string from server!\n", 37, 0) == -1)
    //   printf("Server-send() error lol!");
    // else
    //   printf("Server-send is OK...!\n");

    close(new_fd);
    close(sockfd);
    printf("Server-new socket, new_fd closed successfully...\n");
    return 0;
}
