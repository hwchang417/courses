#include <linux/linkage.h>
#include <linux/kernel.h>
//#include <linux/module.h>
#include <linux/sched.h>
//#include <asm/page.h>
//#include <linux/mm.h>
#include <asm/pgtable.h>
#include <asm/pgtable_types.h>
//#include <asm/highmem.h>
#include <linux/highmem.h>

unsigned long int pmd_count = 0, pte_NULL = 0;
wait_queue_head_t my_queue;
unsigned char my_queue_flag;
unsigned char head_test_flag = 0;

asmlinkage int sys_go_to_sleep_now(void) {
	if(head_test_flag == 0)
	   init_waitqueue_head(&my_queue);
	   
	my_queue_flag = 0;
	head_test_flag = 1;
	wait_event_interruptible_exclusive(my_queue,my_queue_flag);
	//printk(KERN_EMERG "Hello World2 pid = %d \n My flag = %d\n",input,my_queue_flag);
	
	
	return 300;
}
