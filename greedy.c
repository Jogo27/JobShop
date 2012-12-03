#include <limits.h>

#include "prob.h"
#include "job.h"
#include "plan.h"
#include "ressource.h"
#include "main.h"

Plan sch_greedy(Prob prob) {
  ushort nb_jobs = prob_job_count(prob);

  Plan plan = plan_create( prob_res_count(prob), nb_jobs );
  prob_unschedule(prob);

  while (1) {

    int min_start = INT_MAX;
    int max_duration = 0;
    ushort job_id = 0;

    for (int i=0; i < nb_jobs; i++) {
      Job job = prob_get_job(prob, i);
      if (!job_is_scheduled(job)) {
        int time = MAX(job_curop_minstart(job), res_duration( plan_get_ressource(plan, job_curop_res(job)) ));
        if (time <= min_start) {
          int duration = job_remaining_duration(job);
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

