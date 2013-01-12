/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Algorithme génétique
 */

#include <stdlib.h>
#include <limits.h>

#include "main.h"
#include "plan.h"
#include "prob.h"
#include "population.h"

#define NB_GENERATIONS 500

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

struct genetic_data {
  Population youngs;
  int * operations_left;
};

void genetic_aux(Plan plan, void * vdata) {
  struct genetic_data * data = (struct genetic_data *)vdata;
  if (pop_insert(data->youngs, plan) == OK)
    *data->operations_left -= 1;
}

inline Plan genetic(Prob prob, char with_crossover);

Plan sch_genetic(Prob prob) {
  return genetic(prob, 1);
}

Plan sch_mutations(Prob prob) {
  return genetic(prob, 0);
}

static inline Plan genetic(Prob prob, char with_crossover)  {

  Population youngs  = pop_create();
  Population matures = pop_create();
  Population buffer  = pop_create();

  ushort nb_job = prob_job_count(prob);
  ushort nb_res = prob_res_count(prob);

  struct genetic_data data;
  int generation = 0;
  int stat_mutations   = 0;
  int stat_crossovers  = 0;

  pop_append(youngs, sch_greedy(prob));
  for (int i=0; i < POP_SIZE; i++) {
    Plan p = sch_random(prob);
    pop_insert(youngs, p);
  }

  ushort id_m;
  int best_makespan = plan_duration(pop_get(youngs,0));
  for (unsigned int immobility = 0; immobility < NB_GENERATIONS; generation++) {
    debug("%03d %4d %4d ", generation, pop_size(matures), pop_size(youngs));

    immobility += 1;
    
    // Merge youngs and matures into matures

    id_m = 0;
    ushort id_y = 0;
    ushort max_m = MIN(POP_SIZE / 4, pop_size(matures));
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
    debug("%4d %4d ", id_m, id_y);

    for (; id_m < pop_size(matures); id_m++) plan_free(pop_get(matures, id_m));
    pop_reset(matures);
    for (; id_y < pop_size(youngs);  id_y++) plan_free(pop_get(youngs,  id_y));
    pop_reset(youngs);

    Population tmp_pop = matures;
    matures = buffer;
    buffer  = tmp_pop;
    
    debug("- %4d - ", plan_duration(pop_get(matures,0)));

    // Generate youngs from matures 

    max_m = pop_size(matures);
    int median_duration = plan_duration(pop_get(matures, max_m / 2));

    data.youngs = youngs;
    int mutations_left = (with_crossover ? MIN( (POP_SIZE / 4) + immobility * POP_SIZE / NB_GENERATIONS, POP_SIZE) : POP_SIZE);
    int crossovers_left = POP_SIZE - mutations_left;
    debug("%4d %4d\n", mutations_left, crossovers_left);

    long div;
    int proba;
    for (id_m = 0; (id_m < max_m) && ( (mutations_left > 0) || (crossovers_left > 0) ); id_m++) {
      Plan plan_m = pop_get(matures, id_m);
      int duration_m = plan_duration(plan_m);

      // Mutations
      if (mutations_left > 0) {
        div = (max_m - id_m) * 7 * nb_job * duration_m;
        if (9 * mutations_left * median_duration >= div)
          proba = RAND_MAX;
        else
          proba = (RAND_MAX / (int)(div / median_duration)) * 9 * mutations_left ;
        int dice = rand();
        if (dice <= proba) {
          stat_mutations += 1;
          data.operations_left = &mutations_left;
          if ( dice < (proba * (nb_res - 8)) / (8 * (nb_res - 1)) ) {
              plan_neighbourhood_worse(plan_m, prob, &genetic_aux, (void *)&data);
          }
          else
            plan_neighbourhood_one(plan_m, (ushort)(rand() % prob_res_count(prob)), prob, &genetic_aux, (void *)&data);
        }
        if (pop_size(youngs)) {
          int makespan_y = plan_duration(pop_get(youngs,0));
          if (makespan_y < best_makespan) {
            debug("Mutation %d (%d)\n", id_m,makespan_y);
            best_makespan = makespan_y;
            immobility = 0;
          }
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
            stat_crossovers += 1;
            data.operations_left = &crossovers_left;
            if (dice < proba / 2)
              plan_crossover_task_onepoint(plan_m, pop_get(matures,i), prob, &genetic_aux, (void *)&data);
            else
              plan_crossover_task_order(plan_m, pop_get(matures,i), prob, &genetic_aux, (void *)&data);
            if (pop_size(youngs)) {
              int makespan_y = plan_duration(pop_get(youngs,0));
              if (makespan_y < best_makespan) {
                debug("Crossover %d %d (%d)\n", id_m, i,makespan_y);
                best_makespan = makespan_y;
                immobility = 0;
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
      stat_mutations, stat_crossovers);
  return ret;
}
