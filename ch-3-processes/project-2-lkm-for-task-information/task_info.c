/*
LKM that uses the /proc file system for displaying a task's information
based on its process identifier pid.
*/
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

/* the current pid */
static long pid;

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
ssize_t proc_write(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write,
};

/* This function is called when the module is loaded. */
int proc_init(void)
{

	// creates the /proc/seconds entry
	// the following function call is a wrapper for
	// proc_create_data() passing NULL as the last argument
	proc_create(PROC_NAME, 0666, NULL, &proc_ops);

	printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void)
{

	// removes the /proc/seconds entry
	remove_proc_entry(PROC_NAME, NULL);

	printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/hello is read.
 *
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
	int rv = 0;
	char buffer[BUFFER_SIZE];
	static int completed = 0;

	if (completed)
	{
		completed = 0;
		return 0;
	}

	completed = 1;

	/*
	PCB in Linux is represented by the structure task_struct from <linux/sched.h>.
	Given a PID, pid_task() returns associated task_struct. The signature of this
	function appears as follows:
	struct task_struct pid_task(struct pid *pid,
	   enum pid_type type)
	*/
	struct task_struct *tsk = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (!tsk)
	{
		printk(KERN_INFO "Invalid PID\n");
		return -1;
	}

	/*
	Example return:
	command = [bash] pid = [1395] state = [1]
	*/
	rv = sprintf(buffer, "command = [%s] pid = [%d] state = [%ld]\n", tsk->comm, tsk->pid, tsk->state);

	// copies the contents of buffer to userspace usr_buf
	if (copy_to_user(usr_buf, buffer, rv))
	{
		rv = -1;
	}

	return rv;
}

ssize_t proc_write(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
	char *k_mem;

	/* allocate kernel memory */
	k_mem = kmalloc(count, GFP_KERNEL);

	/* copies user space usr_buf to kernel memory */
	if (copy_from_user(k_mem, usr_buf, count))
	{
		printk(KERN_INFO "Error copying from user\n");
		return -1;
	}

	/*
	Convert from char * to long
	int kstrtol(const char *s, unsigned int base, long *res)
	*/
	sscanf(k_mem, "%d", &pid);

	/* return kernel memory */
	kfree(k_mem);

	return count;
}

/* Macros for registering module entry and exit points. */
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Task Info Reporter");
MODULE_AUTHOR("AA");
