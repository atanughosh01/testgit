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
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <signal.h>
#define READ_END 0
#define WRITE_END 1
#define BUFFER_LEN 50
int flag = 1;

void listener_handler(int sig)
{
	if (sig == SIGINT)
		return;
	if (sig == SIGTERM)
		exit(0);
}

void broadcast_signal_handler(int sig)
{
	flag = 0;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		perror("Number of stations not specified\n");
		return 1;
	}
	int N = atoi(argv[1]);
	pid_t stations[N];
	int fds[N][2];
	int id;
	char buffer[BUFFER_LEN];
	for (id = 0; id < N; id++)
	{
		pipe(fds[id]);
		if (stations[id] = fork())
			goto listen;
		else
			close(fds[id][READ_END]);
	}
	signal(SIGINT, broadcast_signal_handler);
	srand(time(NULL));

	while (flag)
	{
		sleep(rand() % 10);
		time_t t;
		time(&t);
		struct tm *time_val = localtime(&t);
		char *arr[] = {"windy", "clear", "snowy", "rainy", "sunny", "cloudy"};
		sprintf(buffer, "%2d:%2d:%2d The weather is %s.", (time_val->tm_hour) % 12, time_val->tm_min,time_val->tm_sec, arr[rand() % 6]);
		int l = strlen(buffer) + 1;
		for (int i = 0; i < N; i++)
			write(fds[i][WRITE_END], buffer, l);
	}
	for (int i = 0; i < N; i++)
	{kill(stations[i], SIGTERM);}
	sleep(2);
	printf("Ending");
	fflush(stdout);
	return 0;
listen:
{
	signal(SIGINT, listener_handler);
	signal(SIGTERM, listener_handler);
	for (int i = 0; i <= id; i++)
		close(fds[i][WRITE_END]);
	int fd = fds[id][READ_END];
	while (1)
	{
		read(fd, buffer, BUFFER_LEN);
		printf("stat %2d > %s\n", id, buffer);
	}
}
}