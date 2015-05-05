#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAXLINE	100

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )


void err_sys(char *err_msg);

void err_sys(char *err_msg)
{
    printf("%s\n", err_msg);
    exit(1);
}
/**
 * 1 size_t 在32位系统上 定义为 unsigned int 也就是说在32位系统上是32位无符号整形.
 * size_t一般用来表示一种计数，比如有多少东西被拷贝等.
 * 2 ssize_t是signed size_t
 * 
 */
ssize_t readn(int fd, void *pstr, size_t n)
{
	printf("start readn\n");
	char *str;
	str = pstr;
	size_t left_n = n;
	ssize_t a = 0;
	
	while(left_n > 0)
	{
		if((a = read(fd, str, left_n)) < 0)
		{	
			if(a == EINTR)
				continue;
			else
				return (-1);
		} else if(a == 0){
			break;
		}
		printf("read a:%d\n", a);
		
		if(a < left_n)
		{
			left_n = n - a;
			str = str + a;
		}
		
	}
	return (n - left_n);
}