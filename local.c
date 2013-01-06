#include "main.h"
#include "plan.h"
#include "prob.h"

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

void local_aux(Plan plan, void * data) {
//  plan_output(plan, stdout);
  if (*((Plan *)data) == NULL) {
    *((Plan *)data) = plan;
  }
  else if (plan_duration(plan) < plan_duration(*((Plan *)data))) {
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




Plan sch_tabou(Prob prob) {
  Plan best = sch_greedy(prob);
  Plan current = plan_clone(best);

  ushort nb_res = prob_res_count(prob);
  ushort max_repetitions = 100 * nb_res;
  ushort res_id = 0;
  ushort count = 0;
  for (int i=0; i < max_repetitions; i++) {
    count += 1;
    Plan neighbour = NULL;
    plan_neighbourhood_one(current, res_id, prob, &local_aux, &neighbour);
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
