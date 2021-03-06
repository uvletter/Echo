#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define CONNECT_NUM 5
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
typedef struct sockaddr SA;

static void
err_print(const char *err)
{
	fprintf(stderr, "errno:%d %s\n%s\n", errno, strerror(errno), err);
	exit(-1);
}

static void
send_msg(int socket)
{
	char input[1024];
	size_t len;

	while(fgets(input, 1024, stdin)){
		write(socket, input, strlen(input));
		if((len = read(socket, input, 1024)) == EOF)
			err_print("server terminated unexpectedly");
		input[len] = 0;
		fputs(input, stdout);
	}
}

int
main()
{
	struct sockaddr_in servaddr;
	int connfd[CONNECT_NUM];
	int i;
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr.s_addr);

	for(i = 0; i<CONNECT_NUM; i++){
		connfd[i] = socket(AF_INET, SOCK_STREAM, 0);
		if(connfd[i] < 0)
			err_print("Cannot create socket");
		if((connect(connfd[i],  (SA *)&servaddr, sizeof(servaddr)))<0)
			err_print("Connect error");
	}
	send_msg(connfd[0]);
	exit(0);
}
