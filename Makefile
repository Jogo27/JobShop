CC = gcc
CFLAGS = -g -std=c99 -pg
LDFLAGS = -pg

SRCS = main.c job.c ressource.c prob.c plan.c random.c greedy.c local.c population.c ngenetic.c
OBJS = $(SRCS:.c=.o)

all: jobshop

jobshop: ${OBJS}
	${CC} $^ -o $@ ${LDFLAGS}

main.o: main.h job.h prob.h plan.h
job.o: job.h main.h
prob.o: prob.h main.h job.h
ressource.o: ressource.h main.h job.h
plan.o: plan.h main.h ressource.h job.h prob.h
random.o: prob.h job.h plan.h
greedy.o: prob.h job.h plan.h main.h ressource.h
local.o: plan.h prob.h main.h
population.o: population.h main.h plan.h
ngenetic.o: main.h plan.h prob.h population.h

clean:
	rm *.o jobshop

PHONY: all clean
