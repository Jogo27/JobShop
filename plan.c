#include "plan.h"

#include <stdlib.h>

#include "ressource.h"

struct plan {
  ushort      nb_res;
  Ressource * res;
};


Plan plan_create(ushort nb_res, ushort nb_task) {
  Plan plan = malloc(sizeof(struct plan));
  if (plan == NULL) return NULL;

  plan->nb_res = nb_res;
  plan->res = calloc(plan->nb_res, sizeof(Ressource));
  if (plan->res == NULL) {
    free(plan);
    return NULL;
  }

  for (int i=0; i < plan->nb_res; i++) {
    plan->res[i] = res_create(nb_task);
    if (plan->res[i] == NULL) {
      for (; i >= 0; i--) res_free(plan->res[i]);
      free(plan->res);
      free(plan);
      return NULL;
    }
  }

  return plan;
}

result plan_free(Plan plan) {
  if (plan == NULL) return FAIL;

  for (int i=0; i < plan->nb_res; i++)
    if (res_free(plan->res[i]) == FAIL) return FAIL;

  free(plan->res);
  free(plan);

  return OK;
}

Ressource plan_get_ressource(Plan plan, ushort res_id) {
  if (plan == NULL) die("NULL plan");
  if (res_id >= plan->nb_res) die("res_id out of bounds\n");
  return plan->res[res_id];
}


int plan_duration(Plan plan) {
  if (plan == NULL) die("NULL plan");
  int duration = 0;
  for (int i=0; i < plan->nb_res; i++)
    duration = MAX(duration, res_duration(plan->res[i]));
  return duration;
}


result plan_schedule(Plan plan, Prob prob, ushort job_id) {
  if ((plan == NULL) || (prob == NULL)) return FAIL;
  Job job = prob_get_job(prob, job_id);

  ushort res_id = job_curop_res(job);
  if (res_id >= plan->nb_res) return FAIL;

  Ressource res = plan->res[res_id];
  if (res_add_task(res, job_id, job_curop_position(job), job_curop_minstart(job), job_curop_duration(job)) == FAIL) return FAIL;
  if (job_next_op(job, res_duration(res)) == FAIL) return FAIL;

  return OK;
}


void plan_output(Plan plan, FILE * stream) {
  if (plan == NULL) die("NULL plan");
  fprintf(stream, "%d %d\n", plan->nb_res, plan_duration(plan));
  for (int i=0; i < plan->nb_res; i++) {
    res_output(plan->res[i], stream);
  }
}

void
plan_neighbourhood(Plan plan, Prob prob, void (*function)(Plan,void *), void * function_data) {
  if ((plan == NULL) || (prob == NULL) || (function == NULL)) die("NULL argument to plan_neighbourhood\n");

  ushort res_pos[plan->nb_res];
  int size = 0;
  for (int i=0; i < plan->nb_res; i++) {
    res_pos[i] = 0;
    size += res_max_task_id(plan->res[i]);
  }

  // We keep track of the sequence of job id until the last permutation
  ushort job_seq[size];

  int job_pos = 0;
  ushort res_id = 0;
  Plan nplan = plan_create(plan->nb_res, size / plan->nb_res);
  prob_unschedule(prob);

  while (job_pos < size) {
    while (res_pos[res_id] >= res_max_task_id(plan->res[res_id])) res_id = (res_id + 1) % plan->nb_res;
    Ressource cur_res = plan->res[res_id];
    ushort cur_pos = res_pos[res_id];

    if ( (res_pos[res_id] + 1 < res_max_task_id(cur_res)) &&
         (res_task_jobstart(cur_res, cur_pos + 1) < res_task_start(cur_res, cur_pos) + res_task_duration(cur_res, cur_pos)) ) {
      // Permutation is possible
      ushort cur_job = res_task_job(cur_res, cur_pos + 1);

      if (res_task_op(cur_res, cur_pos + 1) == job_curop_position(prob_get_job(prob, cur_job))) {
        // permutation
        plan_schedule(nplan, prob, cur_job);
        // TODO terminate nplan
        // TODO call function
        // TODO restart nplan and add cur_pos to job_seq
      }
      else {
        res_id = (res_id + 1) % plan->nb_res;
      }
    }
    else {
      // permutation is impossible
      ushort cur_job = res_task_job(cur_res, cur_pos);

      if (res_task_op(cur_res, cur_pos) == job_curop_position(prob_get_job(prob, cur_job))) {
        plan_schedule(nplan, prob, cur_job);
        job_seq[job_pos++] = cur_job;
        res_pos[res_id] += 1;
      }
      else {
        res_id = (res_id + 1) % plan->nb_res;
      }
    }
  } // while
}
