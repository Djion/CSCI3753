#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<asm/uaccess.h>

#define BUFFER_SIZE 1024
#define DEVICE_ID 240

static char device_buffer[BUFFER_SIZE];

int openCount = 0;
int closeCount = 0;
int placeholder = 0;

static ssize_t myDriver_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	int bytesRead;
	int bytesToRead = BUFFER_SIZE - *offset;

	if(bytesToRead == 0)
	{
		printk(KERN_ALERT "END OF FILE");
		return bytesToRead;
	}

	bytesRead = bytesToRead - copy_to_user(buffer, device_buffer + *offset, bytesToRead);
	printk(KERN_ALERT "Reading with Simple Character Driver. Reading %d bytes\n",bytesRead);

	*offset += bytesRead;
	return bytesRead;
}

static ssize_t myDriver_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	int bytesToWrite;
	int bytesWritten;
	int bytesAvailable = BUFFER_SIZE - *offset - placeholder;

	if(bytesAvailable > length)
	{
		bytesToWrite = length;
	}
	else
	{
		bytesToWrite = bytesAvailable;
	}

	bytesWritten = bytesToWrite - copy_from_user(device_buffer + *offset + placeholder, buffer, bytesToWrite);
	
	if(bytesWritten == 0) 
	{
		printk(KERN_ALERT "The device is out of space.\n");
	}
	else
	{
		*offset += bytesWritten;
		placeholder += bytesWritten;

		printk(KERN_ALERT "Writing with simple character driver. Writing %d bytes\n", bytesWritten);
	}
	return bytesWritten;
}

static int myDriver_open(struct inode *pinode, struct file *pfile)
{
        openCount++;
        printk(KERN_ALERT "OPENED. Count =%d\n",openCount);
        return 0;
}

static int myDriver_close(struct inode *pinode, struct file *pfile)
{
        closeCount++;
        printk(KERN_ALERT "CLOSED. Count =%d\n",closeCount);
        return 0;
}

static struct file_operations memory_fops=
{
	.owner = THIS_MODULE,
	.read = myDriver_read,
	.write = myDriver_write,
	.open = myDriver_open,
	.release = myDriver_close
};



int myDriver_init(void)
{
	printk(KERN_ALERT "DRIVER FOR SIMPLE CHARACTER DRIVER LOADED");
	register_chrdev(240,"simple_character_device",&memory_fops);
	return 0;
}

int myDriver_exit(void)
{
	printk(KERN_ALERT "DRIVER FOR SIMPLE CHARACTER DRIVER UNLOADED");
	unregister_chrdev(240,"simple_character_device");
	return 0;	
}


module_init(myDriver_init);
module_exit(myDriver_exit);

