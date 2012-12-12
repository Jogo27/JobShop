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
  ushort nb_refs;
  ushort max_pos;
  ushort size;
  int    duration;
  Task * tasks;
};


Ressource res_create(ushort initial_size) {
  Ressource res = malloc(sizeof(struct ressource));
  if (res == NULL) return NULL;

  res->nb_refs = 1;
  res->max_pos = 0;
  res->size = initial_size;
  res->duration = 0;

  res->tasks = calloc(res->size, sizeof(Task));
  if (res->tasks == NULL) {
    free(res);
    return NULL;
  }

  return res;
}

Ressource res_copy(Ressource from) {
  if ((from == NULL) || (from->nb_refs == 0)) return NULL;

  Ressource res = malloc(sizeof(struct ressource));
  if (res == NULL) return NULL;

  res->nb_refs  = 1;
  res->max_pos  = from->max_pos;
  res->size     = from->max_pos;
  res->duration = from->duration;

  res->tasks = calloc(res->size, sizeof(Task));
  if (res->tasks == NULL) {
    free(res);
    return NULL;
  }
  memcpy(res->tasks, from->tasks, res->size * sizeof(Task));

  return res;
}

Ressource res_clone(Ressource res) {
  if ((res == NULL) || (res->nb_refs == 0)) return NULL;

  res->nb_refs += 1;
  return res;
}

result res_free(Ressource res) {
  if ((res == NULL) || (res->nb_refs == 0)) return FAIL;

  res->nb_refs -= 1;
  if (res->nb_refs == 0) {
    free(res->tasks);
    free(res);
  }
  
  return OK;
}

int res_equals(Ressource res_a, Ressource res_b) {
  if (res_a == res_b) return 1;
  if ( (res_a == NULL) || (res_b == NULL)   ||
       (res_a->duration != res_b->duration) ||
       (res_a->max_pos  != res_b->max_pos)
     ) return 0;

  for (int i=0; i < res_a->max_pos; i++)
    if ( (res_a->tasks[i].job != res_b->tasks[i].job) ||
         (res_a->tasks[i].op  != res_b->tasks[i].op)
       ) return 0;
  return 1;
}

// Die if res is invalid
void res_verify(Ressource res) {
  if (res == NULL) die("NULL ressource pointer");
  if (res->nb_refs < 1) die("Invalid ressource");
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

  int start = MAX(res->duration, min_start);
  res->tasks[res->max_pos].job      = job;
  res->tasks[res->max_pos].op       = op;
  res->tasks[res->max_pos].start    = start;
  res->tasks[res->max_pos].jobstart = min_start;
  res->tasks[res->max_pos].duration = duration;

  res->max_pos += 1;
  res->duration = start + duration;

  return OK;
}

ushort res_max_task_id(Ressource res) {
  res_verify(res);
  return res->max_pos;
}

int res_duration(Ressource res) {
  res_verify(res);
  return res->duration;
}

ushort res_task_job(Ressource res, ushort task_id) {
  res_verify(res);
  if (task_id >= res->max_pos) die("task_id out of bound\n");
  return res->tasks[task_id].job;
}

ushort res_task_op(Ressource res, ushort task_id) {
  res_verify(res);
  if (task_id >= res->max_pos) die("task_id out of bound\n");
  return res->tasks[task_id].op;
}

ushort res_task_start(Ressource res, ushort task_id) {
  res_verify(res);
  if (task_id >= res->max_pos) die("task_id out of bound\n");
  return res->tasks[task_id].start;
}

ushort res_task_duration(Ressource res, ushort task_id) {
  res_verify(res);
  if (task_id >= res->max_pos) die("task_id out of bound\n");
  return res->tasks[task_id].duration;
}

ushort res_task_jobstart(Ressource res, ushort task_id) {
  res_verify(res);
  if (task_id >= res->max_pos) die("task_id out of bound\n");
  return res->tasks[task_id].jobstart;
}

int res_swap(Ressource res, ushort task_a_id, ushort task_b_id) {
  if ((res == NULL) || (res->nb_refs != 1) ||
      (task_a_id >= res->max_pos) || (task_b_id >= res->max_pos) ) return FAIL;

  Task * buffer = malloc(sizeof(Task));
  if (buffer == NULL) return FAIL;

  memcpy(buffer,                 &res->tasks[task_a_id], sizeof(Task));
  memcpy(&res->tasks[task_a_id], &res->tasks[task_b_id], sizeof(Task));
  memcpy(&res->tasks[task_b_id], buffer,                 sizeof(Task));
  
  free(buffer);
  return OK;
}

void res_output(Ressource res, FILE* stream) {
  res_verify(res);

  for (int i=0; i < res->max_pos; i++) {
    fprintf(stream, "(%d,%d,%d,%d) ", res_task_job(res,i), res_task_op(res,i), res_task_start(res,i), res_task_duration(res,i));
  }
  fprintf(stream, "\n");
}
