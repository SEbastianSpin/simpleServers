#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <fcntl.h>
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

int add_new_client(int sfd){
	int nfd;
	if((nfd=TEMP_FAILURE_RETRY(accept(sfd,NULL,NULL)))<0) {
		if(EAGAIN==errno||EWOULDBLOCK==errno) return -1;
		ERR("accept");
	}
	printf("New connection \n");
	return nfd;
}

// Function designed for chat between client and server.

void doServer(int fdL){
	int cfd,fdmax;
	fd_set base_rfds, rfds ;
	sigset_t mask, oldmask;
	FD_ZERO(&base_rfds);
	FD_SET(fdL, &base_rfds);

	sigemptyset (&mask);
	sigaddset (&mask, SIGINT);
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
	while(1){
		rfds=base_rfds;
		if(pselect(fdL+1,&rfds,NULL,NULL,NULL,&oldmask)>0){
			if(FD_ISSET(fdL,&rfds)) cfd=add_new_client(fdL);
			
		}else{
			if(EINTR==errno) continue;
			ERR("pselect");
		}
	}
	sigprocmask (SIG_UNBLOCK, &mask, NULL);
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

    sockfd=make_socket(AF_INET,SOCK_STREAM);
	bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
    int t=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&t, sizeof(t))) ERR("setsockopt");

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr)))!=0)ERR("bind");
	printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
	if ((listen(sockfd, numOfClients)) != 0)ERR("Listen");

	
    printf("Server listening..\n");
    


	doServer(sockfd);


    close(sockfd);

}