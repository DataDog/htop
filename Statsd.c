/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE 1024

int Statsd_run(void *portno) {
  int sockfd; /* socket */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
      printf("ERROR opening socket\n");
      exit(1);
  }
  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
       (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
     sizeof(serveraddr)) < 0)  {
      printf("ERROR on binding\n");
      exit(1);
  }

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1) {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
     (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0) {
      printf("ERROR in recvfromt\n");
      exit(1);
    }
    /* 
     * gethostbyaddr: determine who sent the datagram
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
        sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL) {
      printf("ERROR in gethostbyaddr\n");
      exit(1);
    }
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL) {
      printf("ERROR in inet_ntoa\n");
      exit(1);
    }
    printf("server received datagram from %s (%s)\n", 
     hostp->h_name, hostaddrp);
    printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);

  }

   pthread_exit(NULL);

}

void Statsd_init(int portno) {
  pthread_t thread;
  int rc;
  rc = pthread_create(&thread, NULL, Statsd_run, (void *)portno);
  if (rc) {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
}

void Statsd_shutdown() {
  pthread_exit(NULL);
}


double Statsd_getMetric(const char* metricName) {
  return 15.0;
}
