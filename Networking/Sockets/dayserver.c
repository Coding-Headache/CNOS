/******************************************************************************
 * File Name: dayserver.c
 * Program Author: Daniel Hyde
 * Student ID: ********
 * Project Name: worksheets
 * Course Code: UFCFQ4-30-2
 * Date Created: 17 October 2016
 *
 * Description: A simple daytime server, replacing the normal one on port 13.
 *              Use telnet 127.0.0.1 portNo to connect. Try telnet localhost
 *              or to a neighbour's host and port number. On a slow machine
 *              try netstat again to see leftover connections! When
 *              experimenting complete kill -9 pid to remove the server. The
 *              port may still be in use if restart too quickly - bind() error!
 *****************************************************************************/
/* Include section
 * Add all #includes here                                                    */
 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/socket.h>        // socket(), bind(), listen(), accept()
 #include <string.h>            // bzero() - should now use memset()
 #include <netinet/in.h>        // struct sockaddr_in, htons(), htonl()
 #include <unistd.h>            // write(), close()
 #include <time.h>
/*****************************************************************************/
/* Defines section
 * Add all #defines here                                                     */
  #define MAXLINE 4096
  #define MAXQ 10
/*****************************************************************************/
/* Function prototype section
 * Add all function prototypes here                                          */

/*****************************************************************************/

int main(int argc, char const *argv[]) {

  int listenfd;
  int connfd;
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;
  socklen_t lenstruct;
  char buff[ MAXLINE];
  time_t ticks;

  // Create socket for listening on and set up internet address strucure
  listenfd = socket( AF_INET, SOCK_STREAM, 0);
  if( listenfd < 0) {
    perror( "socket error");
    return 1;
  }

  bzero( &servaddr, sizeof( servaddr));
  servaddr.sin_family = AF_INET;     // Should now use PF_INET!
  servaddr.sin_port = htons( 60000); // daytime 'well known' port in use!
  // Allow connection on any of my IP addresses - if I have more than one!
  servaddr.sin_addr.s_addr = htonl( INADDR_ANY);

  lenstruct = sizeof( servaddr);
  // Bind the inet address structure to the listening socket ready for use
  if ( (bind(listenfd, (const struct sockaddr *) &servaddr, lenstruct)) < 0) {
    perror( "bind error");
    return 1;
  }
  /* actually put the socket in listen mode and set the q length */
  if( listen( listenfd, MAXQ) < 0) {
    perror( "listen error");
    return 1;
  }

  for ( ; ; ) {  // Loop forever
    /* wait (block) for a client request on listen socket, when a request
     * appears connect it to the new socket connfd ready for the actuall
     * communication with the client. A successful connection will also get
     * client IP details.
     */
    connfd = accept( listenfd, (struct sockaddr *) &cliaddr, &lenstruct);
    if( connfd < 0) {
      perror ("accept error");
      return 1;
    }

    // Get the date and time, and write it to the connected socket
    ticks = time( NULL);
    snprintf(buff, sizeof( buff), "%.24s\r\n", ctime( &ticks));
    if(write( connfd, buff, strlen( buff)) < 0) {
      fprintf(stderr, "write error\n");
      return 1;
    }

    // Close the connected socket
    close( connfd);
  } // Forever

  /* Should never get here, will need to kill the process! So the listen socket
   * will remain open for a while - until garbage collection clears up!
   */
  close( listenfd); // Close listening socket
} // main
