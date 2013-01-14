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

#define MAX_IMMOBILITY 512
#define NB_GENERATIONS (4 * MAX_IMMOBILITY)

extern Plan sch_random(Prob prob);
extern Plan sch_greedy(Prob prob);

struct genetic_data {
  Population youngs;
  int * operations_count;
  int * operations_left;
};

void genetic_aux(Plan plan, void * vdata) {
  struct genetic_data * data = (struct genetic_data *)vdata;
  *data->operations_count += 1;
  if (pop_insert(data->youngs, plan) == OK)
    *data->operations_left -= 1;
}

static inline Plan genetic(Prob prob, char with_crossover, void(*crossover_function)(Plan,Plan,Prob,process_new_plan,void*));

Plan sch_genetic(Prob prob) {
  return genetic(prob, 1, &plan_crossover_task_onepoint);
}

Plan sch_order(Prob prob) {
  return genetic(prob, 1, &plan_crossover_task_order);
}

Plan sch_mutations(Prob prob) {
  return genetic(prob, 0, NULL);
}

static inline Plan genetic(Prob prob, char with_crossover, void(*crossover_function)(Plan,Plan,Prob,process_new_plan,void*))  {

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
  int best_makespan = plan_makespan(pop_get(youngs,0));
  for (unsigned int immobility = 0; (immobility <= MAX_IMMOBILITY) && (generation < NB_GENERATIONS); generation++) {
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
      ushort makespan_m = plan_makespan(plan_m);
      ushort makespan_y = plan_makespan(plan_y);

      if (makespan_m < makespan_y) {
        pop_append(buffer, plan_m);
        id_m += 1;
      }
      else {
        pop_append(buffer, plan_y);
        id_y += 1;
        if ((makespan_m == makespan_y) && plan_equals(plan_m, plan_y)) {
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
    
    debug("- %4d - ", plan_makespan(pop_get(matures,0)));

    // Generate youngs from matures 

    max_m = pop_size(matures);
    ushort median_makespan = plan_makespan(pop_get(matures, max_m / 2));

    data.youngs = youngs;
    int mutations_left = (with_crossover ? MIN( (POP_SIZE / 16) + immobility * POP_SIZE / MAX_IMMOBILITY, POP_SIZE) : POP_SIZE);
    int crossovers_left = POP_SIZE - mutations_left;
    debug("%4d %4d\n", mutations_left, crossovers_left);

    long div;
    int proba;
    for (id_m = 0; (id_m < max_m) && ( (mutations_left > 0) || (crossovers_left > 0) ); id_m++) {
      Plan plan_m = pop_get(matures, id_m);
      ushort makespan_m = plan_makespan(plan_m);

      // Mutations
      if (mutations_left > 0) {
        div = (max_m - id_m) * 4 * (nb_job - 1) * makespan_m;
        if (5 * mutations_left * median_makespan >= div)
          proba = RAND_MAX;
        else
          proba = (RAND_MAX / (int)(div / median_makespan)) * 5 * mutations_left ;
        int dice = rand();
        if (dice <= proba) {
          data.operations_count = &stat_mutations;
          data.operations_left  = &mutations_left;
          if ( dice < (proba * (nb_res - 8)) / (8 * (nb_res - 1)) ) {
              plan_neighbourhood_worse(plan_m, prob, &genetic_aux, (void *)&data);
          }
          else
            plan_neighbourhood_one(plan_m, (ushort)(rand() % prob_res_count(prob)), prob, &genetic_aux, (void *)&data);
        }
        if (pop_size(youngs)) {
          ushort makespan_y = plan_makespan(pop_get(youngs,0));
          if (makespan_y < best_makespan) {
            debug("Mutation %d (%d)\n", id_m,makespan_y);
            best_makespan = makespan_y;
            immobility = 0;
          }
        }
      }

      // Crossover
      if (crossovers_left > 0) {
        div = (long)(max_m - id_m) * (long)(max_m - (1 + id_m)) * (long)2 * (long)makespan_m;
        if (crossovers_left * median_makespan >= div)
          proba = rand();
        else
          proba = (RAND_MAX / (int)(div / median_makespan)) * crossovers_left;
        for (int i = id_m + 1; i < max_m; i++) {
          int dice = rand();
          if (dice <= proba) {
            data.operations_count = &stat_crossovers;
            data.operations_left  = &crossovers_left;
            (*crossover_function)(plan_m, pop_get(matures,i), prob, &genetic_aux, (void *)&data);
            if (pop_size(youngs)) {
              ushort makespan_y = plan_makespan(pop_get(youngs,0));
              if (makespan_y < best_makespan) {
                debug("Crossover %d %d (%d)\n", id_m, i,makespan_y);
                best_makespan = makespan_y;
                immobility = 0;
              }
            }
          }
        } 
      } // if crossover

    } // for
  }

  Plan ret = pop_get(matures, 0);
  ushort max_m = pop_size(matures);
  for (int id_m = 1; id_m < max_m; id_m++) plan_free(pop_get(matures, id_m));
  pop_free(matures);
  pop_free(youngs);
  info("%7d mutations", stat_mutations);
  if (with_crossover) info(" %7d crossovers", stat_crossovers);
  return ret;
}
