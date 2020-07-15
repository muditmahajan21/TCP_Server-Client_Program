//tcpClient.c -- a simple stream socket client program.

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

#define PORT "8080"     //the port client will be connecting to.

#define MAXDATASIZE 1024     //max number of bytes we will get at once.

//get sockadd, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if(sa -> sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa) -> sin_addr);
    }

    return &(((struct sockaddr_in6*)sa) -> sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numBytes;
    char buffer[MAXDATASIZE];
    
    struct addrinfo hints, *serverinfo = NULL, *ptr = NULL;
    int result;

    char straddr[INET6_ADDRSTRLEN];

    if(argc != 2)
    {
        fprintf(stderr, "usage: client hostname\n");
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    result = getaddrinfo(argv[1], PORT, &hints, &serverinfo);
    ptr = serverinfo;

    //loop through all the results and connect to the first one.
    for(ptr = serverinfo; ptr != NULL; ptr = ptr -> ai_next)
    {   
        //creating socket file descriptor.
        if((sockfd = socket(ptr -> ai_family, ptr -> ai_socktype, ptr -> ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        //connecting socket file descriptor.
        if(connect(sockfd, ptr -> ai_addr, ptr -> ai_addrlen) == -1)
        {
            perror("client: connect");
            continue;
        }

        break;
    }

    if(ptr == NULL)
    {
        printf("client failed to connect\n");
        return -1;
    }

    //convert IPv4 and IPv6 addresses from binary to text.
    inet_ntop(ptr -> ai_family, get_in_addr((struct sockaddr *)ptr -> ai_addr), straddr, sizeof straddr);
    printf("client: connecting to %s\n", straddr);

    freeaddrinfo(serverinfo);

    if ((numBytes = recv(sockfd, buffer, MAXDATASIZE, 0)) == -1)
    {
        perror("recieve");
        exit(EXIT_FAILURE);
    }

    buffer[numBytes] = '\0';

    printf("client: recieved '%s'\n", buffer);

    close(sockfd);

return 0;
}

