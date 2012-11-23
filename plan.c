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


result plan_schedule(Plan plan, Job job, ushort job_id) {
  if ((plan == NULL) || (job == NULL)) return FAIL;

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
