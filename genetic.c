#include "main.h"
#include "plan.h"
#include "prob.h"

#define NB_INIT_RANDOM

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

Plan sch_genetic(Prob prob)  {

  Population youngs  = pop_create();
  Population matures = pop_create();
  Population olds    = pop_create();

  pop_apppend(youngs, sch_greedy(prob));
  for (int i=0; i < NB_INIT_RANDOM; i++) {
    Plan p = sch_random(prob);
    pop_insert(youngs, p);
  }

  ushort id_y, id_m, id_o;
  ushort size_y, size_m, size_o;
  while (pop_size(youngs) > 0) {

    // Youngs become mature

    id_y = 0;
    id_o = 0;
    size_y = pop_size(youngs);
    size_o = pop_size(olds);
    while ((id_o < size_o) && (id_y < size_y)) {
      int duration_o = plan_duration(pop_get(olds,  id_o));
      int duration_y = plan_duration(pop_get(youngs,id_y));

      if (duration_o < duration_y) {
        id_o += 1;
      }
      else if ((duration_o == duration_y) && plan_equals(pop_get(olds, id_o), pop_get(youngs, id_y))) {
        plan_free(pop_get(youngs, id_y));
        id_y += 1;
      }
      else {
        pop_append(matures, pop_get(youngs, id_y));
        id_y += 1;
      }
    }
    for (int i=0; i < (POP_SIZE - size_o); i++) {
      pop_append(matures, pop_get(youngs, id_y));
      id_y += 1;
    }
    for (; id_y < size_y; id_y++) plan_free(pop_get(youngs, id_y));
    pop_reset(youngs);

    // Matures merge, derive and become old


  }

