#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "job.h"
#include "prob.h"
#include "ressource.h"

void
die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


int main(int argc, char ** argv) {
  Prob prob = prob_parse(stdin);

  for (int i=0; i < prob_job_count(prob); i++) {
    Job job = prob_get_job(prob, i);
    do {
      printf("%d, %d -> %d, %d\n", i, job_curop_position(job), job_curop_res(job), job_curop_duration(job));
    } while (job_next_op(job,0) == OK);
  }
}
