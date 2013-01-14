/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Structure de données pour l'ordonnancement d'une seule ressource
 */

#include "ressource.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

typedef struct {
  ushort job;
  ushort duration;
  ushort start;
  ushort jobstart;
} Task;

struct ressource {
  ushort nb_refs;
  ushort max_pos;
  ushort size;
  ushort makespan;
  Task * tasks;
};


Ressource res_create(ushort initial_size) {
  Ressource res = malloc(sizeof(struct ressource));
  if (res == NULL) return NULL;

  res->nb_refs = 1;
  res->max_pos = 0;
  res->size = initial_size;
  res->makespan = 0;

  res->tasks = calloc(res->size, sizeof(Task));
  if (res->tasks == NULL) {
    free(res);
    return NULL;
  }

  return res;
}

// Die if res is invalid
void res_verify(Ressource res) {
  if (res == NULL) die("NULL ressource pointer");
  if (res->nb_refs < 1) die("Invalid ressource");
}

Ressource res_copy(Ressource from) {
  if ((from == NULL) || (from->nb_refs == 0)) return NULL;

  Ressource res = malloc(sizeof(struct ressource));
  if (res == NULL) return NULL;

  res->nb_refs  = 1;
  res->max_pos  = from->max_pos;
  res->size     = from->max_pos;
  res->makespan = from->makespan;

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

void res_prepare_for_write(Ressource * res) {
  if (*res == NULL) return;
  if ((*res)->nb_refs < 2) return;

  Ressource buffer = *res;
  *res = res_copy(buffer);
  res_free(buffer);
}

result res_free(Ressource res) {
  if ((res == NULL) || (res->nb_refs == 0)) die("Deleting invalid ressource\n");

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
       (res_a->makespan != res_b->makespan) ||
       (res_a->max_pos  != res_b->max_pos)
     ) return 0;

  for (int i=0; i < res_a->max_pos; i++)
    if (res_a->tasks[i].job != res_b->tasks[i].job) return 0;
  return 1;
}

int res_equals_no_makespan(Ressource res_a, Ressource res_b) {
  if (res_a == res_b) return 1;
  if ( (res_a == NULL) || (res_b == NULL)   ||
       (res_a->max_pos  != res_b->max_pos)
     ) return 0;

  for (int i=0; i < res_a->max_pos; i++)
    if (res_a->tasks[i].job != res_b->tasks[i].job) return 0;
  return 1;
}

result res_add_task_low(Ressource res, ushort job_id, ushort min_start, ushort duration) {
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

  ushort start = MAX(res->makespan, min_start);
  res->tasks[res->max_pos].job      = job_id;
  res->tasks[res->max_pos].start    = start;
  res->tasks[res->max_pos].jobstart = min_start;
  res->tasks[res->max_pos].duration = duration;

  res->max_pos += 1;
  res->makespan = start + duration;

  return OK;
}

result res_add_task(Ressource res, Job job, ushort job_id) {
  res_verify(res);
  if (job == NULL) die("NULL job in res_add_task\n");

  ushort min_start = job_curop_minstart(job);
  ushort duration  = job_curop_duration(job);
  return res_add_task_low(res, job_id, min_start, duration);
}

ushort res_max_task_id(Ressource res) {
  res_verify(res);
  return res->max_pos;
}

ushort res_makespan(Ressource res) {
  res_verify(res);
  return res->makespan;
}

