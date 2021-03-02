/*
* This Program (keygen.c) creates a key file.

* The length of the key must be specified when calling the program
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h> // for srand

int main(int argc, char *argv[]) {
  
  // key is made up of char in key_bank
  char key_bank[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

  // seed random to current time which will always change
  srand(time(0));
  // print a random character from key_bank to standard output
  for (int i = 0; i < atoi(argv[1]); i++){
    printf("%c", key_bank[rand()%27]);
    fflush(stdout);
  }
  // handout: the last character keygen outputs should be a new line
  printf("\n");
  fflush(stdout);
  return 0;
}