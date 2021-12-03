#ifndef JOB_H
#define JOB_H

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Misc manifest constants */
#define MAXLINE 1024   /* max line size */
#define MAXARGS 128    /* max args on a command line */
#define MAXJOBS 16     /* max jobs at any point in time */
#define MAXJID 1 << 16 /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

typedef struct job
{                          /* The job struct */
    pid_t pid;             /* job PID */
    int jid;               /* job ID [1, 2, ...] */
    int state;             /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE]; /* command line */
} job_t;

extern job_t jobs[MAXJOBS];

/* clearjob - Clear the entries in a job struct */
void clearjob(job_t *job);

/* initjobs - Initialize the job list */
void initjobs(job_t *jobs);

/* maxjid - Returns largest allocated job ID */
int maxjid(job_t *jobs);

/* addjob - Add a job to the job list */
int addjob(job_t *jobs, pid_t pid, int state, char *cmdline);

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(job_t *jobs, pid_t pid);

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(job_t *jobs);

/* getjobpid  - Find a job (by PID) on the job list */
job_t *getjobpid(job_t *jobs, pid_t pid);

/* getjobjid  - Find a job (by JID) on the job list */
job_t *getjobjid(job_t *jobs, int jid);

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid);

/* listjobs - Print the job list */
void listjobs(job_t *jobs);

#endif