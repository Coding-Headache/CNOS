/******************************************************************************
 * File Name: web8000.c
 * Program Author: Daniel Hyde
 * Student ID: ********
 * Project Name: worksheets
 * Course Code: UFCFQ4-30-2
 * Date Created: 17 November 2016
 *
 * Description: A simple web server.
 *****************************************************************************/
/* Include section
 * Add all #includes here                                                    */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
/*****************************************************************************/
/* Defines section
 * Add all #defines here                                                     */
#define WEBROOT "/home/netlab/heinz/html"    // Base dir for html/txt files
#define HTTP_PORT 8000
/*****************************************************************************/
/* Function prototype section
 * Add all function prototypes here                                          */
int create_tcp_endpoint(int port);
/*****************************************************************************/

int main(void) {

  int fdnet, fd, count, sock;
  char filename[100], request[1024], reply[1024];

  // Create an endpoint to listen on
  if((sock = create_tcp_endpoint(HTTP_PORT)) < 0) {
     fprintf(stderr, "Cannot create endpoint\n");
     exit(1);
  }

  // Enter the main service loop
  while(1) {
    fdnet = accept(sock, NULL, NULL);       // Get a connection from a client
    fprintf(stderr, "connected on fd %d\n", fdnet);

    // Read the GET request & build the file name
    // i.e. something like request[] = "GET /file.html HTTP/1.1"
    read(fdnet, request, sizeof(request));

    strtok(request, " ");
    strcpy(filename, WEBROOT);
    strcat(filename, strtok(NULL, " "));
    printf("Requested file is %s\n", filename);
    fd = open(filename, O_RDONLY);

    if(fd <0) {                           // check to see if the file is there
      text(fdnet, "HTTP/1.1 404 Not Found\n");
      text(fdnet, "Content-Type: text/html\n\n");
      text(fdnet, "<HTML><BODY> Not Found </BODY></HTML>");
    }
    else {
      text(fdnet, "HTTP/1.1 200 OK\n");
      text(fdnet, "Content-Type: text/html\n\n");
      while((count = read(fd, reply, 1024)) > 0)
        write(fdnet, reply, count);
    }
    close(fd);
    close(fdnet);
    }
  }

/******************************************************************************
 * Function Name      :  create_tcp_endpoint()
 *    returns         :  int
 *    arg1            :  int port
 *    arg2            :  nil
 * Created by         :  Daniel Hyde
 * Date created       :  17 November 2016
 * Description        :  A function to create a server endpoint
 * Notes              :  nil
 *****************************************************************************/

int create_tcp_endpoint(int port) {

  int sock;
  struct sockaddr_in server;

  // Make socket with TCP streams. Kernel choose a suitable protocol
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) return -1;                         // Failed to make socket

  memset(&server, 0, sizeof(server));             // Zero the struct
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);     // Any server's IP addr
  server.sin_port = htons(port);

  if(bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
    return -2;                                     // Failed to bind

  listen(sock, 5);         // A listening socket with a max queue of 5 clients
  return sock;
}
