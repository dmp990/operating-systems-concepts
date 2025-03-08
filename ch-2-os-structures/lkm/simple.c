#include <asm/param.h>
#include <linux/gcd.h>
#include <linux/hash.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/kernel.h>

/* This function is called when the module is loaded. */
int simple_init(void)
{
       printk(KERN_INFO "Golden Ratio Prime: %lu, jiffies: %lu, HZ: %d\n", GOLDEN_RATIO_PRIME, jiffies, HZ);

       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	unsigned long a = 3300;
        int b = 24;
	printk(KERN_INFO "gcd(%lu, %d) = %lu, jiffies: %lu\n", a, b, gcd(a, b), jiffies);
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

