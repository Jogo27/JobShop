#include "main.h"
#include "plan.h"
#include "prob.h"
#include "population.h"

#define NB_INIT_RANDOM 9

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

struct genetic_aux {
  int        max_duration;
  Population youngs;
};

void genetic_aux(Plan plan, void * vdata) {
  struct genetic_aux * data = (struct genetic_aux *)vdata;
  if ( (data->max_duration < 0) ||
       (plan_duration(plan) <= data->max_duration)
     ) pop_insert(data->youngs, plan);
  else
    plan_free(plan);
}

void grew(Plan mature, Population olds, Population youngs, Prob prob) {
  ushort max = pop_size(olds);

  struct genetic_aux data;
  data.max_duration = (max == 0 ? -1 : plan_duration(pop_get(olds,max-1)));
  data.youngs = youngs;
  plan_neighbourhood(mature, prob, &genetic_aux, (void *)&data);

  if (max > 0) max -= 1;
  for (int i=0; i < max; i++) pop_insert(youngs, plan_merge(mature, pop_get(olds,i)));
}

Plan sch_genetic(Prob prob)  {

  Population youngs  = pop_create();
  Population matures = pop_create();
  Population olds    = pop_create();

  pop_append(youngs, sch_greedy(prob));
  for (int i=0; i < NB_INIT_RANDOM; i++) {
    Plan p = sch_random(prob);
    pop_insert(youngs, p);
  }

  ushort id_m, id_o;
  ushort size_m;
  Plan   mature;
  while (pop_size(youngs) > 0) {
    printf("%d %d\n", pop_size(olds), pop_size(youngs));

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
      else if (duration_o == duration_m) {
        if (plan_equals(pop_get(olds, id_o), mature)) {
          plan_free(mature);
          id_m += 1;
        }
        else
          id_o += 1;
      }
      else {
        grew(mature, olds, youngs, prob);
        pop_insert_at(olds, mature, id_o);
        id_m += 1;
        id_o += 1;
      }
    }
    while ((id_m < size_m) && (id_o < POP_SIZE)) {
      mature = pop_get(matures, id_m);
      grew(mature, olds, youngs, prob);
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
