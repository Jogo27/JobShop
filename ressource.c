#include "ressource.h"

#include <stdlib.h>
#include <string.h>

#include "main.h"

typedef struct {
  ushort job;
  ushort op;
  int    start;
  ushort duration;
  int    jobstart;
} Task;

struct ressource {
  ushort cur_pos;
  ushort max_pos;
  size_t size;
  int    start;
  Task * tasks;
};


Ressource res_create(int start, ushort initial_size) {
  Ressource res = malloc(sizeof(struct ressource));
  if (res == NULL) return NULL;

  res->cur_pos = 0;
  res->max_pos = 0;
  res->size = initial_size;
  res->start = start;

  res->tasks = calloc(res->size, sizeof(Task));
  if (res->tasks == NULL) {
    free(res);
    return NULL;
  }

  return res;
}

result res_free(Ressource res) {
  if (res == NULL) return FAIL;

  free(res->tasks);
  free(res);
  
  return OK;
}

// Die if res is invalid
void res_verify(Ressource res) {
  if (res == NULL) die("NULL ressource pointer");
}

result res_add_task(Ressource res, ushort job, ushort op, int min_start, ushort duration) {
  res_verify(res);

  size_t length = res->size;
  if (length < 1) length = 1;

  if (res->max_pos >= length) {
    res->size = length * 2;
    Task * new_tasks = calloc(res->size, sizeof(Task));
    if (new_tasks == NULL) return FAIL;
    memcpy(new_tasks, res->tasks, length * sizeof(Task));
    free(res->tasks);
    res->tasks = new_tasks;
  }

  int start = (res->start > min_start ? res->start : min_start);
  res->tasks[res->max_pos].job      = job;
  res->tasks[res->max_pos].op       = op;
  res->tasks[res->max_pos].start    = start;
  res->tasks[res->max_pos].jobstart = min_start;
  res->tasks[res->max_pos].duration = duration;

  res->max_pos += 1;
  res->start = start + duration;

  return OK;
}

int res_start(Ressource res) {
  res_verify(res);
  return res->start;
}

ushort res_curtask_job(Ressource res) {
  res_verify(res);
  return res->tasks[res->cur_pos].job;
}

ushort res_curtask_op(Ressource res) {
  res_verify(res);
  return res->tasks[res->cur_pos].op;
}

ushort res_curtask_start(Ressource res) {
  res_verify(res);
  return res->tasks[res->cur_pos].start;
}

ushort res_curtask_duration(Ressource res) {
  res_verify(res);
  return res->tasks[res->cur_pos].duration;
}

ushort res_curtask_jobstart(Ressource res) {
  res_verify(res);
  return res->tasks[res->cur_pos].jobstart;
}


result res_next_task(Ressource res) {
  res_verify(res);

  if (res->cur_pos < res->max_pos - 1) {
    res->cur_pos += 1;
    return OK;
  }
  else
    return FAIL;
}

result res_rewind_task(Ressource res) {
  res_verify(res);
  res->cur_pos = 0;
  return (res->max_pos ? OK : FAIL);
}

void res_output(Ressource res, FILE* stream) {
  res_verify(res);

  for (res_rewind_task(res); res_next_task(res);) {
    fprintf(stream, "(%d,%d,%d) ", res_curtask_job(res), res_curtask_op(res), res_curtask_start(res));
  }
  fprintf(stream, "\n");
}
