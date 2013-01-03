#include <stdlib.h>

#include "main.h"
#include "plan.h"
#include "prob.h"
#include "population.h"

#define NB_GENERATIONS 1000

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

struct genetic_data {
  Population youngs;
  int mutations_left;

  int best_duration;
  ushort current;

  // Stats
  unsigned int stat_generations;
  unsigned int stat_mutations;
  unsigned int stat_crossovers;
};

void genetic_aux(Plan plan, void * vdata) {
  struct genetic_data * data = (struct genetic_data *)vdata;
  data->mutations_left -= 1;
  if (pop_insert(data->youngs, plan) == OK) {
    if (plan_duration(plan) < data->best_duration)
      debug("mutation of %d\n", data->current);
    data->stat_mutations += 1;
  }
}


Plan sch_genetic(Prob prob)  {

  Population youngs  = pop_create();
  Population matures = pop_create();
  Population buffer  = pop_create();

  ushort nb_job = prob_job_count(prob);
  ushort nb_res = prob_res_count(prob);

  struct genetic_data data;
  data.stat_generations = 0;
  data.stat_mutations   = 0;
  data.stat_crossovers  = 0;

  pop_append(youngs, sch_greedy(prob));
  for (int i=0; i < POP_SIZE; i++) {
    Plan p = sch_random(prob);
    pop_insert(youngs, p);
  }

  for (; (pop_size(youngs) > 0) && (data.stat_generations < NB_GENERATIONS); data.stat_generations++) {
    debug("%03d %4d %4d ", data.stat_generations, pop_size(matures), pop_size(youngs));
    
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
    debug("%4d %4d - %4d - ", id_m, id_y, plan_duration(pop_get(buffer,0)));
    data.mutations_left = (1 * POP_SIZE) + ((1 * id_m * id_m) / POP_SIZE) - (2 * id_m);

    for (; id_m < pop_size(matures); id_m++) plan_free(pop_get(matures, id_m));
    pop_reset(matures);
    for (; id_y < pop_size(youngs);  id_y++) plan_free(pop_get(youngs,  id_y));
    pop_reset(youngs);

    Population tmp_pop = matures;
    matures = buffer;
    buffer  = tmp_pop;

    // Generate youngs from matures 

    max_m = pop_size(matures);
    data.best_duration = plan_duration(pop_get(matures, 0));
    int median_duration = plan_duration(pop_get(matures, max_m / 2));

    data.youngs = youngs;
//    data.mutations_left = POP_SIZE;
    int crossovers_left = POP_SIZE + (POP_SIZE / 4) - data.mutations_left;
    debug("%4d %4d\n", data.mutations_left, crossovers_left);

    long div;
    int proba;
    for (id_m = 0; (id_m < max_m) && ( (data.mutations_left > 0) || (crossovers_left > 0) ); id_m++) {
      Plan plan_m = pop_get(matures, id_m);
      int duration_m = plan_duration(plan_m);

      data.current = id_m;
//      printf("  %4d %4d %4d ", data.mutations_left, crossovers_left, duration_m);

      // Mutations
      if (data.mutations_left > 0) {
        div = (max_m - id_m) * 7 * nb_job * duration_m;
        if (9 * data.mutations_left * median_duration >= div)
          proba = RAND_MAX;
        else
          proba = (RAND_MAX / (int)(div / median_duration)) * 9 * data.mutations_left ;
//        printf("  m %14ld %10d ", div, proba);
        int dice = rand();
        if (dice <= proba) {
          if ( dice < (proba * (nb_res - 8)) / (8 * (nb_res - 1)) ) {
//            if (dice < proba / 8)
//              plan_neighbourhood(plan_m, prob, &genetic_aux, (void *)&data);
//            else
              plan_neighbourhood_worse(plan_m, prob, &genetic_aux, (void *)&data);
          }
          else
            plan_neighbourhood_one(plan_m, (ushort)(rand() % prob_res_count(prob)), prob, &genetic_aux, (void *)&data);
        }
      }
//      else
//        printf("  m -                         ");

      // Crossover
      if (crossovers_left > 0) {
        div = (long)(max_m - id_m) * (long)(max_m - (1 + id_m)) * (long)duration_m;
        if (crossovers_left * median_duration >= div)
          proba = rand();
        else
          proba = (RAND_MAX / (int)(div / median_duration)) * crossovers_left;
//        printf("c %14ld %10d\n", div, proba);
        for (int i = id_m + 1; i < max_m; i++) {
          int dice = rand();
          if (dice <= proba) {
            crossovers_left -= 1;
            Plan plan_y;
            if (dice < proba / 2) {
              plan_y = plan_merge_task(plan_m, pop_get(matures,i), prob);
              if (pop_insert(youngs, plan_y) == OK) {
                if (plan_duration(plan_y) < data.best_duration)
                  debug("crossover task of %d and %d\n", id_m, i);
                data.stat_crossovers += 1;
              }
            }
            else {
              plan_y = plan_merge_res(plan_m, pop_get(matures,i), prob);
              if (pop_insert(youngs, plan_y) == OK) {
                if (plan_duration(plan_y) < data.best_duration)
                  debug("crossover res of %d and %d\n", id_m, i);
                data.stat_crossovers += 1;
              }
            }
          }
        } 
      } // if crossover
//      else 
//        printf("c -\n");

    } // for
//    printf("- %4d %4d %4d %4d\n", id_m, max_m, data.mutations_left, crossovers_left);
  }

  Plan ret = pop_get(matures, 0);
  ushort max_m = pop_size(matures);
  for (int id_m = 1; id_m < max_m; id_m++) plan_free(pop_get(matures, id_m));
  pop_free(matures);
  pop_free(youngs);
  info("%7d mutations %7d crossovers",
      data.stat_mutations, data.stat_crossovers);
  return ret;
}
