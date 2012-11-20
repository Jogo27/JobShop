#ifndef PROB_H
#define PROB_H

#include <stdio.h>

#include "main.h"
#include "job.h"

struct prob;
typedef struct prob * Prob;

// Create a prob from a stream
extern Prob prob_parse(FILE* stream);

// Get a job from a prob
extern Job prob_get_job(Prob prob, int i);

// Get number of jobs
extern ushort prob_job_count(Prob prob);

extern ushort prop_res_count(Prob prob);

extern result prob_free(Prob prob);

#endif
