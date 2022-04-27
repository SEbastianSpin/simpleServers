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
#define ERR(source) (perror(source),\
		     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
		     exit(EXIT_FAILURE))
#define BACKLOG 3
#define MAXBUF 576
#define MAXADDR 5


int sethandler( void (*f)(int), int sigNo) {
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = f;
	if (-1==sigaction(sigNo, &act, NULL))
		return -1;
	return 0;
}


void usage(char * name){
	fprintf(stderr,"USAGE: %s port\n",name);
}

int main(int argc, char** argv) {
	printf("Start\n");
	
	int fd;
	if(sethandler(SIG_IGN,SIGPIPE)) ERR("Seting SIGPIPE:");

	fd=socket(PF_INET,SOCK_DGRAM,0);
	if(fd < 0) ERR("socket");
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int t=1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,&t, sizeof(t))) ERR("setsockopt");
	if(bind(fd,(struct sockaddr*) &addr,sizeof(addr)) < 0)  ERR("bind");


	if(SOCK_STREAM==SOCK_DGRAM)if(listen(fd, BACKLOG) < 0) ERR("listen");

	printf("Listening\n");

	socklen_t size=sizeof(addr);;

	char buf[MAXBUF]={0};
	
	for(;;){
		printf("s\n");//STDIN_FILENO


		
		if(TEMP_FAILURE_RETRY(recvfrom(fd,buf,MAXBUF,0,&addr,&size)<0)) ERR("read:");
		printf("%s \n",buf);
		
		
		
	}

}