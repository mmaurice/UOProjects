#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

struct pcb {
    pid_t pid;
    int   signo;

    struct pcb *next;
    struct pcb *prev;
};

typedef struct pcb *pcb_t;

struct jobs {
    size_t count;
    pcb_t  active;
};

typedef struct jobs *jobs_t;

int go = 0;
int schedule = 0;

jobs_t
jobs_new(void)
{
    jobs_t jobs;

    jobs = malloc(sizeof(*jobs));
    jobs->count  = 0;
    jobs->active = NULL;

    return jobs;
}

void
jobs_add(jobs_t jobs, pcb_t pcb)
{
    if (jobs->active == NULL) {
	jobs->active = pcb;

	pcb->prev = pcb;
	pcb->next = pcb;
    } else {
	pcb->prev = jobs->active->prev;
	pcb->next = jobs->active;

	pcb->prev->next = pcb;
	pcb->next->prev = pcb;
    }

    jobs->count++;
}

void
jobs_del(jobs_t jobs, pcb_t pcb)
{
    if (pcb->prev == pcb) {
	jobs->active = NULL;
    } else {
	pcb->prev->next = pcb->next;
	pcb->next->prev = pcb->prev;

	if (jobs->active == pcb) {
	    jobs->active = pcb->next;
	}
    }

    jobs->count--;
}

void
jobs_del_pid(jobs_t jobs, pid_t pid)
{
    int i;
    pcb_t pcb = jobs->active;

    for (i=0; i<jobs->count; i++) {
	if (pcb->pid == pid) {
	    jobs_del(jobs, pcb);
	    free(pcb);
	    break;
	} else {
	    pcb = pcb->next;
	}
    }
}

void
jobs_free(jobs_t jobs)
{
    int i;
    pcb_t pcb;

    for (i=0; i<jobs->count; i++) {
	pcb = jobs->active;
	jobs->active = jobs->active->next;
	jobs_del(jobs, pcb);

	free(pcb);
    }

    free(jobs);
}

void
sigusr1_handler(int signo)
{
    go = 1;
}

void
sigalrm_handler(int signo)
{
    schedule = 1;
    alarm(1);
}

pcb_t
launch_program(char *cmdline, size_t len)
{
    int rv;
    pid_t pid;
    pcb_t pcb;
    wordexp_t p;
    char **w;

    rv = wordexp(cmdline, &p, WRDE_NOCMD);
    if (rv != 0) {
	return NULL;
    }
    w = p.we_wordv;

    pid = fork();

    if (pid == 0) {		/* child */
	while (!go) sleep(1);

	if (execvp(w[0], w) < 0) {
	    perror("execvp");
	    exit(1);
	}
    } else {			/* parent */
	pcb = malloc(sizeof(*pcb));
	pcb->pid   = pid;
	pcb->signo = SIGUSR1;
	pcb->next  = pcb;
	pcb->prev  = pcb;
    }

    return pcb;
}

jobs_t
launch_programs(const char *script)
{
    FILE   *fp;
    char   *line;
    size_t  len;
    ssize_t read;
    jobs_t  jobs;
    pcb_t   pcb;

    jobs = jobs_new();
 
    fp = fopen(script, "r");
    if (fp == NULL) {
	perror("fopen");
	return NULL;
    }

    while (1) {
	line = NULL;
	len  = 0;

	read = getline(&line, &len, fp);
	if (read == -1) {
	    break;
	}

	line[read-1] = '\0';
	pcb = launch_program(line, len);
	free(line);

	if (pcb != NULL) {
	    jobs_add(jobs, pcb);
	}
    }

    fclose(fp);

    return jobs;
}

void
harvest(jobs_t jobs)
{
    int status;
    pid_t pid;

    while (1) {
	pid = waitpid(-1, &status, WNOHANG);
	if (pid <= 0) {
	    break;
	}

	jobs_del_pid(jobs, pid);
    }
}

void
scheduler(jobs_t jobs)
{
    if (!schedule) {
	return;
    }

    schedule = 0;

    /* stop current active process */
    kill(jobs->active->pid, jobs->active->signo);
    jobs->active->signo = SIGCONT;

    /* resume next process */
    jobs->active = jobs->active->next;
    kill(jobs->active->pid, jobs->active->signo);
    jobs->active->signo = SIGSTOP;
}

int
main(int argc, char **argv)
{
    int status;
    pid_t pid;
    jobs_t jobs;
    struct sigaction act;

    if (argc != 2) {
	fprintf(stderr, "Usage: mcp <script>\n");
	return 0;
    }

    sigemptyset(&act.sa_mask);
    act.sa_flags   = 0;

    act.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_handler = sigalrm_handler;
    sigaction(SIGALRM, &act, NULL);

    jobs = launch_programs(argv[1]);

    alarm(1);

    kill(jobs->active->pid, jobs->active->signo);
    jobs->active->signo = SIGSTOP;

    while (jobs->count > 0) {
	pid = wait(&status);

	if (pid == -1) {
	    if (errno == EINTR) {
		scheduler(jobs);
	    } else {
		perror("wait");
	    }
	} else {
	    jobs_del_pid(jobs, pid);
	}
    }

    jobs_free(jobs);

    return 0;
}
