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
char tempString[256];
char* textBuffer[1000];
char* keyBuffer[1000];
//char buffer2[256];

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
void load_text(char* filePath){
  // Clear out the buffer array
  memset(textBuffer, '\0', sizeof(textBuffer));
  
  // Open the specified file for reading only
  FILE* plainFile = fopen(filePath, "r");
  char* currLine = NULL;
  size_t len = 256;
  size_t nread;
  int i  = 0;
  // Process Till End of File
  while ((nread = getline(&currLine, &len, plainFile)) != -1){
    currLine[strcspn(currLine, "\n")] = '\0';
    textBuffer[i] = strdup(currLine);
    i++;
  }
  // indicates end of message
  //i++;
  textBuffer[i] = "@";

  
  // Testing Buffer Load
  /*
  for (int j=0; textBuffer[j]; j++)
  {
    printf("j = %d\n", j);
    printf("textBuffer = %s\n", textBuffer[j]);
  }
  */
  return;
}

/*
* This Function takes sends text buffer to server.
*/
void send_text(void){
  // Send message to server
  // Write to the server
  for (int j=0; textBuffer[j]; j++)
  {
    charsWritten = send(socketFD, textBuffer[j], strlen(textBuffer[j]), 0);
    if (charsWritten < 0)
    {
    error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(textBuffer[j]))
    {
      printf("CLIENT: WARNING: Not all data written to socket!\n");
    }
  }
  return;
}

/*
* This Function takes the key pass in and loads it to key buffer.
// Citation: CS344 Winter 2021 Program 1
*/
void load_key(char* filePath){
  // Clear out the buffer array
  memset(keyBuffer, '\0', sizeof(keyBuffer));
  
  // Open the specified file for reading only
  FILE* plainFile = fopen(filePath, "r");
  char* currLine = NULL;
  size_t len = 256;
  size_t nread;
  int i  = 0;
  // Process Till End of File
  while ((nread = getline(&currLine, &len, plainFile)) != -1){
    currLine[strcspn(currLine, "\n")] = '\0';
    keyBuffer[i] = strdup(currLine);
    i++;
  }
  // indicates end of message
  //i++;
  keyBuffer[i] = "#";

  
  // Testing Buffer Load
  /*
  for (int j=0; keyBuffer[j]; j++)
  {
    printf("j = %d\n", j);
    printf("keyBuffer = %s\n", keyBuffer[j]);
  }
  */
  return;
}

/*
* This Function takes sends key buffer to server.
*/
void send_key(void){
  // Send message to server
  // Write to the server
  for (int j=0; keyBuffer[j]; j++)
  {
    charsWritten = send(socketFD, keyBuffer[j], strlen(keyBuffer[j]), 0);
    if (charsWritten < 0)
    {
    error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(keyBuffer[j]))
    {
      printf("CLIENT: WARNING: Not all data written to socket!\n");
    }
  }
  return;
}

/*
* This Function send message to sever stating that it is waiting on encryption.
*/
void send_msg(void){
  // Send message to server
  // Write to the server
  char* msg = "CIPHER";

  charsWritten = send(socketFD, msg, strlen(msg), 0);
  if (charsWritten < 0)
  {
  error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(msg))
  {
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }
  return;
}

/*
* This Function gets the return message from server.
*/
void get_message(void)
{
  // Get return message from server
  // Clear out the buffer again for reuse
  memset(tempString, '\0', sizeof(tempString));
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, tempString, sizeof(tempString), 0); 
  if (charsRead < 0)
  {
  error("CLIENT: ERROR reading from socket");
  }
  printf("CLIENT: I received this from the server: \"%s\"\n", tempString);
  return;
}

// Function that gets plain text from client
void getCipher(void)
{
  // Read the client's message from the socket until termination signal is received
  // to index into text buffer
  int i = 0;
  // continue until broken
  while (1)
  {
    // clear out storage string
    memset(tempString, '\0', 256);
    // receive from client on socket
    // store in tempString
    charsRead = recv(socketFD, tempString, 255, 0);
    if (charsRead < 0)
    {
      error("ERROR1 reading from socket");
    }

    // load string to text buffer
    textBuffer[i] = strdup(tempString);
    i++;

    // to handle termination of client data
        
    // store length of tempString
    int tempLen = strlen(tempString);
    // find @ in tempString
    // if present test != tempLen
    int test = strcspn(tempString, "@");

    // break loop when termination found
    if (tempLen != test)
    {
      textBuffer[i] = '\0';
      break;
    }
  }
  // Testing Buffer Load
  /*
  for (int j=0; textBuffer[j]; j++)
  {
    printf("j = %d\n", j);
    printf("textBuffer = %s\n", textBuffer[j]);
  }
  */
  return;
}

// function prints the Cipher text to stdout
void printCipher(void)
{
  for (int m=0; textBuffer[m]; m++)
  {
    for (int n=0; textBuffer[m][n]; n++)
    {
      // stop when end character is found
      if (textBuffer[m][n] == '@'){
        break;
      }
      // print each character
      printf("%c", textBuffer[m][n]);
    }
  }
  // add new line at the end
  printf("\n");
  return;
}

#endif