/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include "XAlloc.h"
#include "Statsd.h"
#include "Hashtable.h"

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

struct Hashtable_ *metrics;
typedef struct Metric {
  double currentVal;
  double maxVal;
} Metric;

unsigned long hash(unsigned char *str) {
  unsigned long val = 5381;
  int c;

  while ((c = *str++))
    val = ((val << 5) + val) + c; /* val * 33 + c */

  return val;
}


void *Statsd_run(void *portno) {
  int sockfd; /* socket */
  unsigned int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

  char *tokstate;
  char *metricName;
  char *valStr;
  double val;
  char *metricType;
  int hashKey;
  void *hashVal;
  Metric* metric;

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

    metricName = strtok_r(buf, ":", &tokstate);
    if (metricName == NULL) {
      printf("ERROR no metric found\n");
      continue;
    }
    valStr = strtok_r(NULL, "|", &tokstate);
    if (valStr == NULL) {
      printf("ERROR no value found\n");
      continue;
    }
    val = strtod(valStr, NULL);
    metricType = strtok_r(NULL, "|", &tokstate);
    if (metricType == NULL) {
      printf("ERROR no metric type found\n");
      continue;
    }
    printf("Got a metric: %s, val: %s, type: %s\n", metricName, valStr, metricType);

    hashKey = hash((unsigned char *)metricName);
    hashVal = Hashtable_get(metrics, hashKey);
    if (hashVal == NULL) {
      metric = xMalloc(sizeof(Metric));
      metric->maxVal = 0.0;
    } else {
      metric = (Metric *) hashVal;
    }
    metric->currentVal = val;
    if (val > metric->maxVal) {
      metric->maxVal = val;
    }

    Hashtable_put(metrics, hashKey, (void *)metric);
  }

   pthread_exit(NULL);
}

void Statsd_init(int portno) {
  metrics = Hashtable_new(256, false);

  // Statsd_run((void *)portno);
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
  void *hashVal = (double *)Hashtable_get(metrics, hash((unsigned char *)metricName));
  Metric *m;
  if (hashVal == NULL) {
    return 0.0;
  } else {
    m = (Metric *) hashVal;
    return m->currentVal;
  }
}
