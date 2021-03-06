/******************************************************************************
 * File Name: serverFork.c
 * Program Author: Daniel Hyde
 * Student ID: ********
 * Project Name: worksheets
 * Course Code: UFCFQ4-30-2
 * Date Created: 18 October 2016
 *
 * Description: Send a message to any connected client.
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
 #include <sys/wait.h>
 #include <signal.h>
/*****************************************************************************/
/* Defines section
 * Add all #defines here                                                     */
 #define MYPORT 3490        // the port users connect to
 #define BACKLOG 10         // max no. of pending connections in server queue
 #define MAXDATASIZE 200
/*****************************************************************************/
/* Function prototype section
 * Add all function prototypes here                                          */
 void sigchld_handler( int s) {
   while( wait( NULL) > 0);  // wait for any child to finish
 }
/*****************************************************************************/


int main( void) {
  int listenfd;            // listening socket
  int connfd;              // connection socket
  struct sockaddr_in server_addr;  // info for my addr i.e. server
  struct sockaddr_in client_addr;  // client's address info
  int sin_size;            // size of address structure

  //struct sigaction sa;     // deal with signals from dying children!
  int yes = 1;
  char clientAddr[ 20];    // holds ascii dotted quad address

  if( (listenfd = socket( AF_INET, SOCK_STREAM, 0)) == -1) {
     perror( "Server socket");
     exit( 1);
  }

  // Set Unix socket level to allow address reuse
  if( setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR,
                                &yes, sizeof( int)) == -1) {
     perror( "Server setsockopt");
     exit( 1);
  }

sin_size = sizeof( server_addr);
memset( &server_addr, 0, sin_size);        // zero struct
server_addr.sin_family = AF_INET;          // host byte order
server_addr.sin_port = htons( MYPORT);     // short, network byte order
server_addr.sin_addr.s_addr = INADDR_ANY;  // any server IP addr

if( bind( listenfd, (struct sockaddr *)&server_addr,
                             sizeof( struct sockaddr)) == -1) {
    perror( "Server bind");
    exit( 1);
}
if( listen( listenfd, BACKLOG) == -1) {
    perror( "Server listen");
    exit( 1);
}

// // Signal handler stuff
// sa.sa_handler = sigchld_handler;  // reap all dead processes
// sigemptyset( &sa.sa_mask);
// sa.sa_flags = SA_RESTART;
// if( sigaction( SIGCHLD, &sa, NULL) == -1) {
//     perror( "Server sigaction");
//     exit( 1);
// }

while( 1) {                       // main accept() loop
  sin_size = sizeof( struct sockaddr_in);
  if( (connfd = accept( listenfd,
                (struct sockaddr *)&client_addr, &sin_size)) == -1) {
      perror( "Server accept");
      continue;
  }
  strcpy( clientAddr, inet_ntoa( client_addr.sin_addr));
  printf( "Server: got connection from %s\n", clientAddr);

  if( !fork()) {                // the child process dealing with a client
    char msg[ MAXDATASIZE];
    int numbytes;

  close( listenfd);             // child does not need the listener
  msg[ 0] = '\0';               // no message yet!
  do {
  if( (numbytes =recv( connfd, msg, MAXDATASIZE -1, 0)) == -1) {
    perror( "Server recv");
    exit( 1);                   // error end of child
  }
  msg[ numbytes] = '\0';        // end of string
  fprintf( stderr, "Message received: %s\n", msg);

  if( send( connfd, "Hello, World!\n", 14, 0) == -1) {
    perror( "Server send");
    exit( 1);                   // error end of child
  }

} while( strcmp( msg, "quit") != 0);
close( connfd);
exit( 0);                       // end of child!
} // fork()
close( connfd);                 // parent does not need the connection socket
} // while( 1)
return 0;

}
