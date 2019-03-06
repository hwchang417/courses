#include <linux/linkage.h>
#include <linux/kernel.h>
//#include <linux/wait.h>
#include <linux/sched.h>


wait_queue_head_t my_wait_queue1;
int my_wait_queue_head1 = 0;
unsigned char pid_array[32768];

asmlinkage void sys_go_to_sleep_now1(void){
	if(my_wait_queue_head1 == 0){
		int i;
		for(i = 0;i < 32768;i++)
			pid_array[i] = 1;
		init_waitqueue_head(&my_wait_queue1);
	}
	
	pid_array[current->tgid] = 0;
	my_wait_queue_head1 = 1;
	printk(KERN_EMERG "process sleep now");
	wait_event(my_wait_queue1, pid_array[current->tgid]);
	printk(KERN_EMERG "process wake up now");
}
