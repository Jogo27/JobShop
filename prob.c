#include "prob.h"

#include <stdlib.h>


struct prob {
  ushort nb_jobs;
  ushort nb_res;
  Job * jobs;
};


Prob prob_parse(FILE* stream) {
  Prob prob = malloc(sizeof(struct prob));
  if (prob == NULL) return NULL;


  // read the header
  if (fscanf(stream, "%hd %hd", &prob->nb_jobs, &prob->nb_res) != 2) die("File format error in header\n");

  prob->jobs = calloc(prob->nb_jobs, sizeof(Job));
  if (prob->jobs == NULL) {
    free(prob);
    return NULL;
  }

  ushort res, duration;
  for (int i=0; i < prob->nb_jobs; i++) {
    prob->jobs[i] = job_create(prob->nb_res);
    if (prob->jobs[i] == NULL) die("Unable to allocate job %d\n", i);
    for (int j=0; j < prob->nb_res; j++) {
      if (fscanf(stream, " %hd %hd", &res, &duration) != 2) die("File format error at line %d operation %d\n", i+1, j+1);
      if (job_add_op(prob->jobs[i], res, duration) == FAIL) die("Unable to add operation %d to job %d\n", i, j);
    }
  }

  return prob;
}

result prob_free(Prob prob) {
  if (prob == NULL) return FAIL;
  for (int i=0; i < prob->nb_jobs; i++)
    if (job_free(prob->jobs[i]) == FAIL) return FAIL;
  free(prob->jobs);
  free(prob);
  return OK;
}


Job prob_get_job(Prob prob, int i) {
  if ((prob == NULL) || (i < 0) || (i >= prob->nb_jobs)) return NULL;
  return prob->jobs[i];
}


ushort prob_res_count(Prob prob) {
  if (prob == NULL) die("NULL prob pointer in prob_res_count\n");
  return prob->nb_res;
}

ushort prob_job_count(Prob prob) {
  if (prob == NULL) die("NULL prob pointer in prob_job_count\n");
  return prob->nb_jobs;
}

ushort prob_is_scheduled(Prob prob) {
  if (prob == NULL) die("NULL prob pointer in prob_is_scheduled\n");

  for (int i=0; i < prob->nb_jobs; i++)
    if (!job_is_scheduled(prob->jobs[i])) return 0;
  return 1;
}

result prob_unschedule(Prob prob) {
  if (prob == NULL) die("NULL prob pointer in prob_unschedule\n");
  for (int i=0; i < prob->nb_jobs; i++)
    if (job_unschedule(prob->jobs[i]) == FAIL) return FAIL;
  return OK;
}

