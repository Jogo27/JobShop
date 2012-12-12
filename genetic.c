#include "main.h"
#include "plan.h"
#include "prob.h"

#define NB_INIT_RANDOM

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

void aux(Plan plan, void * data) {
  pop_insert((Population)data, plan);
}

void grew(Plan mature, Population olds, Population youngs, Prob prob) {
  plan_neighbourhood(mature, prob, &aux, (void *)youngs);
  ushort max = pop_size(olds) - 1;
  for (int i=0; i < max; i++) pop_insert(youngs, plan_merge(mature, pop_get(olds,i)));
}

Plan sch_genetic(Prob prob)  {

  Population youngs  = pop_create();
  Population matures = pop_create();
  Population olds    = pop_create();

  pop_apppend(youngs, sch_greedy(prob));
  for (int i=0; i < NB_INIT_RANDOM; i++) {
    Plan p = sch_random(prob);
    pop_insert(youngs, p);
  }

  ushort id_m, id_o;
  ushort size_m;
  Plan   mature;
  while (pop_size(youngs) > 0) {

    // Youngs become mature
    Population buffer = matures;
    matures = youngs;
    youngs  = buffer;

    // Matures merge, derive and become old
    id_o = 0;
    id_m = 0;
    size_m = pop_size(matures);
    while ((id_m < size_m) && (id_o < pop_size(olds))) {
      mature = pop_get(matures, id_m);
      int duration_o = plan_duration(pop_get(olds, id_o));
      int duration_m = plan_duration(mature);

      if (duration_o < duration_m) {
        id_o += 1;
      }
      else if ((duration_o == duration_m) && plan_equals(pop_get(olds, id_o), mature)) {
        plan_free(mature);
        id_m += 1;
      }
      else {
        grew(mature, olds, youngs);
        pop_insert_at(olds, mature, id_o);
        id_m += 1;
        id_o += 1;
      }
    }
    while ((id_m < size_m) && (id_o < POP_SIZE)) {
      mature = pop_get(matures, id_m);
      grew(mature, olds, youngs);
      pop_append(olds, mature);
      id_m += 1;
      id_o += 1;
    }
    for (; id_m < size_m; id_m++) plan_free(pop_get(matures, id_m));
    pop_reset(matures);

  }


  Plan ret = pop_get(olds, 0);
  ushort size_o = pop_size(olds);
  for (int i=1; i < size_o; i++) plan_free(pop_get(olds, i));
  pop_free(olds);
  pop_free(matures);
  pop_free(youngs);
  return ret;
}
