#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "job.h"
#include "prob.h"

int main(int argc, char ** argv) {
  Prob prob = prob_parse(stdin);

  for (int i=0; i < prob_job_count(prob); i++) {
    Job job = prob_get_job(prob, i);
    do {
      printf("%d, %d -> %d, %d\n", i, job_cur_position(job), job_cur_res(job), job_cur_duration(job));
    } while (job_next_task(job,0) == OK);
  }
}
