CC = gcc
CFLAGS = -g
LDFLAGS = 

SRCS = main.c job.c
OBJS = $(SRCS:.c=.o)

all: jobshop

jobshop: ${OBJS}
	${CC} $^ -o $@ ${LDFLAGS}

main.o: main.h job.h
job.o: job.h main.h

clean:
	rm *.o ${BINS}

PHONY: all clean
