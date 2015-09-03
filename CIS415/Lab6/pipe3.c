#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 4096  /* Max line length */

int
main(void)
{
	int		n;
	int		fd1[2];
        int		fd2[2];
	pid_t	pid;
	char	line[MAXLINE];

	if (pipe(fd1) < 0) {
		perror("Pipe error!");
		exit(-1);
        }
 
	if (pipe(fd2) < 0) {
		perror("Pipe error!");
		exit(-1);
        }
	if ((pid = fork()) < 0) {
		perror("Fork error!");
                exit(-1);
	} else if (pid > 0) {		/* parent */
		close(fd1[0]);
                close(fd2[1]);
		write(fd1[1], "Hello, child!\n", 12);

		n = read(fd2[0], line, MAXLINE);
                //char tmp[MAXLINE] = "I'm your parent";
                strcat(line, "I'm your parent");
                n = strlen(line);
		write(STDOUT_FILENO, line, n);
	} else {					/* child */
		close(fd1[1]);
                close(fd2[0]);
		n = read(fd1[0], line, MAXLINE);
		write(fd2[1], line, n);
	}
	exit(0);
}
