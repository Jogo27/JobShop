#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "job.h"
#include "prob.h"
#include "plan.h"

void
die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);
extern Plan sch_localy(Prob prob);
extern Plan sch_genetic(Prob prob);

void print_help(FILE * stream, char * prog) {
  fprintf(stream, "Usage: %s [-r|-g|-l|-G] <file>\n", prog);
  exit(1);
}

int main(int argc, char ** argv) {

  //Arguments
  int arg = 1;
  Plan (*algo)(Prob) = &sch_greedy;
  while ((arg < argc) && (argv[arg][0] == '-')) {
    switch (argv[arg][1]) {
      case 'r':
        algo = &sch_random;
        arg += 1;
        break;
      case 'g':
        algo = &sch_greedy;
        arg += 1;
        break;
      case 'l':
        algo = &sch_localy;
        arg += 1;
        break;
      case 'G':
        algo = &sch_genetic;
        arg += 1;
        break;
      case 'h':
        print_help(stdout,argv[0]);
        break;
      default:
        fprintf(stderr, "Unknown argument %s.\n", argv[arg]);
        print_help(stderr,argv[0]);
        break;
    }
  }

  // Main
  for (; arg < argc; arg++) {
    FILE * file = fopen(argv[arg],"r");
    if (file == NULL) die("Unable to open %s : %s\n.", argv[arg], strerror(errno));
    Prob prob = prob_parse(file);
    Plan plan = (*algo)(prob);
    plan_output(plan, stdout);
    fclose(file);
  }
}
