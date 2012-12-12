#include "main.h"
#include "plan.h"
#include "prob.h"

extern Plan sch_random(Prob prob);

void local_aux(Plan plan, void * data) {
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
  Plan plan = sch_random(prob);
  Plan data;

  ushort count = 0;
  while (1) {
    count += 1;
    data = NULL;
    plan_neighbourhood(plan, prob, &local_aux, &data);
    if ((data == NULL) || (plan_duration(plan) <= plan_duration(data))) {
      if (data != NULL) plan_free(data);
      printf("%d iterations\n", count);
      return plan;
    }
    plan_free(plan);
    plan = data;
  }

}
