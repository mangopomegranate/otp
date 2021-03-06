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
char tempString[256];
char tempKey[256];
char* textBuffer[1000];
char* keyBuffer[1000];
char mod[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
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

// Function to send back data to client
void sendMessage(char* msg){
  char* message = msg;
  int msgLen = strlen(message);
  charsRead = send(connectionSocket, message, msgLen, 0); 
  if (charsRead < 0)
  {
  error("ERROR writing to socket");
  }
  charsRead = 0;
  return;
}

/*
* This Function takes sends Cipher text to client
*/
void sendCipher(void){
  // Send message to client
  // Write to the server
  for (int j=0; textBuffer[j]; j++)
  {
    charsRead = send(connectionSocket, textBuffer[j], strlen(textBuffer[j]), 0);
    if (charsRead < 0)
    {
    error("Server: ERROR writing to socket");
    }
    if (charsRead < strlen(textBuffer[j]))
    {
      printf("Server: WARNING: Not all data written to socket!\n");
    }
  }
  return;
}
// Function that gets plain text from client
void getText(void)
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
    charsRead = recv(connectionSocket, tempString, 255, 0);
    if (charsRead < 0)
    {
      error("ERROR reading from socket");
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
  // Send a Success message back to the client
  sendMessage("Text Received");
  return;
}

// Function that gets key from client
void getKey(void)
{
  // Read the client's message from the socket until termination signal is received
  // to index into text buffer
  int k = 0;
  // continue until broken
  while (1)
  {
    // clear out storage string
    memset(tempKey, '\0', 256);
    // receive from client on socket
    // store in tempKey
    charsRead = recv(connectionSocket, tempKey, 255, 0);
    if (charsRead < 0)
    {
      error("ERROR reading from socket");
    }

    // load key buffer
    keyBuffer[k] = strdup(tempKey);
    k++;

    // to handle termination of client data
        
    // store length of tempString
    int tempLen = strlen(tempKey);
    // find @ in tempString
    // if present test != tempLen
    int test = strcspn(tempKey, "#");

    // break loop when termination found
    if (tempLen != test)
    {
      keyBuffer[k] = '\0';
      break;
    }
  }
  // Send a Success message back to the client
  sendMessage("Key Received");
  return;
}

// function that gets the integer for each allowed character
int getMod(char letter){
  // iterate through mod to get index of character
  int index;
  for (int l=0; l < sizeof(mod); l++)
  {
    // integer is the index of character in mod + 1 
    if (letter == mod[l])
    {
      index = l;
    }
  }
  return index + 1;
}

// function to encrypt text using key
void encrypt(){
  // Take a letter from textBuffer
  for (int m=0; textBuffer[m]; m++)
  {
    for (int n=0; textBuffer[m][n]; n++)
    {
      // stop when end character is found
      if (textBuffer[m][n] == '@'){
        // padd cypher message with @ for gazing end of message
        textBuffer[m][n] = '@';
        break;
      }
      // get integer value of text
      int textMod = getMod(textBuffer[m][n]);
      // get integer value of key
      int keyMod = getMod(textBuffer[m][n]);
      
      // sum of text and Key
      int sumMod = textMod + keyMod;
      
      // when number larger than 27
      if ( sumMod > 27)
      {
        // substract 27
        sumMod = sumMod - 27;
      }
      
      // get cipher character from mod and replace it in text buffer
      char cipher = mod[sumMod - 1];
      textBuffer[m][n] = cipher;
    }
  }
  return;
}

// Function handles child processes of enc_server
void runChild(void){
  // Citation: part of code from program3 cs344 Winter2021
  int childStatus;
  // Fork a new process
  pid_t spawnPid = fork();
  switch(spawnPid)
  {
    // Error while forking
    case -1:
      perror("fork()\n");
      exit(1);
      break;
    // Child process Success
    case 0:
      // indicate to the user that a connection has been established
      printf("SERVER: Connected to client running at host %d port %d\n", ntohs(clientAddress.sin_addr.s_addr), ntohs(clientAddress.sin_port));
      // Get the plain text from the client
      getText();
      
      // Get key from client
      getKey();
      
      // replace text Buffer with encrypted message
      encrypt();

      sendCipher();

      printf("SERVER: I received this from the client to encrypt:\n");
      for (int j=0; textBuffer[j]; j++)
      {
        printf("%s\n", textBuffer[j]);
      }

      printf("SERVER: I received this from the client as key:\n");
      for (int j=0; keyBuffer[j]; j++)
      {
        printf("%s\n", keyBuffer[j]);
      }
    default:
      // wait for child process
      waitpid(spawnPid, &childStatus, WNOHANG);
  }
  return;
}

#endif