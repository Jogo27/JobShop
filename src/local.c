/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Descente locale
 */

#include "main.h"
#include "plan.h"
#include "prob.h"

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

void local_aux(Plan plan, void * data) {
//  plan_output(plan, stdout);
//  if (plan == NULL) return;
  if (*((Plan *)data) == NULL) {
    *((Plan *)data) = plan;
  }
  else if (plan_sum_makespan(plan) < plan_sum_makespan(*((Plan *)data))) {
    plan_free(*((Plan *)data));
    *((Plan *)data) = (void*)plan;
  }
  else {
    plan_free(plan);
  }
}

Plan sch_localy(Prob prob) {
  Plan plan = sch_greedy(prob);
  Plan data;

  ushort count = 0;
  while (1) {
    count += 1;
    data = NULL;
    plan_neighbourhood(plan, prob, &local_aux, &data);
    if ((data == NULL) || (plan_duration(plan) <= plan_duration(data))) {
      if (data != NULL) plan_free(data);
      info("%3d iterations", count);
      return plan;
    }
    plan_free(plan);
    plan = data;
  }

}


//Plan sch_local_opt(Prob prob) {
//  Plan best = sch_greedy(prob);
//  Plan next;
//  int duration = plan_duration(best);
//
//  ushort max_repetitions = prob_res_count(prob) + prob_job_count(prob);
//  ushort count = 0;
//  for (int i=0; i < max_repetitions; i++) {
//    count += 1;
//    next = plan_reduce_critical_path(best, prob);
//    if (next == NULL) {
//      i = max_repetitions;
//      debug("\n");
//    }
//    else {
//      plan_free(best);
//      best = next;
//      int tmp_duration = plan_duration(best);
//      debug("makespan %d\n", tmp_duration);
//      if (tmp_duration < duration) {
//        duration = tmp_duration;
//        i = 0;
//      }
//    }
//  }
//
//  info("%3d iterations", count);
//  return best;
//}


Plan sch_local_opt(Prob prob) {
  Plan best = sch_greedy(prob);
  Plan current = plan_clone(best);

  ushort max_repetitions = prob_res_count(prob) + prob_job_count(prob);
  ushort count = 0;
  for (int i=0; i < max_repetitions; i++) {
    count += 1;
    Plan neighbour = plan_reduce_critical_path(best, prob);
    if (neighbour == NULL) {
      i = max_repetitions;
      debug("\n");
    }
    else {
      int duration = plan_duration(neighbour);
      debug("makespan %d\n", duration);
      if (plan_duration(neighbour) < plan_duration(best)) {
        plan_free(best);
        best = plan_clone(neighbour);
        i = 0;
      }
      plan_free(current);
      current = neighbour;
    }
  }

  plan_free(current);
  info("%3d iterations", count);
  return best;
}


Plan sch_tabou(Prob prob) {
  Plan best = sch_greedy(prob);
  Plan current = plan_clone(best);

  ushort nb_res = prob_res_count(prob);
  ushort max_repetitions = prob_job_count(prob) * nb_res * nb_res;
  ushort res_id = 0;
  ushort count = 0;
  for (int i=0; i < max_repetitions; i++) {
    count += 1;
    Plan neighbour = NULL;
    plan_neighbourhood_perm(current, res_id, prob, &local_aux, &neighbour);
    if (neighbour == NULL)
      i = max_repetitions;
    else {
      if (plan_duration(neighbour) < plan_duration(best)) {
        plan_free(best);
        best = plan_clone(neighbour);
        i = 0;
      }
      plan_free(current);
      current = neighbour;
    }
    res_id = (res_id + 1) % nb_res;
  }

  plan_free(current);
  info("%5d iterations", count);
  return best;
}
