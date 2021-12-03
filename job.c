#include "job.h"

job_t jobs[MAXJOBS];    /* The job list */
static int nextjid = 1; /* next job ID to allocate */
extern int verbose;     /* defined in tsh.c */

void clearjob(job_t *job)
{
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

void initjobs(job_t *jobs)
{
    for (int i = 0; i < MAXJOBS; i++)
        clearjob(&jobs[i]);
}

int maxjid(job_t *jobs)
{
    int max = 0;

    for (int i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid > max)
            max = jobs[i].jid;
    return max;
}

int addjob(job_t *jobs, pid_t pid, int state, char *cmdline)
{
    if (pid < 1)
        return 0;

    for (int i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid == 0)
        {
            jobs[i].pid = pid;
            jobs[i].state = state;
            jobs[i].jid = nextjid++;
            if (nextjid > MAXJOBS)
                nextjid = 1;
            strcpy(jobs[i].cmdline, cmdline);
            if (verbose)
            {
                printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
        }
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

int deletejob(job_t *jobs, pid_t pid)
{
    if (pid < 1)
        return 0;

    for (int i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid == pid)
        {
            clearjob(&jobs[i]);
            nextjid = maxjid(jobs) + 1;
            return 1;
        }
    }
    return 0;
}

pid_t fgpid(job_t *jobs)
{
    for (int i = 0; i < MAXJOBS; i++)
        if (jobs[i].state == FG)
            return jobs[i].pid;
    return 0;
}

job_t *getjobpid(job_t *jobs, pid_t pid)
{
    if (pid < 1)
        return NULL;
    for (int i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid)
            return &jobs[i];
    return NULL;
}

job_t *getjobjid(job_t *jobs, int jid)
{
    if (jid < 1)
        return NULL;
    for (int i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid == jid)
            return &jobs[i];
    return NULL;
}

int pid2jid(pid_t pid)
{
    if (pid < 1)
        return 0;
    for (int i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid)
        {
            return jobs[i].jid;
        }
    return 0;
}

void listjobs(job_t *jobs)
{
    for (int i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid != 0)
        {
            printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
            switch (jobs[i].state)
            {
            case BG:
                printf("Running ");
                break;
            case FG:
                printf("Foreground ");
                break;
            case ST:
                printf("Stopped ");
                break;
            default:
                printf("listjobs: Internal error: job[%d].state=%d ",
                       i, jobs[i].state);
            }
            printf("%s", jobs[i].cmdline);
        }
    }
}
