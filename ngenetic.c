#include <stdlib.h>

#include "main.h"
#include "plan.h"
#include "prob.h"
#include "population.h"

#define NB_INIT_RANDOM 1023
#define NB_GENERATIONS 300

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

struct genetic_data {
  Population youngs;

  // Stats
  unsigned int stat_generations;
  unsigned int stat_mutations;
  unsigned int stat_crossovers;
};

void genetic_aux(Plan plan, void * vdata) {
  struct genetic_data * data = (struct genetic_data *)vdata;
  if (pop_insert(data->youngs, plan) == OK)
    data->stat_mutations += 1;
}


Plan sch_genetic(Prob prob)  {

  Population youngs  = pop_create();
  Population matures = pop_create();
  Population buffer  = pop_create();

  struct genetic_data data;
  data.stat_generations = 0;
  data.stat_mutations   = 0;
  data.stat_crossovers  = 0;

  pop_append(youngs, sch_greedy(prob));
  for (int i=0; i < NB_INIT_RANDOM; i++) {
    Plan p = sch_random(prob);
    pop_insert(youngs, p);
  }

  for (; (pop_size(youngs) > 0) && (data.stat_generations < NB_GENERATIONS); data.stat_generations++) {
    printf("%03d %4d %4d ", data.stat_generations, pop_size(matures), pop_size(youngs));
    
    // Merge youngs and matures into matures

    ushort id_m = 0;
    ushort id_y = 0;
    ushort max_m = MIN(POP_SIZE / 2, pop_size(matures));
    ushort max_y = pop_size(youngs);

    while ( (id_y < max_y) && (pop_size(buffer) < POP_SIZE)  &&
            ( (id_m < max_m) || ((id_m < pop_size(matures)) && ((max_y - id_y) < (POP_SIZE - pop_size(buffer)))) )
          ) {
      Plan plan_m = pop_get(matures, id_m);
      Plan plan_y = pop_get(youngs,  id_y);
      int duration_m = plan_duration(plan_m);
      int duration_y = plan_duration(plan_y);

      if (duration_m < duration_y) {
        pop_append(buffer, plan_m);
        id_m += 1;
      }
      else {
        pop_append(buffer, plan_y);
        id_y += 1;
        if ((duration_m == duration_y) && plan_equals(plan_m, plan_y)) {
          plan_free(plan_m);
          id_m += 1;
        }
      }
    }

    if (pop_size(buffer) < POP_SIZE) {
      if (id_y <max_y)
        id_y += pop_copy_append(buffer, youngs, id_y);
      else
        id_m += pop_copy_append(buffer, matures, id_m);
    }
    printf("%4d %4d %4d ", id_m, id_y, plan_duration(pop_get(buffer,0)));

    for (; id_m < pop_size(matures); id_m++) plan_free(pop_get(matures, id_m));
    pop_reset(matures);
    for (; id_y < pop_size(youngs);  id_y++) plan_free(pop_get(youngs,  id_y));
    pop_reset(youngs);

    Population tmp_pop = matures;
    matures = buffer;
    buffer  = tmp_pop;

    // Generate youngs from matures 
    
    data.youngs = youngs;
    max_m = pop_size(matures);
    int proba_mutation = ((((RAND_MAX / prob_job_count(prob)) / max_m) / 5) * 3) * POP_SIZE;
    int proba_crossover = (((RAND_MAX / max_m) * 3) / (max_m - 1)) * POP_SIZE;
    printf("%d %d\n", proba_mutation, proba_crossover);

    for (id_m = 0; id_m < max_m; id_m++) {
      Plan plan_m = pop_get(matures, id_m);

      // Mutations
      int dice = rand();
      if (dice <= proba_mutation) {
        if (dice < proba_mutation / 2) {
          if (dice < proba_mutation / 8)
            plan_neighbourhood(plan_m, prob, &genetic_aux, (void *)&data);
          else
            plan_neighbourhood_worse(plan_m, prob, &genetic_aux, (void *)&data);
        }
        else
          plan_neighbourhood_one(plan_m, (ushort)(rand() % prob_res_count(prob)), prob, &genetic_aux, (void *)&data);
      }

      // Crossover
      for (int i = id_m + 1; i < max_m; i++) {
        dice = rand();
        if (dice <= proba_crossover) {
          if (dice < proba_crossover / 2) {
            if (pop_insert(youngs, plan_merge_task(plan_m, pop_get(matures,i), prob)) == OK)
              data.stat_crossovers += 1;
          }
          else
            if (pop_insert(youngs, plan_merge_res(plan_m, pop_get(matures,i), prob)) == OK)
              data.stat_crossovers += 1;
        }
      }
    }
  }

  Plan ret = pop_get(matures, 0);
  ushort max_m = pop_size(matures);
  for (int id_m = 1; id_m < max_m; id_m++) plan_free(pop_get(matures, id_m));
  pop_free(matures);
  pop_free(youngs);
  printf("%d generations, %d mutations, %d crossovers\n",
      data.stat_generations, data.stat_mutations, data.stat_crossovers);
  return ret;
}
