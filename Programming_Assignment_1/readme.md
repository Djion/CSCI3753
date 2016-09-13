#Contact Information
Name: Alex Ring
Email: Alri7215@colorado.edu

#Important Info
I had some trouble with the dmesg statement that the PDF says to use. It asks us to check sudo tail /var/log/syslog to see our print k statements or type dmesg. I am able to see the printk statements using dmesg but am NOT able to see them in the syslog file we are being directed to. No idea why. But thought it would be worth mentioning.
#Contents of files

###Simpleadd.c
This file contains the system call that we added. It simply takes 2 numbers and 1 pointer as arguments. Prints both numbers, adds them, assigns the pointer to the result, and then prints the pointer. It returns the added arguments result.

###Makefile
The makefile that we changed to compile the two system calls, helloworld and simpleadd. We needed to add the files in order for the kernel to compile them. 

obj-y+=helloworld.o
obj-y+=simple_add.o

Respectively. 	

###syscall_64.tbl
This is the system call table, we had to add the two new system calls we created here, to make sure they were recognized as system calls. 

326     64      hello                   sys_helloworld
327     64      simpleadd               sys_simpleadd

Respectively.

###syscalls.h
This is the header file for the system calls. Like any other header file we needed to add the skeletons of our new system call functions. 

asmlinkage long sys_helloworld(void);
asmlinkage long sys_simpleadd(int number1, int number2, int *result);

Respectively. 

###syslog
This is the system log file. It should contain the printk statements from the system calls that I made using my testfile.

###test.c
This is my test file. It makes 2 calls to the new syscalls that we created. One is to the helloworld call and the other is to simple add. This file then prints out what the two syscalls return. 

Helloworld should return 0.
I am passing simpleadd 3 and 4, so it should return 7. 
