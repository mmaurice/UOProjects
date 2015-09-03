#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 4096  /* Max line length */

int
main(void)
{
	int		n;
	int		fd[2];
	pid_t	pid;
	char	line[MAXLINE];

	if (pipe(fd) < 0) {
		perror("Pipe error!");
		exit(-1);
        }
	if ((pid = fork()) < 0) {
		perror("Fork error!");
                exit(-1);
	} else if (pid > 0) {		/* parent */
		close(fd[0]);
		write(fd[1], "Hello, child!\n", 12);
	} else {					/* child */
		close(fd[1]);
		n = read(fd[0], line, MAXLINE);
		write(STDOUT_FILENO, line, n);
	}
	exit(0);
}
