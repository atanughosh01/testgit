/* 
	4. Implement the following applications using different IPC mechanisms. Your choice is restricted to
   Pipes, FIFOs, and Message Queues (use different mechanisms for each program)
     a. Broadcasting weather information (one broadcasting process and more than one listeners)
     b. Telephonic conversation (between a caller and a receiver)
     c. Broadcasting information regarding pesticides for agricultural fields / prices of agricultural
          products for marketing with a farmer having the option of putting query (one broadcasting process
          and more than one listeners with option of calling back)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define CALLER0_FIFO "/tmp/caller0"
#define CALLER1_FIFO "/tmp/caller1"
#define BUFFER_LEN 120
void makefifos()
{
	mkfifo(CALLER0_FIFO, 0666);
	mkfifo(CALLER1_FIFO, 0666);
}
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		perror("Caller number not given\n");
		return 1;
	}
	char fifo_mine[13], fifo_other[13];
	char buffer[BUFFER_LEN];
	if (argv[1][0] == '0')
	{
		strcpy(fifo_mine, CALLER0_FIFO);
		strcpy(fifo_other, CALLER1_FIFO);
	}
	else if (argv[1][0] == '1')
	{
		strcpy(fifo_mine, CALLER1_FIFO);
		strcpy(fifo_other, CALLER0_FIFO);
	}
	else
	{
		perror("Caller number not valid\n");
		return 1;
	}
	makefifos();
	pid_t sender = fork();
	if (sender < 0)
	{
		perror("Error forking sender\n");
		return 2;
	}
	if (sender)
	{ // receiver part
		while (1)
		{
			int fifo = open(fifo_mine, O_RDONLY);
			read(fifo, buffer, BUFFER_LEN);
			close(fifo);
			printf("%50s\n", buffer);
		}
	}
	else
	{ // sender part
		while (1)
		{
			scanf("%[^\n]%*c", buffer);
			int fifo = open(fifo_other, O_WRONLY);
			write(fifo, buffer, strlen(buffer) + 1);
			close(fifo);
		}
	}
}