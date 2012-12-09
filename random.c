#include <stdlib.h>

#include "prob.h"
#include "job.h"
#include "plan.h"

Plan sch_random(Prob prob) {
  srand((unsigned int)(prob));

  ushort nb_jobs = prob_job_count(prob);

  Plan plan = plan_create(prob);
  prob_unschedule(prob);

  while (1) {

    ushort imin = 0;
    while (job_is_scheduled( prob_get_job(prob, imin) )) {
      imin++;
      if (imin >= nb_jobs) return plan;
    }

    ushort imax = nb_jobs - 1;
    while (job_is_scheduled( prob_get_job(prob, imax) ))
      imax--;

    ushort job_id;
    do { job_id = (ushort)( (rand() % (imax + 1 - imin)) + imin ); } while (job_is_scheduled( prob_get_job(prob, job_id) ));
    plan_schedule(plan, prob, job_id);
  }
}

