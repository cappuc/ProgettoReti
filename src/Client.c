/* #include	"unp.h" */
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

#define MAXLINE 99

int main(int argc, char **argv)
{
  int			sockfd, n;
  char			recvline[MAXLINE + 1];
  struct sockaddr_in	servaddr;

  if (argc != 2){
    /*	err_quit("usage: a.out <IPaddress>"); NON VA */
    printf("usage: a.out <IPaddress>\n");
    exit(-1);
  }

  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    /*	err_sys("socket error"); */
    printf("socket error\n");
    exit(-2);
  }

  /*	bzero(&servaddr, sizeof(servaddr)); */
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port   = htons(60000);	/* daytime server */
  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
    /*	err_quit("inet_pton error for %s", argv[1]); */
    printf("inet_pton error for %s\n", argv[1]);
    exit(-3);
  }

  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
    /*err_sys("connect error");*/
    printf("connect error\n");
    exit(-4);
  }

  while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
    recvline[n] = 0;	/* null terminate */
    if (fputs(recvline, stdout) == EOF){
      /* 	err_sys("fputs error"); */
      printf("fputs error\n");
      exit(-5);
    }
  }
  if (n < 0){
    /*	err_sys("read error");*/
    printf("read error\n");
    exit(-6);
  }

  exit(0);
}
