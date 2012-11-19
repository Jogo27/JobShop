#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "job.h"
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
  Job job = job_create(0, 2);

  job_add_op(job,1,2);
  job_add_op(job,2,3);
  job_add_op(job,3,4);

  do {
    printf("at %d -> (%d,%d)\n", job_curop_position(job), job_curop_res(job), job_curop_duration(job));
  } while (job_next_op(job,0) == OK);

  Ressource res = res_create(0, 2);
  res_add_task(res, 1, 2, 3, 4);
  res_add_task(res, 2, 3, 4, 1);
  res_add_task(res, 3, 4, 1, 2);

  do {
    printf("at %d do task %d from job %d for %d units of time\n",
        res_curtask_start(res), res_curtask_op(res), res_curtask_job(res), res_curtask_duration(res));
  } while (res_next_task(res) == OK);
}
