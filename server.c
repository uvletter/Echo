#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<signal.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
typedef struct sockaddr SA;

static void
err_print(const char *err)
{
	fprintf(stderr, "errno:%d %s\n%s\n", errno, strerror(errno), err);
	exit(-1);
}

int
main()
{
	struct sockaddr_in servaddr;
	int listenfd,connfd;
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVADDR);
	inet_pton(AF_INET, SERVER_IP, &seraddr.sin_addr);

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		err_print("Cannot create socket");
}

}
