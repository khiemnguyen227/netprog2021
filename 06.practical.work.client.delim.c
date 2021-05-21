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

#define PORT 8784

int main(int argc, char **argv)
{
struct hostent *h;
struct sockaddr_in saddr;
int sockfd;
unsigned short port = 8784;
    if(argc == 2) {
	h = gethostbyname(argv[1]);
	}
	else {
	    printf("%s\n", " Please enter hostname: ");
	    char str[50];
	    scanf("%s", str);
	    h = gethostbyname(str);
	}

        printf("IP address is: \n");

    if (!h) {
        printf("Cannot find IP");
    }
    else {
	for (unsigned int i=0; h->h_addr_list[i] != NULL; i++) {
	    printf("%s\n", inet_ntoa( *(struct in_addr*)(h->h_addr_list[i])));
        }
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { 
        printf("Error creating socket\n"); 
        exit(-1); 
    }

    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char*) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    saddr.sin_port = htons(port);
    if(connect(sockfd, (struct sockaddr *) &saddr,sizeof(saddr)) <0) {
        printf("Cannot connect to server.\n"); 
        exit(-1); 
    }

    printf("Connected to the server.\n");
    
    while (1) {
        char m[500];
        char rep[500];
        //print client mess
        printf("client > ");
        fgets(m, 500, stdin);
        m[strlen(m) - 1] = 0;
        send(sockfd, m, strlen(m)+ 1, 0);
        //print server mess
        printf("server > ");
        fflush(stdout);
        recv(sockfd, rep, 500, 0);
        printf("%s\n", rep);
    }
    close(sockfd);

    return 0;
    
}
