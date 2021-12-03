#include "sighandler.h"

void sigchld_handler(int sig)
{
    int old_errno = errno;
    pid_t pid;
    int status, jid;
    sigset_t mask_all, prev_mask;

    Sigfillset(&mask_all);
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        Sigprocmask(SIG_SETMASK, &mask_all, &prev_mask);

        jid = pid2jid(pid);
        if (WIFEXITED(status))
        {
            deletejob(jobs, pid);
        }
        else if (WIFSIGNALED(status))
        {
            deletejob(jobs, pid);
            printf("Job [%d] (%d) terminated by signal %d\n", jid, pid, WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
            job_t *job = getjobpid(jobs, pid);
            job->state = ST;
            printf("Job [%d] (%d) stopped by signal %d\n", jid, pid, WSTOPSIG(status));
        }
        Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    }
    errno = old_errno;
}

void sigint_handler(int sig)
{
    int old_errno = errno;
    sigset_t mask_all, prev_mask;
    pid_t pid;

    Sigfillset(&mask_all);
    Sigprocmask(SIG_SETMASK, &mask_all, &prev_mask);
    pid = fgpid(jobs);
    Kill(-pid, SIGINT);
    Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    errno = old_errno;
}

void sigtstp_handler(int sig)
{
    int old_errno = errno;
    sigset_t mask_all, prev_mask;
    pid_t pid;

    Sigfillset(&mask_all);
    Sigprocmask(SIG_SETMASK, &mask_all, &prev_mask);
    pid = fgpid(jobs);
    Kill(-pid, SIGTSTP);
    Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    errno = old_errno;
}

void sigquit_handler(int sig)
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}