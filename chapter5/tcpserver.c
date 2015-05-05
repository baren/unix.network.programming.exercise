# include "tcpserverutil.h"


//ssize_t readn(int fd, void *pstr, size_t n);
void str_echo(int sockfd);



int main(int argc, char **argv)
{
    int sockfd=0;
    char buff[MAXLINE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    struct sockaddr_in addr_cli;
    bzero(&addr, sizeof(struct sockaddr_in));
    
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    addr.sin_port = htons(9090);
    addr.sin_family = AF_INET; // todo need ?
    int ret = 0;
    if((ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) < 0)
    {
    	err_sys("bind error");
    }
    int lis_ret = 0;
    if((lis_ret = listen(sockfd, 5)) < 0)
    {
    	err_sys("listen error");
    }
    int connect_fd = 0;
    socklen_t cli_len = 0;
    for(;;)
    {
    	connect_fd = accept(sockfd, (struct sockaddr_in *) &addr_cli, &cli_len);
    	if(connect_fd == -1)
    	{
    		printf("accept error ,continue\n");
    		continue;
    	}
    	printf("connection from %s, port %d\n", inet_ntop(AF_INET, &addr_cli.sin_addr, buff, sizeof(buff)), 
    		ntohs(addr_cli.sin_port));
    	pid_t pid;
    	if((pid=fork()) == 0)
    	{
    		printf("pid:%d\n", pid);
    		close(sockfd);
    		str_echo(connect_fd);
    		exit(0);
    	}
    	close(connect_fd);




    }
    return 0;
}

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	again:
	printf("start\n");
	while((n=read(sockfd, buf, MAXLINE)) > 0)
	{
		printf("aa:%d\n", n);
		write(sockfd, buf, n);
	}
	printf("bb:%d\n", n);
	if (n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		err_sys("read error");
}