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
char* buffer[1000];
char buffer2[256];

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
  size_t len = 256;
  size_t nread;
  int i  = 0;
  // Process Till End of File
  while ((nread = getline(&currLine, &len, plainFile)) != -1){
    currLine[strcspn(currLine, "\n")] = '\0';
    buffer[i] = strdup(currLine);
    i++;
  }
  // indicates end of message
  //i++;
  buffer[i] = "@";

  
  // Testing Buffer Load
  /*
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

/*
* This Function gets the return message from server.
*/
void get_message(void)
{
  // Get return message from server
  // Clear out the buffer again for reuse
  //memset(buffer, '\0', sizeof(buffer));
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, buffer2, sizeof(buffer2) - 1, 0); 
  if (charsRead < 0)
  {
  error("CLIENT: ERROR reading from socket");
  }
  printf("CLIENT: I received this from the server: \"%s\"\n", buffer2);
}
#endif