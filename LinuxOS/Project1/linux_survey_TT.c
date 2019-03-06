#include <linux/linkage.h>
#include <linux/kernel.h>
//#include <linux/module.h>
#include <linux/sched.h>

//#include <linux/mm.h>
#include <asm/pgtable.h>
#include <asm/pgtable_types.h>

#include <linux/highmem.h>
unsigned long int for_linux_pmd_count = 0, for_linux_pte_NULL = 0, write_count = 0;
unsigned char wflag = 0;

void write_addr(unsigned long int addr,char *buffer)
{

    int i = 0;
    if(write_count < 500000)
    {
    for(i = 0; i < 4; i++)
    {
    	buffer[write_count*4+i] = addr >> (3-i) * 8; 
    } 
    write_count++;
    }
}
static struct page *
my_follow_page(struct mm_struct *mm, unsigned long addr, char *buffer, unsigned long int *ppaddr)
{
pud_t *pud;
pmd_t *pmd;
pgd_t *pgd;
pte_t pte, *ptep;
long unsigned  int paddr = 0;
struct page *page = NULL;
phys_addr_t phy = 0;
pgd = pgd_offset(mm, addr);
if (pgd_present(*pgd)) {
	pud = pud_offset(pgd,addr);
	if (pud_present(*pud)) {
		pmd = pmd_offset(pud,addr);
		if(pmd_present(*pmd))  
		{  
			if(!pmd_large(*pmd))
			{  
	
				ptep = pte_offset_map(pmd,addr);
				if (pte_none(*ptep) || ptep == NULL) 
				{
					printk("pte_NULL\n");
					for_linux_pte_NULL++;
				}
				else
				{
					pte = *ptep;
					if(pte_present(*ptep))
					{
					   paddr = (pte_val(*ptep) & PAGE_MASK) | (addr & ~PAGE_MASK);
					   printk(KERN_EMERG"physical address of 0x%lx is 0x%lx \n",addr,paddr);
					   printk("__pa(0x%lx) = 0x%lx",addr,__pa(addr));
					   page = pte_page(pte);
					   phy = page_to_phys(page);
					   paddr = (long unsigned int)phy;
					   printk(KERN_EMERG "page frame is %lx",(unsigned long int)pte.pte);
					   //write_addr((long unsigned int)phy,buffer);
					   /*if(wflag == 0)
					   {
					      write_addr((long unsigned int)phy,buffer);
					      wflag = 1;
					   }*/
					}
				}
				pte_unmap(ptep);
			}
			else
			{
				paddr = (pmd_val(*pmd) & PMD_MASK) | (addr & ~PMD_MASK);  
				printk("Use Large Page PSE = 1\n");  
				printk("physical address of 0x%lx is 0x%lx\n", addr, paddr);  
				printk("__pa(vaddr) is 0x%lx", __pa(addr));  
				//phy = paddr;
				/*if(wflag == 0)
				{
				   write_addr((long unsigned int)paddr,addr,buffer);
				   wflag = 1;
			        }*/
			        //write_addr((long unsigned int)paddr,buffer);
				//prev_paddr = paddr;
				for_linux_pmd_count++;
				
			}
		}
	}
}
//write_addr((long unsigned int)addr,buffer);//start vaddr
//write_addr((long unsigned int)paddr,buffer);//physical 
*ppaddr = paddr;
return page;


}


asmlinkage void linux_survey_TT(int input,char *output) {
	struct task_struct *pid_task;
	struct mm_struct *pid_mm;
	struct vm_area_struct *pid_mmap;
	struct page *pid_page = NULL;
	struct page *last_page = NULL;
	phys_addr_t phy,last_phy = 0;
	unsigned long int i = 0,p_mem_count = 0,p_mem_include_0 = 0,paddr = 0;
	unsigned long int addr,prevaddr = 0;
	unsigned long int count = 0;
	/*pgd_t *pgd;
	pmd_t *pmd;
	pte_t *pte;
	pud_t *pud;*/
	
	pid_task = find_task_by_vpid(input);
	pid_mm	 = pid_task->mm;
	pid_mmap = pid_mm->mmap;
	printk(KERN_EMERG "Hello World2 pid = %d \n",input);
	printk(KERN_EMERG "hello world2_new task %d\npid = %d\n",input,pid_task->pid);
	
	while(1)
	{
       	       addr = pid_mmap->vm_start;
       	       count = 0;
       	       wflag = 0;
       	       phy = 0;
       	       paddr = 0;
       	       //write_addr((long unsigned int)addr,output);//start vaddr
       	       while(1)
       	       {
	       	    printk(KERN_EMERG "H2 LINE : %ld vm_start = %lx vm_end = %lx \n",i,addr,addr + 4096);
	       	    
	       	    pid_page = my_follow_page(pid_mm,addr,output,&paddr);
	       	    write_addr((long unsigned int)addr,output);
		    write_addr((long unsigned int)paddr,output);
	       	    if(pid_page == NULL){
		       printk(KERN_EMERG "pid_PAGE = NULL\n");
		       /*if(wflag == 0){
		       	  write_addr((long unsigned int)phy,output);//start paddr
		       	  wflag = 1;
		       }*/
		    
		    }
		    else
		    {
		       
		       phy = page_to_phys(pid_page);
		      /* if(wflag == 0){
		       	  write_addr((long unsigned int)phy,output);//start paddr
		       	  wflag = 1;
		       }*/
		       printk(KERN_EMERG "address = %lx\n",(unsigned long int)phy);
		       if(phy != 0)
		       {	
		       	 if(last_phy != phy)
		       	 {
		 	    p_mem_count++;
		 	    last_phy = phy;
		     	 }
		       }
		       if(pid_page != last_page)
		       	  p_mem_include_0++;
		       last_page = pid_page;
		    }
	       	    count++;
	       	    //if(prevaddr != addr)
	       	       i++;
	       	       prevaddr++;
	       	   // prevaddr = addr;
	       	    if(pid_mmap->vm_end - addr == 4096){
	       	       i--;	
	       	    }
	            if(pid_mmap->vm_end - addr >= 4096){
	               addr = pid_mmap->vm_start + 4096 * count;phy = 0;
	            }
	            else
		       break;	    
	       }
	      // write_addr((long unsigned int)addr,output);//end vaddr
	      // write_addr((long unsigned int)phy,output);// end paddr
	       
	       if(pid_mmap->vm_next == NULL)
		  break;
	       else
	       	  pid_mmap = pid_mmap->vm_next;
		
	} 
	
	printk(KERN_EMERG "total = %ld py = %ld py_include0 = %ld pmd_count = %ld pte_NULL = %ld prevaddr = %ld \n",i,p_mem_count,p_mem_include_0,for_linux_pmd_count,for_linux_pte_NULL,prevaddr);
	write_addr(i,output);
	write_addr(p_mem_count,output);
	write_addr(for_linux_pmd_count,output);
	write_addr(0xffffffff,output);
	for_linux_pmd_count = 0;
	for_linux_pte_NULL  = 0;
	write_count = 0;
	
}
