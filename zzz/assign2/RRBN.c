#include "q1.h"
int max_cpu_burst_length = 25; // time slice given in question
void get_next_event(event *e)
{
	int ioexp, cpuexp, newarr;
	if (!iobusy)
		ioexp = INT_MAX;
	else
		ioexp = currentiojob.bursts->time - io;
	if (!cpubusy)
		cpuexp = INT_MAX;
	else
		cpuexp = (currentcpujob.bursts->time > max_cpu_burst_length ? max_cpu_burst_length : currentcpujob.bursts->time) - cpu;
	if (empty(unarrived))
		newarr = INT_MAX;
	else
		newarr = front(unarrived).arrival - time;
	if (ioexp == cpuexp && newarr == ioexp && ioexp == INT_MAX)
		*e = done;
	else if (cpuexp <= ioexp && cpuexp <= newarr)
	{
		time += cpuexp;
		if (cpubusy)
			cpu += cpuexp;
		if (iobusy)
			io += cpuexp;
		*e = cpuend;
	}
	else if (ioexp <= newarr)
	{
		time += ioexp;
		if (cpubusy)
			cpu += ioexp;
		if (iobusy)
			io += ioexp;
		*e = ioend;
	}
	else
	{
		time += newarr;
		if (cpubusy)
			cpu += newarr;
		if (iobusy)
			io += newarr;
		*e = new_job;
	}
}

void accept_new_jobs()
{
	while (!empty(unarrived))
	{
		job j = front(unarrived);
		if (j.arrival != time)
			break;
		pop(&unarrived);
		if (currentjobs[j.jobid][0])
			printf("job with job id (%d) arrived at %d cannot be taken as job id is already in use\n", j.jobid, time);
		else
		{
			currentjobs[j.jobid][0] = 1;
			currentjobs[j.jobid][1] = time;
			currentjobs[j.jobid][2] = 0;
			currentjobs[j.jobid][3] = j.priority;
			push(&jobqueue, j);
		}
	}
	if (!cpubusy && !empty(jobqueue))
		put_in_cpu();
}

void process_cpu_end()
{
	struct burststruct *temp = currentcpujob.bursts;
	int current_burst_incomplete = temp->time > cpu, incomp = 1;
	currentjobs[currentcpujob.jobid][2] += cpu;
	if (current_burst_incomplete)
	{
		temp->time -= cpu;
	}
	else
	{
		incomp = (currentcpujob.bursts = currentcpujob.bursts->next) != NULL;
		free(temp);
	}
	cpu = 0;
	if (!incomp)
		store_stats(currentcpujob.jobid);
	else if (!current_burst_incomplete)
	{
		push(&ioqueue, currentcpujob);
		if (!iobusy)
			put_in_io();
	}
	else
		push(&jobqueue, currentcpujob);
	if (empty(jobqueue))
		cpubusy = 0;
	else
		put_in_cpu();
}

void remove_waiting()
{
	struct burststruct *temp = currentiojob.bursts;
	currentiojob.bursts = currentiojob.bursts->next;
	free(temp);
	currentjobs[currentiojob.jobid][2] += io;
	io = 0;
	push(&jobqueue, currentiojob);
	if (!cpubusy)
		put_in_cpu();
	if (empty(ioqueue))
		iobusy = 0;
	else
		put_in_io();
}

void put_in_cpu()
{
	currentcpujob = front(jobqueue);
	pop(&jobqueue);
	cpubusy = 1;
}
void put_in_io()
{
	currentiojob = front(ioqueue);
	pop(&ioqueue);
	iobusy = 1;
}