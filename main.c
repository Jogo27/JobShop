#include <stdlib.h>
#include <stdio.h>

#include "job.h"

int main(int argc, char ** argv) {
  Job job = job_new(0, 4);

  job_add_task(job,1,2);
  job_add_task(job,2,3);
  job_add_task(job,3,4);

  do {
    printf("at %d -> (%d,%d)\n", job_cur_position(job), job_cur_res(job), job_cur_duration(job));
  } while (job_next_task(job));
}
