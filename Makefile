CC = gcc
CFLAGS = -g -std=c99
LDFLAGS = 

SRCS = main.c job.c ressource.c prob.o
OBJS = $(SRCS:.c=.o)

all: jobshop

jobshop: ${OBJS}
	${CC} $^ -o $@ ${LDFLAGS}

main.o: main.h job.h prob.h
job.o: job.h main.h
prob.o: prob.h main.h job.h
ressource.o: ressource.h main.h

clean:
	rm *.o ${BINS}

PHONY: all clean
