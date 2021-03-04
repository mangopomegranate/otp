/*
* This Header file contains constants and helper functions for enc_client.c
* These constants and helper functions are related to data processing
*/

// make sure header files are not compiled twice
#ifndef eClient
#define eClient

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()


/*
* Constants used to handle enc_client
*/
int socketFD, portNumber, charsWritten, charsRead;
struct sockaddr_in serverAddress;
char* buffer[256];

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
}

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

/*
* This Function takes text in plainfile and loads it to buffer.
// Citation: CS344 Winter 2021 Program 1
*/
void load_buffer(char* filePath){
  // Clear out the buffer array
  memset(buffer, '\0', sizeof(buffer));
  
  // Open the specified file for reading only
  FILE* plainFile = fopen(filePath, "r");
  char* currLine = NULL;
  size_t len = 0;
  size_t nread;
  int i  = 0;
  // Process Till End of File
  while ((nread = getline(&currLine, &len, plainFile)) != -1){
    buffer[i] = strdup(currLine);
    i++;
  }

  /*
  // Testing Buffer Load
  for (int j=0; buffer[j]; j++)
  {
    printf("j = %d\n", j);
    printf("buffer = %s\n", buffer[j]);
  }
  */
}

/*
* This Function takes sends buffer to server.
*/
void send_buffer(void){
  // Send message to server
  // Write to the server
  for (int j=0; buffer[j]; j++)
  {
    charsWritten = send(socketFD, buffer[j], strlen(buffer[j]), 0);
    if (charsWritten < 0)
    {
    error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(buffer[j]))
    {
      printf("CLIENT: WARNING: Not all data written to socket!\n");
    }
  }
}
#endif