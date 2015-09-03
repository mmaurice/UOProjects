/*
Muhammed Maurice
951275217
Prof. Malony
CIS 415
I found the Piazza posts of Sam Nelson and Lucas Crawford to be very useful
*/


#include <stdio.h>
#include <string.h>
#include <wordexp.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

#define BUFSIZE 100

void sigalrm_handler(int);


int main (int argc, char *argv[]) {
	char buff[BUFSIZE];
	FILE* fp;
	fp=fopen(argv[1], "r");

	int numPrograms = 0;
	wordexp_t p;
	wordexp_t lines[BUFSIZE];
	int k, i;
	pid_t pid[BUFSIZE];
	int status;
	sigset_t sigSet;
	int sig;
	struct sigaction action;
	int vmSize = 11;

	while (fgets(buff, BUFSIZE-1, fp) != NULL) {
		k = strlen(buff)-1;
		if(buff[k] == '\n')
			buff[k] = '\0';

		wordexp(buff, &p, 0);
		lines[numPrograms] = p;
		numPrograms++;
	}
	sigemptyset(&sigSet);
	sigaddset(&sigSet, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigSet, NULL);

	for (i = 0; i <numPrograms; i++){
		pid[i] = fork();
		if (!pid[i]) {

			//printf("%s\n", "child");
			kill(pid[i], SIGUSR1);
			sigwait(&sigSet, &sig);
			action.sa_flags = 0;
			action.sa_handler = &sigalrm_handler;
			sigaction(SIGALRM, &action, NULL);
			alarm(1);
			pause();

			char infobuff[BUFSIZE];
			char buf[12];
			int count = 0;
			int curPid = getpid(); 
			sprintf(buf, "/proc/%d/status", curPid);
			FILE* infofp = fopen(buf, "r");
			printf("Pid: %d\n", curPid);
			printf("Parent PID: %d\n", getppid());
			while(fgets(infobuff, BUFSIZE-1, infofp) != NULL){
				if ( count == 11)
					printf("%s", infobuff);
				count++;
			}

			char schedbuff[BUFSIZE];
			char bufsched[12];
			int countS = 0;
			sprintf(bufsched, "/proc/%d/sched", curPid);
			FILE* schedfp = fopen(bufsched, "r");
			while(fgets(schedbuff, BUFSIZE-1, schedfp) != NULL){
				if ( countS == 2)
					printf("%s", schedbuff);
				countS++;
			}

			char iobuff[BUFSIZE];
			char bufio[12];
			int countio = 0;
			sprintf(bufio, "/proc/%d/io", curPid);
			FILE* iofp = fopen(bufio, "r");
			while(fgets(iobuff, BUFSIZE-1, iofp) != NULL){
				if ( countio == 0)
					printf("%s", iobuff);
				if ( countio == 1)
					printf("%s", iobuff);
				if ( countio == 4)
					printf("%s", iobuff);
				if ( countio == 5)
					printf("%s", iobuff);
				countio++;
			}

			execvp(lines[i].we_wordv[0], lines[i].we_wordv);
		}
		
	}
	for (i = 0; i<numPrograms; i++) {
		//printf("%s\n", "parent");
		waitpid(pid[i], &status, WUNTRACED | WCONTINUED);
	}

	
	wordfree(&p);
	wordfree(lines);
	fclose(fp);

	return 0;
}

void sigalrm_handler(int sig)
{

}
