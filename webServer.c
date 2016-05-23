#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>

#define BUFSIZE 4096
#define PORT 8080
#define MAXPEND 1000 

int main(){
	int servSoc, newSoc, sd, i;
	FILE *fp;
	struct sockaddr_in servAddr, clntAddr;
	int reclen, sendLen, socklen;

	char recvBuf[BUFSIZE];
	char *what, state;

	struct epoll_event *event;
	struct epoll_event ev;

	int epfd, clntev, option = 1;

	what = (char *)malloc(4096);
	event = malloc(sizeof(struct epoll_event));
	memset(&servAddr, 0, sizeof(servAddr));
	memset(&clntAddr, 0, sizeof(clntAddr));
	servSoc = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(servSoc, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(PORT);
	bind(servSoc, (struct sockaddr*)&servAddr, sizeof(servAddr));
	listen(servSoc, 1);
	epfd = epoll_create(1);
	ev.events = EPOLLIN | EPOLLHUP | EPOLLET;
	ev.data.fd = servSoc;
	epoll_ctl(epfd, EPOLL_CTL_ADD, servSoc, &ev);
	while(1) {
		clntev = epoll_wait(epfd, event, 1, 10);
		if(event[clntev].data.fd == servSoc) {
			if((i=fork()) > 0){
				event[clntev].data.fd = 1;
			}
			else if(i==0){
				socklen = sizeof(clntAddr);
				newSoc = accept(servSoc, (struct sockaddr*)&clntAddr, &socklen);

				recv(newSoc, recvBuf, BUFSIZE, 0);
				printf("%s\n", recvBuf);
				if(!strcmp(recvBuf, "GREEN") || !strcmp(recvBuf, "RED")){
					fp = fopen("./tmp.txt", "w");
					fprintf(fp, "%s", recvBuf);
					fclose(fp);
				}

				else{	
					fp = fopen("./tmp.txt", "r");
					state = fgetc(fp);

					if(state == 'G'){
						write(newSoc, "Green", 5);
					}
					else{
						write(newSoc, "Red", 3);
					}
				}
				close(newSoc);
				exit(1);
			}
			else
				exit(1);
		}
	}
	close(servSoc);
	return 0;
}
