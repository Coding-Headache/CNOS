/******************************************************************************
 * File Name: listener.c
 * Program Author: Daniel Hyde
 * Student ID: ********
 * Project Name: worksheets
 * Course Code: UFCFQ4-30-2
 * Date Created: 24 October 2016
 *
 * Description: A datagram socket 'server' that simply displays a message
 *              that has been recieved and then dies.
 *****************************************************************************/
/* Include section
 * Add all #includes here                                                    */
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <errno.h>
  #include <string.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
/*****************************************************************************/
/* Defines section
 * Add all #defines here                                                     */
  #define MYPORT 4950             // The port users connect to
  #define MAXBUFLEN 100
/*****************************************************************************/
/* Function prototype section
 * Add all function prototypes here                                          */

/*****************************************************************************/

int main(void) {

  int sockfd;
  struct sockaddr_in my_addr;     // Info for my_addri.e. server
  struct sockaddr_in their_addr;  // Clients address info
  int addr_len, numbytes;
  char buf[MAXBUFLEN];

  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("Listener socket");
    exit(1);
  }

  memset(&my_addr, 0, sizeof(my_addr));  // Zero struct
  my_addr.sin_family = AF_INET;          // Host byte order ...
  my_addr.sin_port = htons(MYPORT);      // ... short, network byte order
  my_addr.sin_addr.s_addr = INADDR_ANY;  // any of server IP addrs

  if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
    perror("Listener bind");
    exit(1);
  }

  addr_len = sizeof(struct sockaddr);
  if((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
       (struct sockaddr *)&their_addr, &addr_len)) == -1) {
    perror("Listener recvfrom");
    exit(1);
  }

  printf("Got packet from %s\n", inet_ntoa(their_addr.sin_addr));
  printf("Packet is %d bytes long\n", numbytes);
  buf[numbytes] = '\0';           // End of string
  printf("Packet contains \"%s\"\n", buf);

  close(sockfd);
  return 0;
}
