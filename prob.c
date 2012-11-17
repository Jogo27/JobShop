#include "prob.h"

#include <stdlib.h>

#include "main.h"

struct prob {
  unsigned short nb_jobs;
  unsigned short nb_res;
  Job * jobs;
};


Prob prob_parse(FILE* stream) {
  Prob prob = malloc(sizeof(struct prob));
  if (prob == NULL) return NULL;


  // read the header
  fscanf(stream, "%hd %hd", &prob->nb_jobs, &prob->nb_res); // TODO: die if it fails

  prob->jobs = calloc(prob->nb_jobs, sizeof(Job));
  if (prob->jobs == NULL) {
    free(prob);
    return NULL;
  }

  unsigned short res, duration;
  for (int i=0; i < prob->nb_jobs; i++) {
    prob->jobs[i] = job_create(0, prob->nb_res); // TODO: handle failure
    for (int j=0; j < prob->nb_res; j++) {
      fscanf(stream, " %hd %hd", &res, &duration); // TODO: handle failure
      job_add_task(prob->jobs[i], res, duration); // TODO: handle failure
    }
  }

  return prob;
}


Job prob_get_job(Prob prob, int i) {
  if ((prob == NULL) || (i < 0) || (i >= prob->nb_jobs)) return NULL;
  return prob->jobs[i];
}


int prob_job_count(Prob prob) {
  if (prob == NULL) return -1;
  return prob->nb_jobs;
}


int prob_free(Prob prob) {
  if (prob == NULL) return FAIL;
  for (int i=0; i < prob->nb_jobs; i++)
    if (job_free(prob->jobs[i]) == FAIL) return FAIL;
  return OK;
}
