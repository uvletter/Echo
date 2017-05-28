#include<sys/socket.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<signal.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
typedef struct sockaddr SA;
typedef void (*sighandler_t)(int);

static void
err_print(const char *err)
{
	fprintf(stderr, "%s : %s\n", err, strerror(errno));
	fflush(stderr);
	exit(-1);
}

static sighandler_t
signal_s(int signum, sighandler_t handler)
{
	struct sigaction action, old_action;

	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESTART;

	if(sigaction(signum, &action, &old_action) < 0)
		return SIG_ERR;
	return old_action.sa_handler;
}

static void
clean_process(int signum)
{
	pid_t pid;
	fprintf(stderr, "Begin to collect zombie process\n");
	while((pid = waitpid(-1, NULL, WNOHANG)) > 0)
	//while((pid = wait(NULL))>0)
		printf("Thread %d is closed\n", pid);
	if(pid == -1 && errno != ECHILD)
		err_print("wait error");
}

static void
quit(int signum)
{
	printf("Thread %d terminated by user\n", getpid());
	//sleep(1);
	exit(0);
}

static void
echo(int sockfd)
{
	char buf[1024];
	ssize_t len;

	for(;;){
		while((len = read(sockfd, buf, 1024)) > 0)
			write(sockfd, buf, len);
		if(len ==0)
			break;
		else
			err_print("read error");
	}
}

int
main()
{
	struct sockaddr_in servaddr, clientaddr;
	socklen_t len;
	int listenfd,connfd;
	pid_t pid;
	char clientip[16];
	
	if(signal_s(SIGCHLD, clean_process) == SIG_ERR)
		err_print("signal error");

	if(signal_s(SIGINT, quit) == SIG_ERR)
		err_print("signal error");

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0)
		err_print("Cannot create socket");

	len = sizeof(servaddr);
	if(bind(listenfd, (SA *)&servaddr, len) < 0)
		err_print("bind error");

	if(listen(listenfd, 10) < 0)
		err_print("listen error");

	while(1){
		if((connfd = accept(listenfd, (SA *)&clientaddr, &len)) < 0)
			err_print("accept error");

		if((pid = fork()) < 0)
			err_print("fork error");
		else if(pid == 0){
			inet_ntop(AF_INET, &clientaddr.sin_addr, clientip, sizeof(clientip));
			printf("Connect to %s:%d\n", clientip, ntohs(clientaddr.sin_port));
			close(listenfd);

			echo(connfd);
			exit(0);
		}
		else{
			printf("Fork a new thread %d\n", pid);
			close(connfd);
		}
	}
}
