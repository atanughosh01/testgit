
.phony:all
all: q1 q2 q3 q4

.phony:q1
q1: FCFS RRBN PRIOR
	

q1-main.o:q1.c q1.h
	@echo "making $@"
	gcc -c $^
	mv	q1.o $@


q1-common.o:q1-common.c q1.h
	@echo "making $@"
	gcc -c $^

q1-skeleton.o: q1-main.o q1-common.o
	@echo "making $@"
	ld -r $^ -o $@

FCFS:FCFS.c q1-skeleton.o
	@echo "making $@"
	gcc	$^ -o $@

RRBN: RRBN.c q1-skeleton.o
	@echo "making $@"
	gcc	$^ -o $@

PRIOR:	PRIOR.c	q1-skeleton.o
	@echo "making $@"
	gcc	$^ -o $@



THR=-l pthread


q2:	q2.c
	@echo "making $@"
	gcc	$^ -o $@ $(THR)


.phony:q3
q3:q3a q3b

q3a:q3a.c
	@echo "making $@"
	gcc	$^ -o $@ $(THR)

q3b:q3b.c
	@echo "making $@"
	gcc	$^ -o $@ $(THR)

.phony:q4
q4: q4b q4a q4c

q4a:q4a.c
	@echo "making $@"
	gcc	$^ -o $@

q4b:q4b.c
	@echo "making $@"
	gcc	$^ -o $@

.phony:q4c
q4c:q4c-client q4c-server


q4c-client:q4c-client.c
	@echo "making $@"
	gcc	$^ -o $@


q4c-server:q4c-server.c
	@echo "making $@"
	gcc	$^ -o $@


.phony:clear
clear:
	-rm *.o 2> /dev/null
	-find . -maxdepth 1 -type f -executable | xargs rm 2> /dev/null
