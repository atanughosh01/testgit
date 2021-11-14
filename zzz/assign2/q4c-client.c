/* 
   Implement the following applications using different IPC mechanisms. Your choice is restricted to
   Pipes, FIFOs, and Message Queues (use different mechanisms for each program)
     a. Broadcasting weather information (one broadcasting process and more than one listeners)
     b. Telephonic conversation (between a caller and a receiver)
     c. Broadcasting information regarding pesticides for agricultural fields / prices of agricultural
          products for marketing with a farmer having the option of putting query (one broadcasting process
          and more than one listeners with option of calling back)
*/
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		perror("give station name\n");
		return 1;
	}
	key_t k = ftok(argv[0], atoi(argv[1]));
	int myqid = msgget(k, IPC_CREAT | 0666);
	k = ftok("q4c-server", 'S');
	int serverqid = msgget(k, 0666);
	struct
	{
		long mtype;
		int qid;
	} m1;
	struct
	{
		long mtype;
		char message[50];
	} m;
	m1.mtype = 1;
	m1.qid = myqid;
	msgsnd(serverqid, &m1, sizeof(m1), 0);
	pid_t receiver = fork();
	if (receiver)
	{
		printf("Enter 1 to get pesticide info and 2 to get price info\n");
		while (1)
		{
			int ch = 0;
			scanf("%d", &ch);
			if (ch == 1)
			{
				m1.mtype = 2;
				msgsnd(serverqid, &m1, sizeof(m1), 0);
			}
			else if (ch == 2)
			{
				m1.mtype = 3;
				msgsnd(serverqid, &m1, sizeof(m1), 0);
			}
			else
			{
				printf("invalid choice\n");
			}
		}
	}
	else
	{
		while (1)
		{
			m.mtype = 0;
			if (msgrcv(myqid, &m, sizeof(m), 0, 0))
			{
				if (m.mtype == 1)
					printf("%-50s (broadcast message for pesticide)\n", m.message);
				else if (m.mtype == 2)
					printf("%-50s (broadcast message for price)\n", m.message);
				else if (m.mtype == 3)
					printf("%-50s (reply for pesticide)\n", m.message);
				else if (m.mtype == 4)
					printf("%-50s (reply for price)\n", m.message);
			}
		}
	}
}