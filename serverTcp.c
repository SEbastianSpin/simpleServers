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



ssize_t bulk_read(int fd, char *buf, size_t count){
	int c;
	size_t len=0;
	do{
		c=TEMP_FAILURE_RETRY(read(fd,buf,count));
		if(c<0) return c;
		if(0==c) return len;
		buf+=c;
		len+=c;
		count-=c;
	}while(count>0);
	return len ;
}
int add_new_client(int sfd){
	int nfd;
	if((nfd=TEMP_FAILURE_RETRY(accept(sfd,NULL,NULL)))<0) {
		if(EAGAIN==errno||EWOULDBLOCK==errno) return -1;
		ERR("accept");
	}
	return nfd;
}

void usage(char * name){
	fprintf(stderr,"USAGE: %s port\n",name);
}

int main(int argc, char** argv) {
	printf("Start\n");
	
	int fd;
	if(sethandler(SIG_IGN,SIGPIPE)) ERR("Seting SIGPIPE:");

    struct sockaddr_un addr;
    
    if(unlink(argv[1]) <0&&errno!=ENOENT) ERR("unlink");

    if((fd = socket(PF_UNIX,SOCK_STREAM,0))<0) ERR("socket");
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path,argv[1],sizeof(addr.sun_path)-1);

    if(bind(fd,(struct sockaddr*) &addr,SUN_LEN(&addr)) < 0)  ERR("bind");
	if(listen(fd, BACKLOG) < 0) ERR("listen");
	printf("Listening\n");

    sigset_t mask, oldmask;
    sigemptyset (&mask);
	sigaddset (&mask, SIGINT);
	sigprocmask (SIG_BLOCK, &mask, &oldmask);

    fd_set base_rfds, rfds ;
    FD_ZERO(&base_rfds);
	FD_SET(fd, &base_rfds);

    socklen_t size=sizeof(addr);;

	int32_t data[5];

    int cfd;

	for(;;){
    rfds=base_rfds;
		if(pselect(fd+1,&rfds,NULL,NULL,NULL,&oldmask)>0){
			if((cfd=add_new_client(fd))>=0){

			if((size=bulk_read(cfd,(char *)data,sizeof(int32_t[5])))<0) ERR("read:");
				if(size==(int)sizeof(int32_t[5])){
					for(int c=0;c<5;c++){

                        printf("%d",data[c]);
                    }
                    fflush(stdout); 

				}
				if(TEMP_FAILURE_RETRY(close(cfd))<0)ERR("close");
		}else{
			if(EINTR==errno) continue;
			ERR("pselect");
		}
		
	}

}
}