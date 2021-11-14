/*
	Write a program for p-producer c-consumer problem, p, c >= 1. A shared circular buffer that can hold 50 items is to be used. Each producer process can store any number between 1 to 100 (along with the producer id) and deposit in the buffer. Each consumer process reads a number from the buffer and adds it to a shared variable TOTAL (initialized to 0). Though any consumer process can read any of the numbers in the buffer, the only constraint being that every number written by some producer should be read exactly once by exactly one of the consumers. The program reads in the value of p and c from the user, and forks p producers and c consumers. After all the producers and consumers have finished (the consumers exit after all the data produced by all producers have been read), the parent process prints the value of TOTAL. Test the program with different values of p and c.
 */
#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#define BUFFER_LEN 50
#define PER_PRODUCER_MINIMUM 100
#define PER_PRODUCER_RANGE 101
struct requiredmemory
{
	sem_t f, e, m;
	int total, start, count;
	unsigned char circular_buffer[50];
};
int localtotal;
int count;
int id;
void consumerhandler(int sig)
{
	printf("Consumer %d : consumed %d items totalling to %d\n", id, count, localtotal);
	fflush(stdout);
	raise(SIGKILL);
};
int main(int argc, char **argv)
{
	localtotal = 0;
	if (argc != 3)
	{
		char buff[100];
		perror("Arguements mismatch\n");
		sprintf(buff, "usage: %s [procuder_count] [consumer_count]\n", argv[0]);
		perror(buff);
		exit(1);
	}
	int P, C;
	P = atoi(argv[1]);
	C = atoi(argv[2]);
	struct requiredmemory *shared = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	shared->count = 0;
	shared->start = 0;
	shared->total = 0;
	sem_t *full, *empty, *mutex;
	empty = &(shared->e);
	full = &(shared->f);
	mutex = &(shared->m);
	sem_init(empty, 1, 50);
	sem_init(full, 1, 0);
	sem_init(mutex, 1, 1);
	pid_t producers[P], consumers[C];
	int p, c;
	for (c = 0; c < C; c++)
	{
		pid_t pid = fork();
		if (pid < 0)
			goto error_forking_p;
		else if (pid)
		{
			producers[c] = pid;
		}
		else
		{
			id = c;
			goto consumer;
		}
	}
	for (p = 0; p < P; p++)
	{
		pid_t pid = fork();
		if (pid < 0)
			goto error_forking_p;
		else if (pid)
		{
			producers[p] = pid;
		}
		else
		{
			id = p;
			goto producer;
		}
	}
	while (p--)
		wait(NULL);
	while (shared->count)
		sleep(1);
	signal(SIGQUIT, SIG_IGN);
	while (c--)
	{
		kill(consumers[c], SIGQUIT);
	}
	sleep(1);
	printf("Total : %d\n", shared->total);
	exit(0);

error_forking_c:
{
	perror("Some error forking consumers\n");
	for (--c; c >= 0; c--)
		kill(consumers[c], SIGKILL);
	exit(1);
}
error_forking_p:
{
	perror("Some error forking producers\n");
	for (--c; c >= 0; c--)
		kill(consumers[c], SIGKILL);
	for (--p; p >= 0; p--)
		kill(producers[p], SIGKILL);
	exit(1);
};

producer:
{
	srand(clock());
	count = PER_PRODUCER_MINIMUM + rand() % PER_PRODUCER_RANGE;
	int cnt = count;
	while (cnt--)
	{
		unsigned char num = 1 + rand() % 100;
		sem_wait(empty);
		sem_wait(mutex);
		shared->circular_buffer[(shared->start + (shared->count)++) % BUFFER_LEN] = num;
		sem_post(mutex);
		sem_post(full);
		localtotal += num;
	}
	printf("Producer %d : produced %d items totalling to %d\n", id, count, localtotal);
	return 0;
}
consumer:
{
	signal(SIGQUIT, consumerhandler);
	count = 0;
	while (1)
	{
		unsigned char num = 0;
		sem_wait(full);
		sem_wait(mutex);
		num = shared->circular_buffer[shared->start];
		shared->start = (shared->start + 1) % BUFFER_LEN;
		shared->count -= 1;
		shared->total += num;
		sem_post(mutex);
		sem_post(empty);
		count++;
		localtotal += num;
	}
	return 0;
}
}
