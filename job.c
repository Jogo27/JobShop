#include "job.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
  ushort res;
  ushort duration;
} Op;

struct job {
  ushort cur_pos;
  ushort max_pos;
  size_t size;
  int start;
  Op * op;
};

Job job_create(ushort initial_size) {
  Job job = malloc(sizeof(struct job));
  if (job == NULL) return NULL;

  job->cur_pos = 0;
  job->max_pos = 0;
  job->start = 0;
  job->size = initial_size;

  job->op = calloc(initial_size, sizeof(Op));
  if (job->op == NULL) {
    free(job);
    return NULL;
  }

  return job;
}

result job_add_op(Job job, ushort res, ushort duration) {
  if (job == NULL) return FAIL;

  size_t length = job->size;
  if (length < 1) length = 1;

  if (job->max_pos >= length) {
    job->size = length * 2;
    Op * new_op = calloc(job->size, sizeof(Op));
    if (new_op == NULL) return FAIL;
    memcpy(new_op, job->op, length * sizeof(Op));
    free(job->op);
    job->op = new_op;
  }

  job->op[job->max_pos].res      = res;
  job->op[job->max_pos].duration = duration;
  job->max_pos += 1;

  return OK;
}

ushort job_curop_position(Job job) {
  if (job == NULL) die("NULL pointer for job_curop_position\n");
  return job->cur_pos;
}

int job_curop_minstart(Job job) {
  if (job == NULL) die("NULL pointer for job_curop_start\n");
  return job->start;
}

ushort job_curop_res(Job job) {
  if (job == NULL) die("NULL pointer for job_curop_res\n");
  return job->op[job->cur_pos].res;
}

ushort job_curop_duration(Job job) {
  if (job == NULL) die("NULL pointer for job_curop_duration\n");
  return job->op[job->cur_pos].duration;
}

result job_unschedule(Job job) {
  if (job == NULL) return FAIL;
  job->cur_pos = 0;
  job->start = 0;
  return OK;
}

result job_next_op(Job job, int next_minstart) {
  if (job == NULL) return FAIL;

  if (job->cur_pos < job->max_pos) {
    int job_minstart = job->start + job->op[job->cur_pos].duration;
    job->cur_pos += 1;
    job->start = MAX(next_minstart, job_minstart);
    return OK;
  }
  else
    return FAIL;
}

int job_is_scheduled(Job job) {
  if (job == NULL) die("NULL pointer for job_is_scheduled\n");
  return (job->cur_pos >= job->max_pos);
}

result job_free(Job job) {
  if (job == NULL) return FAIL;
  free(job->op);
  free(job);
  return OK;
}
