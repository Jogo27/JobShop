/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Structure de données pour les instances du problème
 */

#ifndef PROB_H
#define PROB_H

#include <stdio.h>

#include "main.h"
#include "job.h"

struct prob;
typedef struct prob * Prob;



/* Constructors Desctructors */

// Create a prob from a stream
extern Prob prob_parse (FILE* stream);

// Desallocate the prob and its jobs
extern result prob_free (Prob prob);



/* Getters and Setters */

// Get the number of jobs
extern ushort prob_job_count (Prob prob);

// Get the number of ressources
extern ushort prob_res_count (Prob prob);

// Retrieve a given job
extern Job prob_get_job (Prob prob, int i);



/* Scheduling */

/* Prob keep a track of whether all it's operations have been scheduled.
 * See: job.h
 */

// Wheter all the job are scheduled
extern ushort prob_is_scheduled (Prob prob);

// Mark all jobs as unscheduled
extern result prob_unschedule (Prob prob);


#endif
