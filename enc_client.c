/*
* This Program sets up a client that sends data to a server.
* The data consists of:
  1. plaintext (text to be encrypted)
  2. key (encryption key)
  3. port (at which encryption server is listening)
*/
#include "eClient.h"

int main(int argc, char *argv[]) {
  
  // Check usage & args
  if (argc < 4) { 
    fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); 
    exit(0); 
  } 

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }

  // Process Plain text and key 
  printf("CLIENT::SERVER\n");
  // load file in buffer
  load_buffer(argv[1]);
  // send buffer to server
  send_buffer();
  // get return message from server
  get_message();

  // Close the socket
  close(socketFD); 
  return 0;
}