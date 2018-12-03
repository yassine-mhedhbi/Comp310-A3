#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
int child_fn(void *arg)
{
   printf("\nINFO: This is child process.\n");

   return 0;
}

int main(int argc, char *argv[])
{
   printf("Start\n");

   void *stack = malloc(1024 * 1024);
   if (stack == NULL ) {
      printf("ERROR: Unable to allocate memory.\n");
      return -1 ;
   }

   int pid = clone(child_fn, stack + (1024 * 1024),  CLONE_NEWPID, NULL);
   if ( pid < 0 ) {
        printf("ERROR: Unable to create the child process.\n");
        return -1;
   }


   free(stack);

   //printf("INFO: Child process terminated.\n");
}
