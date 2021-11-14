/* 
   Write a program for the Reader-Writer process for the following situations:
    a) Multiple readers and one writer: writer gets to write whenever it is ready (reader/s wait)
    b) Multiple readers and multiple writers: any writer gets to write whenever it is ready, provided no
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
	sem_t r, w, m;
	int readcount;
	char buffer[50];
};
int localtotal;
int count;
int id;
int main(int argc, char **argv)
{
	localtotal = 0;
	if (argc != 2)
	{
		char buff[100];
		perror("Arguements mismatch\n");
		sprintf(buff, "usage: %s [reader_count]\n", argv[0]);
		perror(buff);
		exit(1);
	}
	int R;
	R = atoi(argv[1]);
	struct requiredmemory *shared = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	strcpy(shared->buffer, "This is the default sentence");
	shared->readcount = 0;
	sem_t *read, *wrt, *mutex;
	read = &(shared->r);
	wrt = &(shared->w);
	mutex = &(shared->m);
	sem_init(read, 1, 1);
	sem_init(wrt, 1, 1);
	sem_init(mutex, 1, 1);
	pid_t readers[R], writer;
	int r;
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
	writer = fork();
	if (writer < 0)
		goto error_forking_w;
	else if (!writer)
	{
		goto writer;
	}
	while (r--)
		wait(NULL);
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
	perror("Some error forking writer\n");
	for (--r; r >= 0; r--)
		kill(readers[r], SIGKILL);
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
		sprintf(buff, "This is line number %3d written by the writer", l);
		sem_wait(read); // must acquire read mutex to stop new readers
		sem_wait(wrt); // must acquire write mutex -- will ensure no reader is  reading
		strcpy(shared->buffer, buff); // do writing
		sem_post(wrt); // release write
		sem_post(read); // release read
	}
	printf("Writer wrote %d lines\n", count);
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
		sem_wait(read); // read mutex is needed for reader entry
		// if a reader hs reached here, the writer has not taken the read mutex
		// so writer will have to wait at least till this reader is done
		sem_wait(mutex); // mutex on readcount to preserve integrity of variable
		if (!shared->readcount) // if no readers are present, write mutex must be free and thus should be acquired
			sem_wait(wrt);
		sem_post(read); // release read now other readers can enter
		shared->readcount++; // increase readcount
		sem_post(mutex); // release mutex so that other readers can proceed
		printf("line %5d read by reader %2d: %s\n", count, id, shared->buffer);// read
		sem_wait(mutex);// acquire mutex
		shared->readcount--;// decrease mutex
		if (!shared->readcount)// if no readers are left, write mutex should be freed
			sem_post(wrt);
		sem_post(mutex);
	}
	printf("reader %d read %d lines\n", id, count);
	return 0;
}
}
