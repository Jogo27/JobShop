/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Structure de données pour les solutions
 */

#include "plan.h"

#include <stdlib.h>

#include "ressource.h"

struct plan {
  ushort      nb_res;
  Ressource * res;
};


Plan plan_create_empty(ushort nb_res) {
  Plan plan = malloc(sizeof(struct plan));
  if (plan == NULL) return NULL;

  plan->nb_res = nb_res;
  plan->res = calloc(plan->nb_res, sizeof(Ressource));
  if (plan->res == NULL) {
    free(plan);
    return NULL;
  }

  return plan;
}

Plan plan_create(Prob prob) {
  Plan plan = plan_create_empty(prob_res_count(prob));
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

Plan plan_clone(Plan plan) {
  Plan ret = plan_create_empty(plan->nb_res);
  if (ret == NULL) return NULL;

  for (int i=0; i < ret->nb_res; i++)
    ret->res[i] = res_clone(plan->res[i]);

  return ret;
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

ushort plan_makespan(Plan plan) {
  if (plan == NULL) die("NULL plan");
  ushort makespan = 0;
  for (int i=0; i < plan->nb_res; i++)
    makespan = MAX(makespan, res_makespan(plan->res[i]));
  return makespan;
}

ushort plan_sum_makespan(Plan plan) {
  if (plan == NULL) die("NULL plan");
  ushort first  = 0;
  ushort second = 0;
  for (int i=0; i < plan->nb_res; i++) {
    ushort makespan = res_makespan(plan->res[i]);
    if (makespan > first) {
      second = first;
      first  = makespan;
    }
    else if (makespan > second)
      second = makespan;
  }
  return first + second;
}

result plan_schedule(Plan plan, Prob prob, ushort job_id) {
  if ((plan == NULL) || (prob == NULL)) return FAIL;
  Job job = prob_get_job(prob, job_id);

  ushort res_id = job_curop_res(job);
  if (res_id >= plan->nb_res) return FAIL;

  Ressource res = plan->res[res_id];
  if (res_add_task(res, job, job_id) == FAIL) return FAIL;
  if (job_next_op(job, res_makespan(res)) == FAIL) return FAIL;

  return OK;
}


void plan_output(Plan plan, FILE * stream) {
  if (plan == NULL) die("NULL plan");
  for (int i=0; i < plan->nb_res; i++) {
    res_output(plan->res[i], stream);
  }
}

static Plan plan_replay(Plan plan, Prob prob) {
  if ((plan == NULL) || (prob == NULL)) die("NULL argument to plan_replay\n");

  Plan ret = plan_create(prob);
  ushort res_id = 0;
  prob_unschedule(prob);
  while (!prob_is_scheduled(prob)) {
    ushort task_id  = res_max_task_id(ret->res[res_id]);
    ushort task_max = res_max_task_id(plan->res[res_id]);

    while (task_id < task_max) {
      ushort job_id = res_task_job(plan->res[res_id], task_id);
      if (job_curop_res(prob_get_job(prob, job_id)) == res_id) {
        plan_schedule(ret, prob, job_id);
        task_id += 1;
      }
      else {
        task_id = task_max;
      }
    }

    res_id = (res_id + 1) % plan->nb_res;
  }

  return ret;
}

static Plan plan_repair(Plan plan_arg, Prob prob) {
  if ((plan_arg == NULL) || (prob == NULL)) die("NULL argument to plan_repair\n");

  Plan plan = plan_clone(plan_arg);
  Plan ret  = plan_create(prob);
  prob_unschedule(prob);

  ushort res_id = 0;
  ushort guard = 0;
  ushort forward = 0;
  char cont = 1;
  while (cont) {

    while (cont && (guard < plan->nb_res)) {

      ushort task_id  = res_max_task_id(ret->res[res_id]);
      ushort task_max = res_max_task_id(plan->res[res_id]);

      while (task_id + forward < task_max) {
        ushort job_id = res_task_job(plan->res[res_id], task_id + forward);
        if (job_curop_res(prob_get_job(prob, job_id)) == res_id) {
          plan_schedule(ret, prob, job_id);
          guard = 0;
          if (forward) {
            res_prepare_for_write(&plan->res[res_id]);
            res_move(plan->res[res_id], task_id + forward, task_id);
            forward = 0;
          }
          cont = !prob_is_scheduled(prob);
          task_id += 1;
        }
        else {
          task_id = task_max;
        }
      }

      guard += 1;
      res_id = (res_id + 1) % plan->nb_res;
    } // while guard

    forward += 1;
    guard = 0;

  } // while not scheduled

  plan_free(plan);
  return ret;
}

static Plan plan_replay_move(Plan plan, Prob prob, ushort res_id, ushort task_a_id, ushort task_b_id) {
  if ((plan == NULL) || (prob == NULL)) die("NULL argument to plan_replay_move\n");
  if (res_id >= plan->nb_res) die("res_id out of bounds for plan_replay_move\n");

  Plan nplan = plan_create_empty(plan->nb_res);
  if (nplan == NULL) return NULL;

  for (int i=0; i < nplan->nb_res; i++) {
    if (i == res_id) {
      nplan->res[i] = res_copy(plan->res[i]);
      res_move(nplan->res[i], task_a_id, task_b_id);
    }
    else {
      nplan->res[i] = res_clone(plan->res[i]);
    }
  }

  Plan ret = plan_replay(nplan, prob);
  plan_free(nplan);
  return ret;
}

void
plan_neighbourhood_one(Plan plan, ushort res_id, Prob prob, void (*function)(Plan,void *), void * function_data) {
  if ((plan == NULL) || (prob == NULL) || (function == NULL)) die("NULL argument to plan_neighbourhood_one\n");
  if (res_id > plan->nb_res) die("res_id out of bound for plan_neighbourhood_one\n"); 
  Ressource res = plan->res[res_id];

  for (int task_id = 0; task_id + 1 < res_max_task_id(plan->res[res_id]); task_id++) {
    if (res_task_jobstart(res, task_id + 1) < res_task_start(res, task_id) + res_task_duration(res, task_id)) {
      Plan nplan = plan_replay_move(plan, prob, res_id, task_id, task_id + 1);
      (*function)(nplan, function_data);
    }
  }
}

void
plan_neighbourhood_perm(Plan plan, ushort res_id, Prob prob, void (*function)(Plan,void *), void * function_data) {
  if ((plan == NULL) || (prob == NULL) || (function == NULL)) die("NULL argument to plan_neighbourhood_one\n");
  if (res_id > plan->nb_res) die("res_id out of bound for plan_neighbourhood_one\n"); 
  Ressource res = plan->res[res_id];

  for (int i = 0; i + 1 < res_max_task_id(plan->res[res_id]); i++) {
    ushort termination = res_task_start(res, i) + res_task_duration(res, i);
    for (int j = i + 1; j < res_max_task_id(plan->res[res_id]); j++) {
      if (res_task_jobstart(res, j) < termination) {
        Plan nplan = plan_replay_move(plan, prob, res_id, j, i);
        (*function)(nplan, function_data);
      }
    }
  }
}

void
plan_neighbourhood_worse(Plan plan, Prob prob, void (*function)(Plan,void *), void * function_data) {
  if ((plan == NULL) || (prob == NULL) || (function == NULL)) die("NULL argument to plan_neighbourhood\n");

  ushort makespan = plan_makespan(plan);
  for (int res_id = 0; res_id < plan->nb_res; res_id++)
    if (makespan == res_makespan(plan->res[res_id]))
      plan_neighbourhood_one(plan, res_id, prob, function, function_data);
}

void
plan_neighbourhood(Plan plan, Prob prob, void (*function)(Plan,void *), void * function_data) {
  if ((plan == NULL) || (prob == NULL) || (function == NULL)) die("NULL argument to plan_neighbourhood\n");

  for (int res_id = 0; res_id < plan->nb_res; res_id++)
    plan_neighbourhood_one(plan, res_id, prob, function, function_data);
}

Plan plan_reduce_critical_path(Plan plan, Prob prob) {
  if ((plan == NULL) || (prob == NULL)) die("NULL argument to plan_reduce_critical_path\n");

  ushort makespan = plan_makespan(plan);
  ushort res_id = 0;
  while (res_makespan(plan->res[res_id]) < makespan) res_id += 1;
  debug("res %d ", res_id);

  // Search for inactivity of the critical ressource
  Ressource res = plan->res[res_id];
  ushort task_id = res_max_task_id(plan->res[res_id]) - 1;
  while ((task_id > 0) &&
         (res_task_jobstart(res, task_id) <= res_task_start(res, task_id - 1) + res_task_duration(res, task_id - 1))) {
    task_id -= 1;
  }
  debug("task %d ", task_id);
  if (task_id <= 0) return NULL;

  // Get the corresponding operation
  ushort job_id = res_task_job(res, task_id);
  Job job = prob_get_job(prob, job_id);
  ushort op_id = 1;
  while (job_op_res(job, op_id) != res_id) op_id += 1;
  debug("job %d op %d ", job_id, op_id);
  
  // Search a permutation
  ushort jobstart;
  do {
    if (op_id == 0)
      return NULL;
    op_id -= 1;
    res_id = job_op_res(job, op_id);
    res = plan->res[res_id];
    if (res_task_job(res, 0) == job_id)
      return NULL;
    task_id = 1;
    while (res_task_job(res, task_id) != job_id) task_id += 1;
    jobstart = res_task_jobstart(res, task_id);
  } while (jobstart == res_task_start(res, task_id));
  ushort move_to = task_id - 1;
  while ((move_to > 0) && (jobstart < res_task_start(res, move_to))) move_to -= 1;
  debug("perm %d with %d ", task_id, move_to);

  return plan_replay_move(plan, prob, res_id, task_id, move_to);
}

void plan_crossover_task (Plan plan_a, Plan plan_b, Prob prob,
            Ressource * (*res_crossover)(Ressource,Ressource), process_new_plan function, void * function_data) {
  if ((plan_a == NULL) || (plan_b == NULL)) die("NULL plan for plan_crossover_task\n");
  if ((plan_a->nb_res != plan_b->nb_res) || (plan_a->nb_res != prob_res_count(prob))) die("Plans' size doesn't match in plan_crossover_task\n");

  Plan draft_a = plan_create_empty(plan_a->nb_res);
  if (draft_a == NULL) return;
  Plan draft_b = plan_create_empty(plan_a->nb_res);
  // We die here because we can't cleanly free draft_a
  if (draft_b == NULL) die("Unable to allocate memory in plan_crossover_task\n");

  for (int i=0; i < plan_a->nb_res; i++) {
    if (res_equals(plan_a->res[i], plan_b->res[i])) {
      draft_a->res[i] = res_clone(plan_a->res[i]);
      draft_b->res[i] = res_clone(plan_a->res[i]);
    }
    else {
      Ressource * res = (*res_crossover)(plan_a->res[i], plan_b->res[i]);
      draft_a->res[i] = res[0];
      draft_b->res[i] = res[1];
      free(res);
    }
  }

  (*function)(plan_repair(draft_a, prob), function_data);
  (*function)(plan_repair(draft_b, prob), function_data);
  plan_free(draft_a);
  plan_free(draft_b);
}
