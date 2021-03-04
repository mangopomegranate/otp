/*
* This Header file contains constants and helper functions for enc_server.c
* These constants and helper functions are related to data processing
*/

// make sure header files are not compiled twice
#ifndef eServer
#define eServer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h> //waitpid

/*
* Constants used to handle enc_server
*/
int connectionSocket, charsRead;
char buffer[256];
struct sockaddr_in serverAddress, clientAddress;
socklen_t sizeOfClientInfo = sizeof(clientAddress);

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

void runChild(void){
  // Citation: part of code from program3 cs344 Winter2021
  int childStatus;
  // Fork a new process
  pid_t spawnPid = fork();
  switch(spawnPid){
    // Error while forking
    case -1:
      perror("fork()\n");
      exit(1);
      break;
    // Child process Success
    case 0:
    // indicate to the user that a connection has been established
    printf("SERVER: Connected to client running at host %d port %d\n", ntohs(clientAddress.sin_addr.s_addr), ntohs(clientAddress.sin_port));
    // Get the message from the client and encode it
    memset(buffer, '\0', 256);
    // Read the client's message from the socket
    charsRead = recv(connectionSocket, buffer, 255, 0); 
    if (charsRead < 0){
    error("ERROR reading from socket");
    }
    printf("SERVER: I received this from the client: \"%s\"\n", buffer);

    // Send a Success message back to the client
    charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0); 
    if (charsRead < 0){
      error("ERROR writing to socket");
    }
    default:
    // wait for child process
    waitpid(spawnPid, &childStatus, WNOHANG);
  }
  return;
}

#endif