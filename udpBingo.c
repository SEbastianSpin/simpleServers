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

struct sockaddr_in make_address(char *address, char *port){
	int ret;
	struct sockaddr_in addr;
	struct addrinfo *result;
	struct addrinfo hints = {};
	hints.ai_family = AF_INET;
	if((ret=getaddrinfo(address,port, &hints, &result))){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		exit(EXIT_FAILURE);
	}
	addr = *(struct sockaddr_in *)(result->ai_addr);
	freeaddrinfo(result);
	return addr;
}

int make_socket(int domain, int type){
	int sock;
	sock = socket(domain,type,0);
	if(sock < 0) ERR("socket");
	return sock;
}

void usage(char * name){
	fprintf(stderr,"USAGE: %s port\n",name);
}

void doServer(int fd, int fdout, char* bingoMessage,struct sockaddr_in addrOUT){

    printf("Working Server message: %s\n",bingoMessage);//STDIN_FILENO


	int fdmax;
	fd_set base_rfds,base_wfds, rfds ,wfds;
	
	FD_ZERO(&base_rfds);
    FD_ZERO(&base_wfds);

	FD_SET(fd, &base_rfds);
    FD_SET(STDIN_FILENO, &base_rfds);
    FD_SET(fdout, &base_wfds);

    fdmax=(fd>STDIN_FILENO?fd:STDIN_FILENO);
    fdmax=(fdmax>fdout?fdmax:fdout);
 
    sigset_t mask, oldmask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGINT);
	sigprocmask (SIG_BLOCK, &mask, &oldmask);

    char buf[MAXBUF]={0};
    struct sockaddr_in addr;
    socklen_t size=sizeof(addr);;
    
	while(1){
		rfds=base_rfds;
        wfds=base_wfds;

        int num_fds =pselect(fdmax+1,&rfds,&wfds,NULL,NULL,&oldmask);
		if(num_fds<0){
		if(EINTR==errno) continue;
			ERR("pselect");
		}else{
			
            if(FD_ISSET(fd,&rfds)){
             if(TEMP_FAILURE_RETRY(recvfrom(fd,buf,MAXBUF,0,&addr,&size)<0)) ERR("read:");
             printf("%s \n",buf);
            }

            if(FD_ISSET(STDIN_FILENO,&rfds)){
             printf("We should send\n");
                int n = 0;
                char buff[MAXBUF];
                while ((buff[n++] = getchar()) != '\n');
                if(TEMP_FAILURE_RETRY(sendto(fdout,buff,MAXBUF,0,&addrOUT,sizeof(addrOUT))<0)) ERR("sendto:");
                printf("We sent %s \n",buff);
            }
            
		}
	}
	sigprocmask (SIG_UNBLOCK, &mask, NULL);
}


int main(int argc, char** argv) {
	printf("Start\n");
	
	int fd;
	if(sethandler(SIG_IGN,SIGPIPE)) ERR("Seting SIGPIPE:");

    if(argc<3)usage(argv[0]);
    int portnum=atoi(argv[2]);
    char* direcction=argv[4];
    char* portnumOut=argv[5];
    char* bingoMessage=argv[3];
    fd=make_socket(PF_INET,SOCK_DGRAM);
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portnum);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int t=1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,&t, sizeof(t))) ERR("setsockopt");
	if(bind(fd,(struct sockaddr*) &addr,sizeof(addr)) < 0)  ERR("bind");


	if(SOCK_STREAM==SOCK_DGRAM)if(listen(fd, BACKLOG) < 0) ERR("listen");

	printf("Listening\n");


    int fdo=make_socket(PF_INET,SOCK_DGRAM);
	
	struct sockaddr_in direcionadd=make_address(direcction,portnumOut);
	doServer(fd,fdo,bingoMessage,direcionadd);

}