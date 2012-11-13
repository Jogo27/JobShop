#ifndef JOB_H
#define JOB_H

typedef struct {
  unsigned short res;
  unsigned short duration;
} Task;

struct job;
typedef struct job * Job;

extern Job job_new(int start, unsigned short initial_size);
extern int job_add_task(Job job, unsigned short res, unsigned short duration);

extern unsigned short job_cur_position(Job job);
extern int            job_start(Job job);
extern unsigned short job_cur_res(Job job);
extern unsigned short job_cur_duration(Job job);
extern int            job_next_task(Job job);

extern int job_free(Job job);

#endif
