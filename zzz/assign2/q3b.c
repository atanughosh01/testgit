/*
	Write a program for p-producer c-consumer problem, p, c >= 1. A shared circular buffer that can hold 50
	items is to be used. Each producer process can store any number between 1 to 100 (along with the
	producer id) and deposit in the buffer. Each consumer process reads a number from the buffer and adds
	it to a shared variable TOTAL (initialized to 0). Though any consumer process can read any of the
	numbers in the buffer, the only constraint being that every number written by some producer should be
	read exactly once by exactly one of the consumers.
	The program reads in the value of p and c from the user, and forks p producers and c consumers. After
	all the producers and consumers have finished (the consumers exit after all the data produced by all
	producers have been read), the parent process prints the value of TOTAL. Test the program with
	different values of p and c.
 */
#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#define BUFFER_LEN 50
#define PER_WRITER_MINIMUM 10
#define PER_WRITER_RANGE 11
#define PER_READER_MINIMUM 10
#define PER_READER_RANGE 11
struct requiredmemory
{
	sem_t re, we, m1, m2, w;
	int readcount, writecount;
	char buffer[50];
};
int localtotal;
int count;
int id;
int main(int argc, char **argv)
{
	localtotal = 0;
	if (argc != 3)
	{
		char buff[100];
		perror("Arguements mismatch\n");
		sprintf(buff, "usage: %s [reader_count] [writer_count]\n", argv[0]);
		perror(buff);
		exit(1);
	}
	int R, W;
	R = atoi(argv[1]);
	W = atoi(argv[2]);
	struct requiredmemory *shared = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	strcpy(shared->buffer, "This is the default sentence");
	shared->readcount = 0;
	shared->writecount = 0;
	sem_t *readentry, *wrtentry, *mutex1, *mutex2, *wrt;
	readentry = &(shared->re);
	wrtentry = &(shared->we);
	wrt = &(shared->w);
	mutex1 = &(shared->m1);
	mutex2 = &(shared->m2);
	sem_init(readentry, 1, 1);
	sem_init(wrtentry, 1, 1);
	sem_init(wrt, 1, 1);
	sem_init(mutex1, 1, 1);
	sem_init(mutex2, 1, 1);
	pid_t readers[R], writers[W];
	int r, w;
	for (r = 0; r < R; r++)
	{
		pid_t pid = fork();
		if (pid < 0)
			goto error_forking_r;
		else if (pid)
		{
			readers[r] = pid;
		}
		else
		{
			id = r;
			goto reader;
		}
	}
	for (w = 0; w < W; w++)
	{
		pid_t pid = fork();
		if (pid < 0)
			goto error_forking_w;
		else if (pid)
		{
			writers[w] = pid;
		}
		else
		{
			id = w;
			goto writer;
		}
	}
	while (r--)
		wait(NULL);
	while (w--)
		wait(NULL);
	exit(0);

error_forking_r:
{
	perror("Some error forking readers\n");
	for (--r; r >= 0; r--)
		kill(readers[r], SIGKILL);
	exit(1);
};
error_forking_w:
{
	perror("Some error forking writers\n");
	for (--r; r >= 0; r--)
		kill(readers[r], SIGKILL);
	for (--w; w >= 0; w--)
		kill(writers[w], SIGKILL);
	exit(1);
};

writer:
{
	srand(clock());
	count = PER_WRITER_MINIMUM + rand() % PER_WRITER_RANGE;
	int cnt = count;
	int l = 0;
	while (cnt--)
	{
		struct timespec sltime = {0, rand() % 150 << 10};
		nanosleep(&sltime, NULL);
		l++;
		char buff[50];
		sprintf(buff, "writer %2d line %3d", id, l);
		sem_wait(mutex2);// mutex on writecount
		shared->writecount++;
		if (shared->writecount == 1) // if first writer, must acquire readentry to prevent new readers (writer priority)
			sem_wait(readentry);
		sem_post(mutex2); 
		sem_wait(wrt);// wait for wrt mutex
		strcpy(shared->buffer, buff);// write
		sem_post(wrt);// release wrt mutex
		sem_wait(mutex2); 
		shared->writecount--;
		if (shared->writecount == 0)// if no more writers, release readentry so that readers can progress
			sem_post(readentry);
		sem_post(mutex2);
	}
	printf("Writer %2d wrote %3d lines\n", id, count);
	return 0;
}
reader:
{
	srand(clock());
	int l = PER_READER_MINIMUM + rand() % PER_READER_RANGE;
	while (l--)
	{
		struct timespec sltime = {0, rand() % 150 << 10};
		nanosleep(&sltime, NULL);
		count++;
		sem_wait(readentry);// mutex that should be acuired for entry
		sem_wait(mutex1);// mutex on readcount
		if (!shared->readcount)// if first reader, must take wrt mutex to prevent writing
			sem_wait(wrt);
		sem_post(readentry);// unlike writers, readers do not hold onto readentry mutex
		shared->readcount++;
		sem_post(mutex1);
		printf("line %5d read by reader %2d: %s\n", count, id, shared->buffer); // read
		sem_wait(mutex1);
		shared->readcount--; 
		if (!shared->readcount) // if last reader, wrt mutex should be released
			sem_post(wrt);
		sem_post(mutex1);
	}
	printf("reader %2d read %3d lines\n", id, count);
	return 0;
}
}
