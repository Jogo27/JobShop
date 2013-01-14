/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Programme principal
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

char debug_mode = 0;
char repetitions = 1;
char output_plan = 0;

void debug(const char *format, ...) {
  if (debug_mode) {
    va_list ap;
    va_start(ap, format);
    vfprintf((output_plan == 1 ? stderr : stdout), format, ap);
    va_end(ap);
  }
}

void info(const char *format, ...) {
  if (debug_mode || (repetitions == 1)) {
    va_list ap;
    va_start(ap, format);
    vfprintf((output_plan == 1 ? stderr : stdout), format, ap);
    va_end(ap);
  }
}

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);
extern Plan sch_localy(Prob prob);
extern Plan sch_local_opt(Prob prob);
extern Plan sch_vns(Prob prob);
extern Plan sch_genetic(Prob prob);
extern Plan sch_order(Prob prob);
extern Plan sch_mutations(Prob prob);

static void verify(char * prob_fn, char * plan_fn) {
  FILE * file = fopen(prob_fn,"r");
  if (file == NULL) die("Unable to open %s : %s\n.", prob_fn, strerror(errno));
  Prob prob = prob_parse(file);
  fclose(file);

  file = fopen(plan_fn,"r");
  if (file == NULL) die("Unable to open %s : %s\n.", plan_fn, strerror(errno));
  Plan plan = plan_parse(file,prob);
  fclose(file);

  Plan replay = plan_verify(plan, prob);
  if (replay != NULL) {
    printf("The solution %s is admissible for the instance %s.\nThe makespan is %d.\n",
           plan_fn, prob_fn, plan_makespan(replay));
    plan_free(replay);
  }
  else {
    printf("The solution %s is NOT admissible for the instance %s.\n",
           plan_fn, prob_fn);
  }

  plan_free(plan);
  prob_free(prob);
}

static void print_help(FILE * stream, char * prog) {
  fprintf(stream, "Usage: %s [-d] [-n <repetitions>] [-r|-g|-l|-L|-v|-G|-m|-o] <file>\n", prog);
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
        break;
      case 'g':
        algo = &sch_greedy;
        break;
      case 'l':
        algo = &sch_localy;
        break;
      case 'L':
        algo = &sch_local_opt;
        break;
      case 'v':
        algo = &sch_vns;
        break;
      case 'G':
        algo = &sch_genetic;
        break;
      case 'm':
        algo = &sch_mutations;
        break;
      case 'o':
        algo = &sch_order;
        break;
      case 'd':
        debug_mode = 1;
        break;
      case 'n':
        arg += 1;
        repetitions = atoi(argv[arg]);
        break;
      case 'V':
        arg += 1;
        verify(argv[arg], argv[arg+1]);
        exit(0);
        break;
      case 'h':
        print_help(stdout,argv[0]);
        break;
      default:
        fprintf(stderr, "Unknown argument %s.\n", argv[arg]);
        print_help(stderr,argv[0]);
        break;
    }
    arg += 1;
  }
  output_plan = (debug_mode || ((arg + 1) == argc)) && (repetitions == 1);
  FILE * info_out = (output_plan == 1 ? stderr : stdout);

  // Main
  for (; arg < argc; arg++) {
    FILE * file = fopen(argv[arg],"r");
    if (file == NULL) die("Unable to open %s : %s\n.", argv[arg], strerror(errno));
    if (debug_mode)
      fprintf(info_out, "Processing %s\n", argv[arg]);
    else
      fprintf(info_out, "%s ", argv[arg]);
    Prob prob = prob_parse(file);
    fclose(file);

    long duration = 0;
    long makespan = 0;
    Plan plan = NULL;
    for (int i=0; i < repetitions; i++) {
      if (plan != NULL) plan_free(plan);
      clock_t c = clock();
      plan = (*algo)(prob);
      duration += clock() - c;
      makespan += plan_makespan(plan);
    }
    fprintf(info_out, " makespan %4.2f duration %.2f s\n",
        (double)makespan / (double) repetitions,
        (double)duration / (double)((long)CLOCKS_PER_SEC * (long)repetitions)
    );
    if (output_plan) plan_output(plan, stdout);

    plan_free(plan);
    prob_free(prob);
  }
}
