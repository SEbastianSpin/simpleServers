#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define ERR(source) (perror(source),\
		fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
		exit(EXIT_FAILURE))


int make_socket(int domain, int type){
	int sock;
	sock = socket(domain,type,0);
	if(sock < 0) ERR("socket");
	return sock;
}



// Function designed for chat between client and server.
void func(int connfd)
{
	char buff[MAX];
	int n;
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("From client: %s\t To client : ", buff);
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n')
			;

		// and send that buffer to client
		write(connfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

void mapDisplayer(int size,int map[][size]){

for(int r=0;r<size;r++){
    for(int c=0;c<size;c++){
        if(map[r][c]!=0)printf("[%d]",map[r][c]);
        else printf("[ ]");
    }
    printf("\n");
}
}

void usage(char * name){
	fprintf(stderr,"USAGE: %s MAP size [10 20] num of CLients[2 5]\n",name);
}


int main(int argc, char** argv) {

	int sockfd, connfd, len, mapsize, numOfClients;
    struct sockaddr_in servaddr, cli;
    mapsize=atoi( argv[1]);
    if(!(9<mapsize&&20>mapsize)){
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    numOfClients=atoi(argv[2]);

    

    int map[mapsize][mapsize];
    memset(&map, 0, sizeof(map));
    printf("Map Created\n");
    map[0][0]=8;
    mapDisplayer(mapsize,map);

    sockfd=make_socket(PF_INET,SOCK_DGRAM);

    servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);


    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

}