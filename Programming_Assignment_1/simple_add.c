#include <linux/kernel.h>
#include <linux/linkage.h>
asmlinkage long sys_simpleadd(int number1, int number2, int *result)
{
        printk(KERN_ALERT "Number1: %d",number1);
        printk(KERN_ALERT "Number2: %d",number2);
        *result = number1 + number2;
        printk(KERN_ALERT "Result %p", result);
        return number1 + number2;
}

