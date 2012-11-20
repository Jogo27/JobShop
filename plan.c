#include "plan.h"

#include <stdlib.h>

#include "ressource.h"

struct plan {
  ushort      nb_res;
  int         start;
  Ressource * res;
};


Plan plan_create(int start, ushort nb_res, ushort nb_task) {
  Plan plan = malloc(sizeof(struct plan));
  if (plan == NULL) return NULL;

  plan->nb_res = nb_res;
  plan->start  = start;

  plan->res = calloc(plan->nb_res, sizeof(Ressource));
  if (plan->res == NULL) {
    free(plan);
    return NULL;
  }

  for (int i=0; i < plan->nb_res; i++) {
    plan->res[i] = res_create(plan->start, nb_task);
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


int plan_duration(Plan plan) {
  if (plan == NULL) die("NULL plan");
  return plan->start;
}


result plan_schedule(Plan plan, Job job, ushort job_id) {
  if ((plan == NULL) || (job == NULL)) return FAIL;

  ushort res_id = job_curop_position(job);
  if (res_id >= plan->nb_res) return FAIL;

  Ressource res = plan->res[res_id];
  if (res_add_task(res, job_id, res_id, job_curop_start(job), job_curop_duration(job)) == FAIL) return FAIL;
  if (job_next_op(job, res_start(res)) == FAIL) return FAIL;

  return OK;
}


void plan_output(Plan plan, FILE * stream) {
  if (plan == NULL) die("NULL plan");
  for (int i=0; i < plan->nb_res; i++) {
    res_output(plan->res[i], stream);
  }
}
