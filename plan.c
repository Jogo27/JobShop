#include "plan.h"

#include <stdlib.h>

#include "ressource.h"

struct plan {
  ushort      nb_res;
  Ressource * res;
};


Plan plan_create_empty(Prob prob) {
  Plan plan = malloc(sizeof(struct plan));
  if (plan == NULL) return NULL;

  plan->nb_res = prob_res_count(prob);
  plan->res = calloc(plan->nb_res, sizeof(Ressource));
  if (plan->res == NULL) {
    free(plan);
    return NULL;
  }

  return plan;
}

Plan plan_create(Prob prob) {
  Plan plan = plan_create_empty(prob);
  if (plan == NULL) return NULL;

  for (int i=0; i < plan->nb_res; i++) {
    plan->res[i] = res_create(prob_job_count(prob));
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

int plan_equals(Plan plan_a, Plan plan_b) {
  if ((plan_a == NULL) && (plan_b == NULL)) return 1;
  if ((plan_a == NULL) || (plan_b == NULL) || (plan_a->nb_res != plan_b->nb_res)) return 0;

  for (int i=0; i < plan_a->nb_res; i++)
    if (!res_equals(plan_a->res[i], plan_b->res[i])) return 0;
  return 1;
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
  if (res_add_task(res, job, job_id) == FAIL) return FAIL;
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

Plan
plan_replay(Plan plan, Prob prob) {
  if ((plan == NULL) || (prob == NULL)) die("NULL argument to plan_replay\n");

  Plan ret = plan_create(prob);
  ushort res_id = 0;
  ushort guard = 0; // used to prevent infinite loop in case where the plan is invalid
  prob_unschedule(prob);
  while (!prob_is_scheduled(prob)) {
    ushort task_id  = res_max_task_id(ret->res[res_id]);
    ushort task_max = res_max_task_id(plan->res[res_id]);

    while (task_id < task_max) {
      ushort job_id = res_task_job(plan->res[res_id], task_id);
      if (job_curop_res(prob_get_job(prob, job_id)) == res_id) {
        guard = 0;
        plan_schedule(ret, prob, job_id);
        task_id += 1;
      }
      else {
        task_id = task_max;
      }
    }

    // Prevent infinite loop
    guard += 1;
    if (guard > plan->nb_res) {
//      plan_output(plan, stdout);
//      plan_output(ret, stdout);
      plan_free(ret);
      return NULL;
    }

    res_id = (res_id + 1) % plan->nb_res;
  }

  return ret;
}

void
plan_neighbourhood(Plan plan, Prob prob, void (*function)(Plan,void *), void * function_data) {
  if ((plan == NULL) || (prob == NULL) || (function == NULL)) die("NULL argument to plan_neighbourhood\n");

  for (int res_id = 0; res_id < plan->nb_res; res_id++) {
    for (int task_id = 0; task_id + 1 < res_max_task_id(plan->res[res_id]); task_id++) {
      Ressource res = plan->res[res_id];

      if (res_task_jobstart(res, task_id + 1) < res_task_start(res, task_id) + res_task_duration(res, task_id)) {

        Plan nplan = malloc(sizeof(struct plan));
        if (nplan == NULL) die("Unable to allocate neighbour plan\n");

        nplan->nb_res = plan->nb_res;
        nplan->res = calloc(nplan->nb_res, sizeof(Ressource));
        if (nplan->res == NULL) die("Unable to allocate ressources for neighbour plan\n");

        for (int i=0; i < nplan->nb_res; i++) {
          if (i == res_id) {
            nplan->res[i] = res_copy(plan->res[i]);
            res_swap(nplan->res[i], task_id, task_id + 1);
          }
          else {
            nplan->res[i] = res_clone(plan->res[i]);
          }
        }

        (*function)(plan_replay(nplan,prob), function_data);
        plan_free(nplan);
      }

    }
  }

}

Plan plan_merge(Plan plan_a, Plan plan_b, Prob prob) {
  /* Gene are ressources. If the ressource is the last to be released in its parent, the other one is choosen.
   * Othewise, it's random. */
  if ((plan_a == NULL) || (plan_b == NULL)) die("NULL plan for plan_merge\n");
  if ((plan_a->nb_res != plan_b->nb_res) || (plan_a->nb_res != prob_res_count(prob))) die("Plans' size doesn't match in plan_merge\n");

  Plan draft = plan_create_empty(prob);
  if (draft == NULL) return NULL;

  int makespan_a = plan_duration(plan_a);
  int makespan_b = plan_duration(plan_b);
  for (int i=0; i < plan_a->nb_res; i++) {
    if (res_duration(plan_a->res[i]) == makespan_a) {
      if ((makespan_a < makespan_b) && (res_duration(plan_b->res[i]) == makespan_b))
        draft->res[i] = res_clone(plan_a->res[i]);
      else
        draft->res[i] = res_clone(plan_b->res[i]);
    }
    else
      draft->res[i] = res_clone(((rand() & 1) ? plan_a : plan_b)->res[i]);
  }

  Plan ret = plan_replay(draft, prob);
  plan_free(draft);
  return ret;
}
