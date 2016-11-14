#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcnt1.h>

#define BUFFER_SIZE 1024

openSimpleDevice()
{
	open("/dev/simple_character_driver", O_RDWR | O_APPEND);
	getInput();
}

readSimpleDevice()
{
	printf("\nData read from the device:\n");
	system("cat /dev/simple_character_driver");
	getInput();
}

writeSimpleDevice()
{
	printf("\nEnter data you want to write to the device:\n");
	char buffer[BUFFER_SIZE]
	char data[BUFFER_SIZE]
	if (fgets(data, BUFFER_SIZE, stdin) != NULL)
	{
		char * strippedData;
		if ((strippedData=strchr(data, '\n')) != NULL)
		{
			*strippedData = '\0';
		}	
		char cmd[BUFFER_SIZE + 40] = {0x0};
		sprintf(cmd,"echo \"%s\" >> /dev/simple_character_device", data);
		system(cmd);
	}
	getInput();
}

char getInput(void)
{
        printf("Menu\n");
        printf("r : read from device\n");
        printf("w : write to the device\n");
        printf("e : exit from the device\n");
        printf("Make a selection: ");

        char in;
        scanf(" %c",&in);
        if(in == 'r')
        {
                printf("You have selected to read from the device\n");
                readSimpleDevice();
        }
        else if(in == 'w')
        {
                printf("You selected to write to the device\n");
                writeSimpleDevice();
        }
        else if(in == 'e')
        {
		printf("\n---------"
                printf("EXITING\n");
		printf("---------\n"
                exit(0);
        }
        else
        {
                printf("ERROR : PLEASE ENTER VALID SELECTION\n");
                getInput();
        }
}

int main(void)
{
	getInput();
}
