#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>


#define BUFSIZE 520

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  echoserver [options]\n"                                                    \
"options:\n"                                                                  \
"  -p                  Port (Default: 20502)\n"                                \
"  -m                  Maximum pending connections (default: 1)\n"            \
"  -h                  Show this help message\n"                              \

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
  {"port",          required_argument,      NULL,           'p'},
  {"maxnpending",   required_argument,      NULL,           'm'},
  {"help",          no_argument,            NULL,           'h'},
  {NULL,            0,                      NULL,             0}
};


int main(int argc, char **argv) {
  int option_char;
  int portno = 20502; /* port to listen on */
  int maxnpending = 1;
  
  // Parse and set command line arguments
  while ((option_char = getopt_long(argc, argv, "p:m:hx", gLongOptions, NULL)) != -1) {
   switch (option_char) {
      case 'p': // listen-port
        portno = atoi(optarg);
        break;                                        
      default:
        fprintf(stderr, "%s ", USAGE);
        exit(1);
      case 'm': // server
        maxnpending = atoi(optarg);
        break; 
      case 'h': // help
        fprintf(stdout, "%s ", USAGE);
        exit(0);
        break;
    }
  }

    setbuf(stdout, NULL); // disable buffering

    if ((portno < 1025) || (portno > 65535)) {
        fprintf(stderr, "%s @ %d: invalid port number (%d)\n", __FILE__, __LINE__, portno);
        exit(1);
    }
    if (maxnpending < 1) {
        fprintf(stderr, "%s @ %d: invalid pending count (%d)\n", __FILE__, __LINE__, maxnpending);
        exit(1);
    }


  /* Socket Code Here */

  int sock_fd = socket(AF_INET , SOCK_STREAM, 0);
  if( sock_fd < 0 ){
        perror("Error occur when creating a socket");
        exit(1);
  }
  
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  int yes= 1;
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  int binding = bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if(binding < 0){
    perror("Error occured on binding");
    exit(1);
  }
  listen(sock_fd , 5);
  // int client_len = sizeof(cli_addr);
  // int newsockfd = accept(sock_fd, (struct sockaddr *) &cli_addr, &client_len);
  // int buffer[16];
  // bzero(buffer,sizeof(buffer));
  // int n = read(newsockfd,buffer,sizeof(buffer));
  // if (n < 0){
  //   perror("ERROR reading from socket");
  //   exit(1);
  // } 
  // n = write(newsockfd,buffer,16);
  // if (n < 0){
  //   perror("ERROR writing to socket");
  //   exit(1);
  // }
  // return 0;
  socklen_t client_len = sizeof(cli_addr);
  while(1){
  int newsockfd = accept(sock_fd, (struct sockaddr *) &cli_addr, &client_len);
  char buffer[16];
  bzero(buffer,sizeof(buffer));
  int f = fork();
  if(f < 0){
    perror("There has been an error in forking");
    exit(1);
  }

  if(f == 0){
    close(sock_fd);
    int n = read(newsockfd,buffer,sizeof(buffer));
  if (n < 0){
    perror("ERROR reading from socket");
    exit(1);
  } 
  printf("%s" , buffer);
  n = write(newsockfd,buffer,16);
  if (n < 0){
    perror("ERROR writing to socket");
    exit(1);
  }
  exit(0);
  }
  else{
    close(newsockfd);
  }
  }
}
