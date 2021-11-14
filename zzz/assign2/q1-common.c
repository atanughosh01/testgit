#include "q1.h"

/* basic queue operations */
#pragma region queue_operations

extern job dummy;
extern int count;
void init_q(job_queue *q)
{
	q->head = q->tail = NULL;
}

void push(job_queue *q, job j)
{
	if (q->head)
		q->tail = q->tail->next = (struct jobnode *)malloc(sizeof(struct jobnode));
	else
		q->tail = q->head = (struct jobnode *)malloc(sizeof(struct jobnode));
	q->tail->j = j;
}

int empty(job_queue q)
{
	return !q.head;
}

job front(job_queue q)
{
	if (q.head)
		return q.head->j;
	else
		return dummy;
}

void pop(job_queue *q)
{
	if (q->head)
	{
		if (q->head == q->tail)
		{
			free(q->head);
			q->head = q->tail = NULL;
		}
		else
		{
			struct jobnode *t = q->head;
			q->head = q->head->next;
			free(t);
		}
	}
}

#pragma endregion

/* parses jobs from a file called "jobs" */
int parse_jobs()
{
	/*
	 * Assumption: job descriptions are given in primarily non-decreasing order of time and secondarily non-decreasing order of priority number
	 */
	FILE *f = fopen("jobs", "r");
	if (f == NULL)
	{
		perror("Could not open file\n");
		return 1;
	}
	int l = 0;
	while (1)
	{
		l++;
		int id, pr, atime;
		if (fscanf(f, "%d", &id) == EOF)
			break;
		fscanf(f, "%d", &pr);
		fscanf(f, "%d", &atime);
		id &= __MAX_MASK__;
		job j;
		j.jobid = id;
		j.priority = pr;
		j.arrival = atime;
		struct burststruct h, *cur;
		h.next = NULL;
		cur = &h;
		int fl = 1;
		while (1)
		{
			int t;
			fscanf(f, "%d", &t);
			if (t == -1)
				break;
			cur = cur->next = (struct burststruct *)malloc(sizeof(struct burststruct));
			cur->time = t;
			fl = !fl;
		}
		if (!h.next)
		{
			printf("job at line %d ends with has no bursts. Ingoring it.\n", l);
			continue;
		}
		if (fl)
		{
			printf("job at line %d ends with an IO burst. Ingoring it.\n", l);
			continue;
		}
		j.bursts = h.next;
		push(&unarrived, j);
	}
	return 0;
}

/* store stats regarding a completed job with job_id id */
void store_stats(int id)
{
	struct stat *s = stats + count++;
	s->jobid = id;
	s->arrival = currentjobs[id][1];
	s->finish = time;
	s->wait = time - s->arrival - currentjobs[id][2];
	s->priority = currentjobs[id][3];
	currentjobs[id][0] = 0;
}

/* displays results */
void display_results()
{
	if (count == 0)
	{
		printf("No Results to display!\n");
		return;
	}
	int i;
#define REPEAT(n, ch) \
	i = n;            \
	while (i--)       \
		printf("%c", ch);
	REPEAT(1, '\t')
	REPEAT(1, '+')
	REPEAT(132, '-')
	REPEAT(1, '+')
	REPEAT(1, '\n')
	printf("\t|%5s|%10s|%10s|%20s|%20s|%20s|%20s|%20s|\n", "s.no.", "job id","priority", "arrival time", "queue wait time", "finish time", "turnaround time","wait time %");
	REPEAT(1, '\t')
	REPEAT(1, '|')
	REPEAT(5, '-')
	REPEAT(1, '+')
	REPEAT(10, '-')
	REPEAT(1, '+')
	REPEAT(10, '-')
	REPEAT(1, '+')
	REPEAT(20, '-')
	REPEAT(1, '+')
	REPEAT(20, '-')
	REPEAT(1, '+')
	REPEAT(20, '-')
	REPEAT(1, '+')
	REPEAT(20, '-')
	REPEAT(1, '+')
	REPEAT(20, '-')
	REPEAT(1, '|')
	REPEAT(1, '\n')
	int TTA, TWT;
	TTA = TWT = 0;
	for (i = 0; i < count; i++)
	{
		printf("\t|%5d|%10d|%10d|%20d|%20d|%20d|%20d|%18.3f %%|\n", i + 1, stats[i].jobid,stats[i].priority, stats[i].arrival, stats[i].wait, stats[i].finish, stats[i].finish - stats[i].arrival,(100.00 * stats[i].wait)/(stats[i].finish-stats[i].arrival));
		TWT += stats[i].wait;
		TTA += stats[i].finish - stats[i].arrival;
	}
	REPEAT(1, '\t')
	REPEAT(1, '+')
	REPEAT(132, '-')
	REPEAT(1, '+')
	REPEAT(2, '\n')
	printf("average turnaround time\t%.3f\n", ((float)TTA) / count);
	printf("average queue wait time\t%.3f\n", ((float)TWT) / count);

#undef REPEAT
}