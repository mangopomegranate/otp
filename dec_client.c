/*
* Exact copy of enc_client.c and eClient.h
* Difference is that here encrypted text is sent with a key to decrypt message.
* Also this client sends a different identifier than enc_client
*/
#include "dClient.h"


int main(int argc, char *argv[]) {
  
  // Check usage & args
  if (argc < 4) { 
    fprintf(stderr,"USAGE: %s encyptedtext key port\n", argv[0]); 
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
  fprintf(stderr, "CLIENT::SERVER\n");
  // load file in buffer
  load_text(argv[1]);
  // check if characters in text are valid
  checkChar();
  // load key in buffer
  load_key(argv[2]);
  // check if key and text file have equal length
  checkLen();
  // send and receive identifier
  send_msg();
  get_message();
  // handle when server rejects client
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