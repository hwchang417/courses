#include <linux/linkage.h>
#include <linux/kernel.h>
//#include <linux/module.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/list.h>
//#include <linux/mm.h>
#include <asm/pgtable.h>
#include <asm/pgtable_types.h>

#include <linux/highmem.h>

int wake_up_PID = 0;
static void __wake_up_commonf(wait_queue_head_t *q, unsigned int mode,
			int nr_exclusive, int wake_flags, void *key)
{
	wait_queue_t *curr, *next;
	struct task_struct *currtask;
	list_for_each_entry_safe(curr, next, &q->task_list, task_list) {
		unsigned flags = curr->flags;
		currtask = curr->private;
		//printk(KERN_EMERG "HELLO1 wakeup = %d tgid = %d PID = %d flag = %d \n",wake_up_PID,currtask->tgid,currtask->pid,flags);
		if(currtask->tgid == wake_up_PID){
		if (curr->func(curr, mode, wake_flags, key) &&
				(flags & WQ_FLAG_EXCLUSIVE) && !--nr_exclusive )
		
			//printk(KERN_EMERG "IN BREAK\n");
			break;
		
		}
		
	}
	//printk(KERN_EMERG "IN COMMONF\n");
}


void __wake_upf(wait_queue_head_t *q, unsigned int mode,
			int nr_exclusive, void *key)
{
	unsigned long flags;

	spin_lock_irqsave(&q->lock, flags);
	__wake_up_commonf(q, mode, nr_exclusive, 0, key);
	spin_unlock_irqrestore(&q->lock, flags);
}



extern unsigned char my_queue_flag;
extern wait_queue_head_t my_queue;

asmlinkage int sys_wake_up_my_process(int input) {
	my_queue_flag = 1;
	wake_up_PID = input;
	__wake_upf(&my_queue, TASK_INTERRUPTIBLE, 1, NULL);
	//printk(KERN_EMERG "Hello World1 pid = %d \n My flag = %d\n",input,my_queue_flag);
	return input;
	
}
