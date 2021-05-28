#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>

int main (int argc, char const *argv[]){
	int sockfd, clen, clientfd;
	int const max_client = 100;
	int clientfds[max_client];
	struct sockaddr_in saddr, caddr;
	unsigned short port = 8784;
	char s[256];

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error creating socket!\n");
		close(sockfd);
		return -1;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

	int fl = fcntl(sockfd, F_GETFL, 0);
	fl |= O_NONBLOCK;
	fcntl(sockfd, F_SETFL, fl);

	memset(clientfds, 0, sizeof(clientfds));

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);

	if((bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr))) <0){
		printf("Error in binding\n");
		close(sockfd);
		return -1;
	}

	if(listen(sockfd, 5) <0){
		printf("Error listening\n");
		close(sockfd);
		return -1;
	}

	clen = sizeof(caddr);

	while(1){
		fd_set set; // declaration of the set
		FD_ZERO(&set); //// clear the set
		FD_SET(sockfd, &set); // add listening sockfd to set

		int maxfd = sockfd; // a required value to pass to select()
		for (int i = 0; i < max_client; i++){
			// add connected client sockets to set
			if(clientfds[i] > 0){
				FD_SET(clientfds[i], &set);
			}

			if(clientfds[i] > maxfd){
				maxfd = clientfds[i];
			}
		}

		select(maxfd + 1, &set, NULL, NULL, NULL);

		// a «listening» socket?

		if (FD_ISSET(sockfd, &set)){
			clientfd = accept(sockfd, (struct sockaddr *) &saddr, &clen);

			// make something nonblocking
			int fl = fcntl(clientfd,F_GETFL, 0);
			fl |= O_NONBLOCK;
			fcntl(clientfd, F_SETFL, fl);

			// add it to the clientfds array
			for (int i=0; i< max_client; i++){
				if (clientfds[i] == 0){
					clientfds[i] = clientfd;
					break;
				}
			}

			for (int i=0; i< max_client; i++){
				if(clientfds[i]>0 && FD_ISSET(clientfds[i], &set)){
					if(read(clientfds[i], s, sizeof(s)) > 0){
						printf("Client %d said: %s\n", clientfds[i], s);
					}
					else{
					// some error, remove it from the "active" fd array
					printf("client %d has disconnected.\n", clientfds[i]);
					clientfds[i] = 0;
					}	
				}
			}	
		}
	

		//message
		char m[500];
		char rep[500];
		while (1) {
			//print client mess
			printf("client > ");
			recv(clientfd, m, 500, 0);
			m[500 - 1] = 0;
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
	}	
	shutdown(clientfd, SHUT_RDWR);
	close(clientfd);
	close(sockfd);

	return 0;	
	}

