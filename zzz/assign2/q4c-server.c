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
struct sharedmem
{
	int broadcastList[99], count;
	char pesticideinfo[50];
	char pricesinfo[50];
};

int main()
{
	srand(time(NULL));
	pid_t request_manager;
	struct
	{
		long message_type;
		char message[50];
	} m;
	struct sharedmem *shared = mmap(NULL, sizeof(struct sharedmem), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	shared->count = 0;
	strcpy(shared->pesticideinfo, "default Avalailable pesticides =  40");
	strcpy(shared->pricesinfo, "default Price =  150");
	if (request_manager = fork())
	{
		while (1)
		{
			sleep(rand() % 10);
			if (rand() % 2)
			{
				time_t t;
				time(&t);
				struct tm *time_val = localtime(&t);
				sprintf(shared->pesticideinfo, "%2d:%2d:%2d Available persticides = %3d", time_val->tm_hour % 12, time_val->tm_min, time_val->tm_sec, rand() % 51);
				m.message_type = 1;
				strcpy(m.message, shared->pesticideinfo);
				for (int i = 0; i < shared->count; i++)
				{
					msgsnd(shared->broadcastList[i], &m, sizeof(m), 0);
				}
			}
			sleep(rand() % 4);
			if (rand() % 2)
			{
				time_t t;
				time(&t);
				struct tm *time_val = localtime(&t);
				sprintf(shared->pricesinfo, "%2d:%2d:%2d Price = %3d", time_val->tm_hour % 12, time_val->tm_min, time_val->tm_sec, 100 + rand() % 101);
				m.message_type = 2;
				strcpy(m.message, shared->pricesinfo);
				for (int i = 0; i < shared->count; i++)
				{
					msgsnd(shared->broadcastList[i], &m, sizeof(m), 0);
				}
			}
		}
	}
	else
	{
		key_t k = ftok("q4c-server", 'S');
		int msqid = msgget(k, 0666 | IPC_CREAT);
		struct
		{
			long message_type;
			int messageqid;
		} m1;
		m.message[0] = 'R';
		m.message[1] = 'e';
		m.message[2] = ':';
		m.message[3] = ' ';
		while (1)
		{
			m1.message_type = 0;
			if (msgrcv(msqid, &m1, sizeof(m1), 0, 0))
			{
				if (m1.message_type == 1)
				{ // add to broadcast list
					printf("adding to broadcast list\n");
					int found = 0;
					for (int i = 0; i < shared->count; i++)
					{
						if (shared->broadcastList[i] == m1.messageqid)
						{
							found = 1;
							break;
						}
					}
					if (!found)
					{
						shared->broadcastList[shared->count] = m1.messageqid;
						shared->count++;
					}
				}
				else if (m1.message_type == 2)
				{ // send pesticide info
					m.message_type = 3;
					printf("request for pesticide info\n");
					strcpy(m.message + 4, shared->pesticideinfo);
					msgsnd(m1.messageqid, &m, sizeof(m), 0);
				}
				else if (m1.message_type == 3)
				{ // send prices info
					m.message_type = 4;
					printf("request for price info\n");
					strcpy(m.message + 4, shared->pricesinfo);
					msgsnd(m1.messageqid, &m, sizeof(m), 0);
				}
			}
		}
	}
}