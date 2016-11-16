/* #include     "unp.h" */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include	<time.h>

#define MAXLINE	100
#define LISTENQ  10

int main(int argc, char **argv)
{
  int			listenfd, connfd;
  struct sockaddr_in	servaddr;
  char			buff[MAXLINE];
  time_t			ticks;

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  /*	bzero(&servaddr, sizeof(servaddr)); */
  memset(&servaddr,0 , sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(60000);	/* daytime server */

  bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  listen(listenfd, LISTENQ);

  for ( ; ; ) {

    connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);

    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    printf("about to send time: %.24s\r\n", buff);
    write(connfd, buff, strlen(buff));

    close(connfd);
  }
}
