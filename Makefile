CC = gcc
CFLAGS = -std=c99 -Winline -g
LDFLAGS = -Winline -g

SRCS = main.c job.c ressource.c prob.c plan.c random.c greedy.c local.c population.c ngenetic.c
OBJS = $(addprefix lib/,$(SRCS:.c=.o))

all: jobshop

#vpath %.o lib
vpath %.c src
vpath %.h src

lib/%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

jobshop: ${OBJS}
	${CC} $^ -o $@ ${LDFLAGS}

lib/main.o: main.h job.h prob.h plan.h
lib/job.o: job.h main.h
lib/prob.o: prob.h main.h job.h
lib/ressource.o: ressource.h main.h job.h square_matrice.c
lib/plan.o: plan.h main.h ressource.h job.h prob.h
lib/random.o: prob.h job.h plan.h
lib/greedy.o: prob.h job.h plan.h main.h ressource.h
lib/local.o: plan.h prob.h main.h
lib/population.o: population.h main.h plan.h
lib/ngenetic.o: main.h plan.h prob.h population.h

clean:
	rm lib/*.o jobshop

PHONY: all clean
