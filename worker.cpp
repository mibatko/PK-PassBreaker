#include "pvm3.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <unistd.h>

#define MSG_GET_WK 3
#define MSG_GET_PSL 4
#define MSG_GET_PSS 5
#define MSG_SEND 7

#define ASCII_EX 33
#define ASCII_TILDE 126

char* hash;
char* guess;
bool stop = false;
int mytid, numberOfWorkers, master;

void bruteForce() {

}

int main(int argc, const char* argv[])
{
   int passLength;
   hash = (char *)malloc(13);
   strcpy(hash,"");

   mytid = pvm_mytid();
   master = pvm_parent();

   pvm_recv(-1,MSG_GET_WK);
   pvm_upkint(&numberOfWorkers, 1, 1);
   pvm_recv(-1,MSG_GET_PSL);
   pvm_upkint(&passLength, 1, 1);
   pvm_recv(-1,MSG_GET_PSS);
   pvm_upkstr(hash);

   guess = (char *)malloc(passLength);

   switch(passLength) {
      case 1: strcpy(guess,"0"); break;
      case 2: strcpy(guess,"00"); break;
      case 3: strcpy(guess,"000"); break;
      case 4: strcpy(guess,"0000"); break;
      case 5: strcpy(guess,"00000"); break;
      case 6: strcpy(guess,"000000"); break;
      case 7: strcpy(guess,"0000000"); break;
      default: strcpy(guess,"00000000");
   }

   for(guess[0] = ASCII_EX + mytid % numberOfWorkers; guess[0] <= ASCII_TILDE && !stop; guess[0]+=numberOfWorkers) {
      for(guess[1] = ASCII_EX; guess[1] <= ASCII_TILDE && !stop; guess[1]++) {
         for(guess[2] = ASCII_EX; guess[2] <= ASCII_TILDE && !stop; guess[2]++) {
            for(guess[3] = ASCII_EX; guess[3] <= ASCII_TILDE && !stop; guess[3]++) {
               if(strcmp(hash, crypt(guess, "GG")) == 0) {
                  stop = true;
                  pvm_initsend(PvmDataRaw);
                  pvm_pkstr(guess);
                  pvm_send(master, MSG_SEND);
               }
            }
         }
      }
   }

   pvm_initsend(PvmDataRaw);
   pvm_pkstr((char*)"Fail");
   pvm_send(master, MSG_SEND);

   pvm_exit();
}