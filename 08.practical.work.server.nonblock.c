#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h> 
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>


int main (int argc, char const *argv[]){
        int sockfd, clen, clientfd;
        struct sockaddr_in saddr, caddr;
        unsigned short port = 8784;
        if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("Error creating socket\n");
                exit(-1);
        }

        //Reusing address
        setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

        //make clientfd non-blocking
        int flags = fcntl(clientfd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        fcntl(clientfd, F_SETFL, flags);

        memset(&saddr, 0,sizeof(saddr));
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = htonl(INADDR_ANY);
        saddr.sin_port = htons(port);

        if((bind(sockfd, (struct sockaddr *) &saddr,sizeof(saddr))) < 0) {
                printf("Error binding\n");
                exit(-1);
        }

        if(listen(sockfd, 5) < 0) {
                printf("Error listening\n");
                exit(-1);
        }

        clen=sizeof(caddr);
        if((clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen)) < 0) {
                printf("Error accepting connection\n");
                exit(-1);
        }
        else{
                printf("An user is waiting for connect\n");
            
	}

        //check if the client has typed something
        struct pollfd mypoll = { STDIN_FILENO, POLLIN };
        switch (poll(&mypoll, 1, 1000)) {
                case -1:
                        fprintf(stderr, "Poll Failed");
                        exit(-1);
                case 0:
                        break;
        }

        //message
        char m[500];
        char rep[500];
        while (1) {
                //print client mess
    	        printf("client > ");
                recv(clientfd, m, 500, 0);
                printf("%s\n", m);
                //print server mess
                printf("server > ");
                fgets(rep, 500, stdin);
                //server "dc" disconnect
                if (strncmp(rep, "/dc", 3) == 0) {
                        return 0;
                }
                
                rep[strlen(rep) - 1] = 0;
                send(clientfd, rep, strlen(rep) + 1, 0);
        }
        shutdown(clientfd, SHUT_RDWR);
        close(clientfd);
        close(sockfd);

        return 0;
        
}
