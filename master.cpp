#include "pvm3.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <time.h>

#define ASCII_0 48
#define ASCII_z 122

#define MSG_SEND_WK 3
#define MSG_SEND_PSL 4
#define MSG_SEND_PSS 5
#define MSG_GET 7

int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

int main(int argc, char* argv[])
{
   if(argc < 3) {
      std::cout << "Please specify command line arguments in following order:" << std::endl;
      std::cout << " - number of workers [int]" << std::endl;
      std::cout << " - password you want hashed and cracked [string]" << std::endl;
      std::cout << "Example: ./master 5 Password1\n";
      return 0;
   }
   
   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC, &start);
   
   int numberOfWorkers = atoi(argv[1]);
   int mytid, task_ids[numberOfWorkers];

   char* password = argv[2];
   std::cout << "Password to be guessed: " << password << std::endl;
   int passLength = strlen(password);
   passLength = passLength > 8 ? 8 : passLength;
   char* salt = (char *)"GG";

   char* encryptedPass = (char *)malloc(13);
   strcpy(encryptedPass,crypt(password, salt));
   char* guess = (char *)malloc(passLength+1);
   strcpy(guess,"");
   char* answer = (char *)malloc(passLength+1);
   strcpy(answer,"");

   std::cout << "Encrypted pass: " << encryptedPass << std::endl;

   mytid = pvm_mytid();

   pvm_spawn((char *)"worker", (char **)0, PvmTaskDefault, (char *)"", numberOfWorkers, task_ids);

   for (int i = 0; i < numberOfWorkers; i++) {
      pvm_initsend(PvmDataRaw);
      pvm_pkint(&numberOfWorkers, 1, 1);
      pvm_send(task_ids[i], MSG_SEND_WK);

      pvm_initsend(PvmDataRaw);
      pvm_pkint(&passLength, 1, 1);
      pvm_send(task_ids[i], MSG_SEND_PSL);
      
      pvm_initsend(PvmDataRaw);
      pvm_pkstr(encryptedPass);
      pvm_send(task_ids[i], MSG_SEND_PSS);
   }

   // do {
   pvm_recv(-1, MSG_GET);
   pvm_upkstr(answer);

   std::cout << "Recieved pass: " << answer << std::endl;
   // } while (1);

   for (int i = 0; i < numberOfWorkers; i++) {
      pvm_kill(task_ids[i]);
   }

   pvm_exit();

   clock_gettime(CLOCK_MONOTONIC, &end);
   uint64_t timeElapsed = timespecDiff(&end, &start);
   double time= timeElapsed / 1000000000.0;
   std::cout << "Time elapsed: " << time << " ms" << std::endl;
}