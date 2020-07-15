//tcpServer.c -- a simple stream socket server program.

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>

#define PORT "8080"     //the port user will be connecting to.
    
#define BACKLOG 10      //maximum pending connections in the queue.


//get sockadd, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if(sa -> sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa) -> sin_addr);
    }

    return &(((struct sockaddr_in6*)sa) -> sin6_addr);
}

int main(void)
{
    int sockfd, new_fd;     //listen on sock_fd and connect through new_fd.
    struct sockaddr_storage address;    //connector's address;
    struct addrinfo hints, *serverinfo = NULL, *ptr = NULL;
    
    int opt = 1;
    int addrlen = sizeof(address);

    int result;
    char *msg = "Hello from TCP-Server!";

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, PORT, &hints, &serverinfo);
    ptr = serverinfo;

    //creating socket file descriptor.
    if((sockfd = socket(ptr -> ai_family, ptr -> ai_socktype, ptr -> ai_protocol)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //reuse of address and port.
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //forcefully attaching socket to the PORT.
    if(bind(sockfd, ptr -> ai_addr, ptr -> ai_addrlen) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(serverinfo);

    if(ptr == NULL)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("server: waiting for connection...\n");

    new_fd = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    if(new_fd == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    send(new_fd, msg, strlen(msg), 0);

    close(sockfd);
    close(new_fd);

return 0; 
}