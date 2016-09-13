#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main()
{
        int amma = syscall(326);
        printf("System call sys_helloworld returns: %Id\n", amma);
	int *ptr;
	ptr = malloc(sizeof(int));
	*ptr = 1;
	int n1,n2;
	n1 = 3;
	n2 = 4;
	int anna = syscall(327,n1,n2,ptr);
	printf("System call sys_simpleadd returns: %Id\n", anna);
	return 0;
}

