/*===========================================================================*/
/*     prototype of new system call one is as follows                        */
/*     void go_to_sleep_now()                                               */
/*===========================================================================*/
 #include <stdio.h>
#include <unistd.h>
 #include <stdlib.h>
 #include <time.h>
 #include <sys/time.h>

int main(int argc, char **argv)
{ 
   unsigned         i, j, flag=0;
   int  b;
   struct timeval   start, end;

/*==========================================*/
/*                 PART  I                  */
/*==========================================*/ 
   gettimeofday(&start, NULL);
   for(i=0;i<100000;i++)
   for(j=0;j<100000;j++)                    
   if(flag==0)
   { 
      b = syscall(352);    //new system call one
      flag=1;
   }
   gettimeofday(&end, NULL);
   printf("Return %d\n",b);
   printf("It takes the process %ld uses to complete the double loop, if the process has slept for a while.\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));


/*==========================================*/
/*                 PART  II                 */
/*==========================================*/ 
  gettimeofday(&start, NULL);
  for(i=0;i<100000;i++)
  for(j=0;j<100000;j++);                  
  
  gettimeofday(&end, NULL);
  printf("It takes the process %ld uses to complete the double loop, if the process does not sleep.\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
}
