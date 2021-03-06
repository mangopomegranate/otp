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
    error("CLIENT: ERROR opening socket", 2);
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting", 2);
  }

  // Process Plain text and key 
  printf("CLIENT::SERVER\n");
  // load file in buffer
  load_text(argv[1]);
  // load key in buffer
  load_key(argv[2]);
  checkLen();
  send_msg();
  get_message();
  anyError();
  // send text to server
  send_text();
  // get return message from server
  get_message();
  // send key to server
  send_key();
  // get return message from server
  get_message();
  // send message to server
  // let server know that client is ready for cipher
  send_msg();
  // get cipher server
  getCipher();
  // send something else to end cypher loading
  // helps with large files
  send_msg();
  // print encoded message to stdout
  printCipher();
  
  // Close the socket
  close(socketFD); 
  return 0;
}