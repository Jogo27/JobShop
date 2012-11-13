#include "job.h"

#include <stdlib.h>
#include <string.h>

#include "main.h"

typedef struct {
  unsigned short res;
  unsigned short duration;
} Task;

struct job {
  unsigned short cur_pos;
  unsigned short max_pos;
  size_t size;
  int start;
  Task * task;
};

Job job_new(int start, unsigned short initial_size) {
  Job job = malloc(sizeof(struct job));
  if (job == NULL) return NULL;

  job->cur_pos = 0;
  job->max_pos = 0;
  job->start = start;
  job->size = initial_size;

  job->task = calloc(initial_size, sizeof(Task));
  if (job->task == NULL) {
    free(job);
    return NULL;
  }

  return job;
}

int job_add_task(Job job, unsigned short res, unsigned short duration) {
  if (job == NULL) return FAIL;

  size_t length = job->size;
  if (length < 1) length = 1;

  if (job->max_pos >= length) {
    job->size = length * 2;
    Task * new_task = calloc(job->size, sizeof(Task));
    if (new_task == NULL) return FAIL;
    memcpy(new_task, job->task, length * sizeof(Task));
    free(job->task);
    job->task = new_task;
  }

  job->task[job->max_pos].res      = res;
  job->task[job->max_pos].duration = duration;
  job->max_pos += 1;

  return OK;
}

unsigned short job_cur_position(Job job) {
  return job->cur_pos;
}

int            job_start(Job job) {
  return job->start;
}

unsigned short job_cur_res(Job job) {
  return job->task[job->cur_pos].res;
}

unsigned short job_cur_duration(Job job) {
  return job->task[job->cur_pos].duration;
}

int job_next_task(Job job, int start) {
  if (job == NULL) return FAIL;
  job->start = start;
  if (job->cur_pos < job->max_pos) job->cur_pos += 1;
  return (job->cur_pos < job->max_pos ? OK : FAIL);
}

int job_free(Job job) {
  if (job == NULL) return FAIL;
  free(job->task);
  free(job);
  return OK;
}
