# include "tcpserverutil.h"


//ssize_t readn(int fd, void *pstr, size_t n);
void str_echo(int sockfd);
void str_echo_select(struct pollfd *read_pollfd);



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
    long open_max = sysconf(_SC_OPEN_MAX );
    struct pollfd client[open_max];
    int i;
    for(i = 0; i < open_max; i++) {
        client[i].fd = -1;
    }
    client[0].fd = sockfd;
    client[0].events = POLLRDNORM;
    int maxi = 0;
    int poll_ret=0;

    int connect_fd = 0;
    socklen_t cli_len = 0;
    
    for(;;)
    {
        poll_ret = poll(client, maxi+1, -1); //INFTIM
        if(poll_ret < 0) {
            printf("error of poll\n");
            exit(-1);
        }
        if(client[0].revents & POLLRDNORM) {
            // has new connect
            connect_fd = accept(sockfd, (struct sockaddr_in *) &addr_cli, &cli_len);
            if(connect_fd == -1) {
                printf("accept error ,continue\n");
                continue;
            }
            printf("connection from %s, port %d\n", inet_ntop(AF_INET, &addr_cli.sin_addr, buff, sizeof(buff)), 
                ntohs(addr_cli.sin_port));
            
            for(i=0; i<open_max; i++) {
                if(client[i].fd == -1) {
                    client[i].fd = connect_fd;
                    break;
                }
            }
            if(i == open_max) {
                err_sys("too open_max\n");
            }
            client[i].events = POLLRDNORM;
            if(i > maxi)
                maxi = i;

            printf("new conn fd: %d\n", client[i].fd);
            
            
            if(poll_ret == 1){ // only one new connect, no data from client
                continue;
            }
        }
        for(i = 1; i <= maxi; i++) {
            if(client[i].fd > 0 && (client[i].revents & (POLLRDNORM | POLLERR))) {

                str_echo_select(client+i);
            }
        }

    }
    return 0;
}

void str_echo_select(struct pollfd *read_pollfd) {
    ssize_t n;
    char buf[MAXLINE];
    int sockfd;
    sockfd = (*read_pollfd).fd;
    printf("start read %d\n", sockfd);
    
    
    if((n = read(sockfd, buf, MAXLINE)) == 0) {
        close(sockfd);
        (*read_pollfd).fd = -1;
        printf("close fd:%d\n", sockfd);
        
    } else if (n < 0){
        if(errno == ECONNRESET) {
            close(sockfd);
            (*read_pollfd).fd = -1;
            printf("close RESET fd:%d\n", sockfd);
        } else {
            err_sys("read err\n");
        }
        
    } else {
        printf("bb:%d\n", n);
        write(sockfd, buf, n);
    }
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