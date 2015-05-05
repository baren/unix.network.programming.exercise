# include "tcpserverutil.h"

int main(int argc, char **argv)
{
	int sockfd=0;
    char buff[MAXLINE];
    char buff_read[MAXLINE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(struct sockaddr_in));
    int ptonz_ret = 0;
    ptonz_ret = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if(ptonz_ret!=1)
    {
    	printf("pton error\n");
    	exit(1);
    }
    
    addr.sin_port = htons(9090);
    addr.sin_family = AF_INET; 
    int con_ret = 0;
    if ((con_ret = connect(sockfd, (struct sockaddr_in *) &addr, sizeof(addr))) < 0){
    	err_sys("connect error");
    }
    int readn = 0;
    while(1){
        /**
         * stdin类型为 FILE* 
         * STDIN_FILENO类型为 int 
         */
    	if((readn = read(STDIN_FILENO, buff, MAXLINE)) > 0){
    		write(sockfd, buff, readn);
    		if((readn = read(sockfd, buff_read, MAXLINE)) > 0){
                buff[readn] = NULL;
    			printf("%s\n", buff);
    		}
    	}
    }
    return 0;
}

//void str_cli(FILE *fp, char *)