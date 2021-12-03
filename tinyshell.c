#include "tinyshell.h"

/* static functions decleration */
static int isvalid(char *id);

void eval(char *cmdline)
{
    char *argv[MAXARGS];
    int bg = parseline(cmdline, argv);
    sigset_t mask_all, prev_mask;
    pid_t pid;

    Sigfillset(&mask_all);
    if (!builtin_cmd(argv))
    {
        if ((pid = Fork()) == 0)
        {
            Setpgid(0, 0);
            Execve(argv[0], argv, environ);
        }
        Sigprocmask(SIG_BLOCK, &mask_all, &prev_mask);
        if (bg)
        {
            addjob(jobs, pid, BG, cmdline);
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
        }
        else
        {
            addjob(jobs, pid, FG, cmdline);
        }
        Sigprocmask(SIG_SETMASK, &prev_mask, NULL);

        if (!bg)
            waitfg(pid);
    }
}

int parseline(const char *cmdline, char **argv)
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf) - 1] = ' ';   /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
        buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'')
    {
        buf++;
        delim = strchr(buf, '\'');
    }
    else
    {
        delim = strchr(buf, ' ');
    }

    while (delim)
    {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* ignore spaces */
            buf++;

        if (*buf == '\'')
        {
            buf++;
            delim = strchr(buf, '\'');
        }
        else
        {
            delim = strchr(buf, ' ');
        }
    }
    argv[argc] = NULL;

    if (argc == 0) /* ignore blank line */
        return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc - 1] == '&')) != 0)
    {
        argv[--argc] = NULL;
    }
    return bg;
}

int builtin_cmd(char **argv)
{
    int builtin_cmd = 1;
    if (strcmp(argv[0], "quit") == 0)
        exit(0);
    else if (strcmp(argv[0], "jobs") == 0)
        listjobs(jobs);
    else if (strcmp(argv[0], "bg") == 0)
        do_bgfg(argv);
    else if (strcmp(argv[0], "fg") == 0)
        do_bgfg(argv);
    else
        builtin_cmd = 0;

    return builtin_cmd;
}

void do_bgfg(char **argv)
{
    int jid;
    pid_t pid;
    char *id = argv[1];
    job_t *job;
    sigset_t mask_all, prev_mask;

    Sigfillset(&mask_all);
    if (id == NULL)
    {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }

    if (!isvalid(id))
    {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }

    Sigprocmask(SIG_SETMASK, &mask_all, &prev_mask);
    if (id[0] == '%')
    {
        jid = atoi(id + 1);
        job = getjobjid(jobs, jid);
        if (!job)
        {
            printf("(%s): No such job\n", id + 1);
            Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            return;
        }
        pid = job->pid;
    }
    else
    {
        pid = atoi(id);
        job = getjobpid(jobs, pid);
        if (!job)
        {
            printf("(%s): No such job\n", id);
            Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            return;
        }
        jid = pid2jid(pid);
    }

    if (strcmp(argv[0], "bg") == 0)
    {
        Kill(-(job->pid), SIGCONT);
        job->state = BG;
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    }
    else
    {
        job->state = FG;
        Kill(-(job->pid), SIGCONT);
        Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
        waitfg(job->pid);
    }
    Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
}

void waitfg(pid_t pid)
{
    sigset_t mask_chld, prev_mask;

    Sigemptyset(&mask_chld);
    Sigaddset(&mask_chld, SIGCHLD);
    Sigprocmask(SIG_BLOCK, &mask_chld, &prev_mask);

    while (fgpid(jobs) == pid)
        Sigsuspend(&prev_mask);

    Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
}

/*
 *  isvalid - check if fg and bg arguments is valid
 */
int isvalid(char *id)
{
    int st = (id[0] == '%') ? 1 : 0;
    int n = strlen(id);
    for (int i = st; i < n; i++)
    {
        if (!isdigit(id[i]))
            return 0;
    }
    return 1;
}

void usage(void)
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}