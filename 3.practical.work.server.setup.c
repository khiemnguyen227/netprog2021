#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


int main (int argc, char const *argv[]){
        int sockfd, clen, clientfd;
        struct sockaddr_in saddr, caddr;
        unsigned short port = 8784;
        if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("Error creating socket\n");
                exit(-1);
        }

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
        else
	{
                printf("An user is waiting for connect\n");
                
	}

        return 0;
}
