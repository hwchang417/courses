#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/list.h>

extern wait_queue_head_t my_wait_queue1;
extern unsigned char pid_array[32768];

asmlinkage int sys_wake_up_my_process1(int pid){
	int exist;
	
	if(pid_array[pid] != 1){
		exist = 1;
		pid_array[pid] = 1;
		printk(KERN_EMERG "start wake up\n");
		__wake_up(&my_wait_queue1, TASK_UNINTERRUPTIBLE, 0, NULL);	
		printk(KERN_EMERG "end wake up\n");
	}else
		exist = -1;
	
	return exist;
}
