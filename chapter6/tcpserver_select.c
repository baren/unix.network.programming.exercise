# include "tcpserverutil.h"


//ssize_t readn(int fd, void *pstr, size_t n);
void str_echo(int sockfd);
void str_echo_select(int sockfd, fd_set *rset, int *client_bit);



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

    fd_set rset, wset;
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    int select_ret = 0;
    int connfd_array[FD_SETSIZE];
    int i;
    // init array
    for(i=0; i < FD_SETSIZE; i++) {
        connfd_array[i] = -1;
    }
    int maxfd = sockfd;

    int connect_fd = 0;
    socklen_t cli_len = 0;
    int for_sockfd=0;
    
    for(;;)
    {
        FD_SET(sockfd, &rset);
        printf("begin for select\n");
        select_ret = select(maxfd+1, &rset, NULL, NULL, NULL);
        printf("after select\n");
        if(select_ret < 0) {
            printf("select func err.\n");
            exit(0);
        }
        if(FD_ISSET(sockfd, &rset)) { // has new connect
            connect_fd = accept(sockfd, (struct sockaddr_in *) &addr_cli, &cli_len);
            if(connect_fd == -1) {
                printf("accept error ,continue\n");
                continue;
            }
            printf("connection from %s, port %d\n", inet_ntop(AF_INET, &addr_cli.sin_addr, buff, sizeof(buff)), 
                ntohs(addr_cli.sin_port));
            FD_SET(connect_fd, &rset);
            for(i=0; i<FD_SETSIZE; i++) {
                if(*(connfd_array+i) == -1) {
                    *(connfd_array+i) = connect_fd;
                    printf("new conn fd: %d\n", *(connfd_array+i));
                    break;
                }
            }
            if(connect_fd > maxfd) {
                maxfd = connect_fd;
                printf("maxfd: %d\n", maxfd);
            }
            if(select_ret == 1){ // only one new connect, no data from client
                continue;
            }
        }
        printf("begin for test read fd\n");
        for(i=0; i<FD_SETSIZE; i++){
            if((for_sockfd=*(connfd_array+i)) < 0){
                continue;
            }
            if(FD_ISSET(for_sockfd, &rset)) {
                
                str_echo_select(for_sockfd, &rset, connfd_array+i);
                
            } else {
                FD_SET(for_sockfd, &rset); // importent, if not old connect can not read data
            }
        }

    }
    return 0;
}

void str_echo_select(int sockfd, fd_set *rset, int *client_bit) {
    ssize_t n;
    char buf[MAXLINE];
    printf("start read\n");
    again:
    if((n = read(sockfd, buf, MAXLINE)) == 0) {
        close(sockfd);
        FD_CLR(sockfd, rset);
        printf("close fd:%d\n", *client_bit);
        *client_bit = -1;
    } else if (n < 0 && errno == EINTR){
        goto again;
    } else if (n < 0) {
        err_sys("read error");
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