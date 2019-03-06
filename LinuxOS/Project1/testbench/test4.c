#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#define memsize 2000000
int main(int argc,char **argv)
{
	//char output[memsize]={0};
	unsigned long int tvaddr1[memsize/4]={0},tvaddr2[memsize/4]={0},tpaddr1[memsize/4]={0},tpaddr2[memsize/4]={0},vaddr1,vaddr2,vaddr3,vaddr4,paddr1,paddr2,paddr3,paddr4;
	int i=0,m = 0,j = 0;
	unsigned int flag = 0;
	FILE *f = fopen(argv[1],"r");
	FILE *f2 = fopen(argv[2],"r");
	FILE *f3 = fopen("same_v.txt","w");
	FILE *f4 = fopen("same_p.txt","w");
	
	if(f != NULL && f2 != NULL && f3 != NULL)
	while(m != -1)
	{
	      m = fscanf(f,"vaddr = %lx-%lx  paddr = %lx-%lx\n",&vaddr1,&vaddr2,&paddr1,&paddr2);
	      //j = fscanf(f2,"vaddr = %lx-%lx  paddr = %lx-%lx\n",&vaddr3,&vaddr4,&paddr3,&paddr4);
	      //printf("m = %d j = %d \n",m,j);
	     // printf("va = %lx pa = %lx \n",vaddr1,paddr3);
	     // if(vaddr1 == vaddr3 && vaddr1 != 0)
	     // 	fprintf(f3,"vaddr : %lx \n",vaddr1);
	     // if(paddr1 == paddr3 && paddr1 != 0)
	     // 	fprintf(f3,"paddr : %lx \n",paddr1);
	      if(paddr1 == 0xffffffff)
	      	break;
	      tvaddr1[i] = vaddr1; tvaddr2[i] = vaddr2; tpaddr1[i] = paddr1; tpaddr2[i++] = paddr2;
	}
	printf("i = %d\n",i);
	m = 0;
	
	while(m != -1)
	{
	      m = fscanf(f2,"vaddr = %lx-%lx  paddr = %lx-%lx\n",&vaddr1,&vaddr2,&paddr1,&paddr2);
	      if(paddr2 == 0xffffffff)
	      	break;
	      for(j = 0; j < i-1; j++)
	      {
	      	  if(vaddr1 == tvaddr1[j] && vaddr1 != 0 && !(flag & 0x0001)){
	             fprintf(f3,"vaddr1 : %lx \n",vaddr1);flag = flag | 0x0001;
	             tvaddr1[j] = 0;
	          }
	          if(paddr1 == tpaddr1[j] && paddr1 != 0 && !(flag & 0x0010)){
	       	     fprintf(f4,"paddr1 : %lx \n",paddr1);flag = flag | 0x0010;
	       	     tpaddr1[j] = 0;
	       	  }
	       	  if(vaddr2 == tvaddr2[j] && vaddr2 != 0 && !(flag & 0x0100)){
	             fprintf(f3,"vaddr2 : %lx \n",vaddr2);flag = flag | 0x0100;
	             tvaddr2[j] = 0;
	          }
	          if(paddr2 == tpaddr2[j] && paddr2 != 0 && !(flag & 0x1000)){
	       	     fprintf(f4,"paddr2 : %lx \n",paddr2);flag = flag | 0x1000;
	       	     tpaddr2[j] = 0;
	       	  }
	      }	
	      flag = 0;
	}
	fclose(f);
	fclose(f2);
	fclose(f3);
	return 0;
}
