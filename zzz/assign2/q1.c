#include "q1.h"
int time = 0;
int cpu = 0, io = 0;
job_queue unarrived, jobqueue, ioqueue;
int currentjobs[__MAX_MASK__][4] = {0};
int cpubusy = 0, iobusy = 0;
job currentcpujob, currentiojob;
int count = 0;
struct stat stats[1000];
job dummy = {-1, -1, -1, NULL};
int main()
{
	init_q(&unarrived);
	init_q(&ioqueue);
	init_q(&jobqueue);
	event currentevent = new_job;
	if (parse_jobs())
	{
		return 1;
	}
	if (empty(unarrived))
		goto DONE;
#define GO_TO_NEXT_EVENT           \
	get_next_event(&currentevent); \
	switch (currentevent)          \
	{                              \
	case new_job:                  \
		goto NEWJOB;               \
	case cpuend:                   \
		goto CPUEND;               \
	case ioend:                    \
		goto IOEND;                \
	case done:                     \
		goto DONE;                 \
	}
	GO_TO_NEXT_EVENT
NEWJOB:
	accept_new_jobs();
	GO_TO_NEXT_EVENT
CPUEND:
	process_cpu_end();
	GO_TO_NEXT_EVENT
IOEND:
	remove_waiting();
	GO_TO_NEXT_EVENT
DONE:
	display_results();
#undef GO_TO_NEXT_EVENT
}
