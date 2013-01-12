/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Heuristique gloutonne
 */

#include <limits.h>

#include "prob.h"
#include "job.h"
#include "plan.h"
#include "ressource.h"
#include "main.h"

Plan sch_greedy(Prob prob) {
  ushort nb_jobs = prob_job_count(prob);

  Plan plan = plan_create(prob);
  prob_unschedule(prob);

  while (1) {

    ushort min_start = USHRT_MAX;
    ushort max_duration = 0;
    ushort job_id = 0;

    for (int i=0; i < nb_jobs; i++) {
      Job job = prob_get_job(prob, i);
      if (!job_is_scheduled(job)) {
        ushort time = MAX(job_curop_minstart(job), res_makespan( plan_get_ressource(plan, job_curop_res(job)) ));
        if (time <= min_start) {
          ushort duration = job_remaining_duration(job);
          if ((time < min_start) || (duration > max_duration)) {
            min_start = time;
            max_duration = duration;
            job_id = i;
          }
        }
      }
    }

    if (min_start == INT_MAX) return plan;

    plan_schedule(plan, prob, job_id);
  }
}