ushort res_task_job(Ressource res, ushort task_id) {
  res_verify(res);
  if (task_id >= res->max_pos) die("task_id out of bound\n");
  return res->tasks[task_id].job;
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

result res_move(Ressource res, ushort a, ushort b) {
  if ((res == NULL) || (res->nb_refs != 1) ||
      (a >= res->max_pos) || (b >= res->max_pos) ) return FAIL;
  if (a == b) return OK;

  Task buffer;

  memcpy(&buffer, &res->tasks[a], sizeof(Task));
  if (a < b)
    memmove(&res->tasks[a], &res->tasks[a + 1], (b - a) * sizeof(Task));
  else
    memmove(&res->tasks[b + 1], &res->tasks[b], (a - b) * sizeof(Task));
  memcpy(&res->tasks[b], &buffer, sizeof(Task));
  
  return OK;
}

#define TASK_INVALID USHRT_MAX

Ressource * res_crossover_onepoint (Ressource res_a, Ressource res_b) {
  res_verify(res_a);
  res_verify(res_b);
  if (res_a->max_pos != res_b->max_pos) die("Not matching ressources for res_crossover_onepoint\n");

  Ressource * ret = calloc(2, sizeof(Ressource));
  if (ret == NULL) die("Unable to allocation the result array in res_crossover_onepoint\n");

  ushort size = res_a->max_pos;
  ushort quarter_size = size / 4;
  ushort pivot = quarter_size + rand() % (size - 2 * quarter_size);

  // left to right child
  ret[0] = res_copy(res_a);
  for (int i=0; i < size; i++) {
    ushort in_b = pivot;
    while ((res_a->tasks[i].job != res_b->tasks[in_b].job) && (in_b < size)) in_b += 1;

    if (in_b < size)
      ret[0]->tasks[i].job = TASK_INVALID;
    else {
      int j = i;
      while ((j > 0) && (ret[0]->tasks[j-1].job == TASK_INVALID)) j -= 1;
      if (i != j) {
        memcpy(&ret[0]->tasks[j], &ret[0]->tasks[i], sizeof(Task));
        ret[0]->tasks[i].job = TASK_INVALID;
      }
    }
  }
  memcpy(&ret[0]->tasks[pivot], &res_b->tasks[pivot], (size - pivot) * sizeof(Task));

  // right to left child
  ret[1] = res_copy(res_b);
  for (int i=size-1; i >= 0; i--) {
    ushort in_a = 0;
    while ((res_b->tasks[i].job != res_a->tasks[in_a].job) && (in_a < pivot)) in_a += 1;

    if (in_a < pivot)
      ret[1]->tasks[i].job = TASK_INVALID;
    else {
      int j = i;
      while ((j + 1 < size) && (ret[1]->tasks[j+1].job == TASK_INVALID)) j += 1;
      if (i != j) {
        memcpy(&ret[1]->tasks[j], &ret[1]->tasks[i], sizeof(Task));
        ret[1]->tasks[i].job = TASK_INVALID;
      }
    }
  }
  memcpy(&ret[1]->tasks[0], &res_a->tasks[0], pivot * sizeof(Task));

  return ret;
}

// For efficiency, sqmat_get and sqmat_set need to be static.
#include "square_matrice.c"

SqMat res_incident_matrice(Ressource res) {
  res_verify(res);
  ushort size = res->max_pos;

  SqMat im = sqmat_create(size);
  if (im == NULL) die("Unable to create incident matrice\n");

  for (int i=0; i < size; i++)
    sqmat_set(im, i, i, 0);

  for (int task_id=0; task_id < (size - 1); task_id++) {
    ushort job_id = res->tasks[task_id].job;
    for (int i=0; i < size; i++)
      if (i != job_id) {
        if (sqmat_get(im, job_id, i) == SM_NULL)
          sqmat_set(im, job_id, i, -1);
        if (sqmat_get(im, i, job_id) == SM_NULL)
          sqmat_set(im, i, job_id,  1);
      }
  }

  return im;
}

Ressource res_create_from_partial_order (SqMat order, ushort size) {
  Ressource res = res_create(size);
  if (res == NULL) die("Unable to allocate ressource in res_create_from_partial_order\n");

  ushort id = rand() % size;
  while (res->max_pos < size) {
    ushort y = 0;

    schar v;
    do {
      v = sqmat_get(order, id, y);
    } while (((v == -2) || (v == 0)) && (++y < size));

    if (y == size) {
      res_add_task_low(res, id, 0, 1);
      sqmat_set(order, id, 0, SM_NULL);
      for (int i=0; i < size; i++)
        if (sqmat_get(order, i, id) == 2) sqmat_set(order, i, id, 0);
      id = rand() % size;
    }
    else
      id = (id + 1) % size;
  }

  return res;
}

Ressource * res_crossover_order(Ressource res_a, Ressource res_b) {
  res_verify(res_a);
  res_verify(res_b);
  if (res_a->max_pos != res_b->max_pos) die("Not matching ressources for res_crossover_order\n");

  SqMat im_a = res_incident_matrice(res_a);
  SqMat im_b = res_incident_matrice(res_b);
  SqMat order_a = sqmat_add(im_a, im_b);
  sqmat_free(im_a);
  sqmat_free(im_b);
  if (order_a == NULL) die("The partial order matrice is NULL in res_crossover_order\n");
  SqMat order_b = sqmat_copy(order_a);

  Ressource * ret = calloc(2, sizeof(Ressource));
  if (ret == NULL) die("Unable to allocation the result array in res_crossover_order\n");
  ushort size = res_a->max_pos;
  ret[0] = res_create_from_partial_order(order_a, size);
  sqmat_free(order_a);
  ret[1] = res_create_from_partial_order(order_b, size);
  sqmat_free(order_b);

  return ret;
}

void res_output(Ressource res, FILE* stream) {
  res_verify(res);

  for (int i=0; i < res->max_pos; i++) {
    fprintf(stream, "%d ", res_task_job(res,i));
  }
  fprintf(stream, "\n");
}
