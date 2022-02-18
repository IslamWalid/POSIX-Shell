#ifndef WRAPPER_H
#define WRAPPER_H

#include <stdlib.h>

/* error handling functions */
void unix_error(char *msg);
void app_error(char *msg);

/* Process control wrappers */
pid_t Fork(void);
void Execve(const char *filename, char *const argv[], char *const envp[]);
void Kill(pid_t pid, int signum);
void Setpgid(pid_t pid, pid_t pgid);

/* Signal wrappers */
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
void Sigemptyset(sigset_t *set);
void Sigfillset(sigset_t *set);
void Sigaddset(sigset_t *set, int signum);
int Sigsuspend(const sigset_t *set);

/* Unix I/O wrappers */
int Dup2(int fd1, int fd2);

#endif
