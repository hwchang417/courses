#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#define memsize 2000000
int main(int argc,char **argv)
{
	char output[memsize]={0};
	unsigned long int toutput[memsize/4]={0},total = 0,paddrt = 0, pmaddrt = 0;
	int i=0,m = 0,j = 0;
	FILE *f = fopen("Matrix1.txt","w");
	FILE *f2 = fopen(argv[2],"w");
	//FILE *f2 = fopen("matrix3.txt","w");
	int a = atoi(argv[1]);
	int temp;
	printf("%d\n",a);
	int b = syscall(353,a,output);
	for(i = 0; i < memsize; i++){
		fprintf(f,"a[%d] = %x  |  ",i,output[i]);
		if( (i+1) % 4 == 0)
		  fprintf(f,"\n");
	}
	
	for(i = 0,j = 0; i < memsize; i++)
	{
	    temp = temp | (unsigned char)output[i];
	    if((i + 1) % 4 == 0)
	    {
	       toutput[j++] = temp;
	       
	       if(temp == 0xffffffff)
	       	  break; 
	       temp = 0;
	    }
	    else
	       temp = temp << 8;
	}
	
	
	total = toutput[j-4];
	paddrt = toutput[j-3]; 
	pmaddrt = toutput[j-2]; 

	printf("total = %ld paddrt = %ld pmaddrt = %ld \n",total,paddrt,pmaddrt);   
	printf("total = %ld paddrt = %ld pmaddrt = %ld paddr = %ld \n",total*4,paddrt*4,pmaddrt*4,paddrt*4+pmaddrt*4);   
	for(i = 0; i < memsize/4; i+=2){
		if(toutput[i + 1] != 0)
		   fprintf(f2,"vaddr = %lx-%lx  paddr = %lx-%lx",toutput[i],toutput[i]+4095,toutput[i + 1],toutput[i + 1]+4095);	
		else
		   fprintf(f2,"vaddr = %lx-%lx  paddr = %lx-%lx",toutput[i],toutput[i]+4095,toutput[i + 1],toutput[i + 1]);
		fprintf(f2,"\n");
	}
	
	fclose(f);
	//fclose(f2);
	//printf("%d \n",b);
	return 0;
}
