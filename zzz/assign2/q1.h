#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define __MAX_MASK__ 127
#ifndef Q1_H
#define Q1_H
struct burststruct
{
	int time;
	struct burststruct *next;
};
typedef struct jobstruct
{
	int jobid, priority, arrival;
	struct burststruct *bursts;
} job;

struct jobnode
{
	job j;
	struct jobnode *next;
};

typedef struct jbqstr
{
	struct jobnode *head, *tail;
} job_queue;

typedef enum
{
	new_job,
	cpuend,
	ioend,
	done
} event;

struct stat
{
	int jobid, arrival, finish, wait, priority;
};
#endif

extern int time;
extern int cpu, io;
extern job_queue unarrived, jobqueue, ioqueue;
extern int currentjobs[__MAX_MASK__][4];
extern int cpubusy, iobusy;
extern job currentcpujob, currentiojob;
extern int count;
extern struct stat stats[1000];
extern job dummy;

extern void process_cpu_end();
extern void put_in_cpu();
extern void put_in_io();
extern void remove_waiting();
extern void accept_new_jobs();
extern void get_next_event(event *);

/* basic queue operations */
#pragma region queue_operations

extern void init_q(job_queue *q);

extern void push(job_queue *q, job j);

extern int empty(job_queue q);

extern job front(job_queue q);

extern void pop(job_queue *q);
#pragma endregion

/* parses jobs from a file called "jobs" */
extern int parse_jobs();

/* store stats regarding a completed job with job_id id */
extern void store_stats(int id);

/* displays results */
extern void display_results();
