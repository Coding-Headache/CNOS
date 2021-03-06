/******************************************************************************
 * File Name: dayclient.c
 * Program Author: Daniel Hyde
 * Student ID: ********
 * Project Name: worksheets
 * Course Code: UFCFQ4-30-2
 * Date Created: 11 October 2016
 *
 * Description: A simple daytime client, i.e. connects to port 13. Requires IP
 *              address to be given on command line, e.g. dayclient 127.0.0.1.
 *****************************************************************************/
/* Include section
 * Add all #includes here                                                    */
 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/socket.h>        // socket(), connect()
 #include <string.h>            // bzero() - should now use memset()
 #include <netinet/in.h>        // struct sockaddr_in, htons()
 #include <arpa/inet.h>         // inet_aton() - should now use inet_pton()!
 #include <unistd.h>            // read()
/*****************************************************************************/
/* Defines section
 * Add all #defines here                                                     */
  #define MAXLINE 4096
/*****************************************************************************/
/* Function prototype section
 * Add all function prototypes here                                          */

/*****************************************************************************/

int main(int argc, char const *argv[]) {
  int sockfd;                  // the socket file descriptor
  int nbytes;                  // number of bytes actually read from the socket
  char recvline[ MAXLINE + 1]; // to receive the day and time string
  struct sockaddr_in servaddr; // socket internet address structure

  if (argc!=2) {
    fprintf(stderr, "usage: %s <IPaddress>\n", argv[0]);
    return 1;
  }

  /* Create an internet (AF_INET) stream (SOCK_STREAM) socket
   * with default protocol, i.e. a TCP socket.
   * Try changing the default protocol from 0 to 1 to force error!
   */
  if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket error");
    return 1;
  }

  /* Zero the entire socket address structure to ensure we set up
   * only what we want! Then specify internet address family to AF_INET.
   * Set the port to that of the daytime server (i.e. 13) converting it from
   * host integer format to network format.
   * Finally convert the ascii quad format of the IP addr to network format.
   */
   bzero( &servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;  // should now use PF_INET!
   servaddr.sin_port = htons(13);  // daytime server 'well known' port
   if( inet_aton(argv[1], &servaddr.sin_addr) <=0){
     /* should now use - inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
      * even older versions use servaddr.sin_addr.s_addr = inet_addr(argv[1]);
      */
     fprintf(stderr, "inet_aton error for %s\n", argv[1]);
     return 1;
   }

   if( connect( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) <0){
     perror("connect error");
     return 1;
   }

   // Read data from the network. No bytes read implies end or error!
   while ((nbytes=read(sockfd, recvline, MAXLINE)) >0) {
     recvline[nbytes] = '\0';     // end-of-string char *
     if (fputs(recvline, stdout) == EOF) {
       perror("fputs error");
       return 1;
     }
   }

   if(nbytes < 0){
     perror("read error");
     return 1;
   }
   exit(0);
}
