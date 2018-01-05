#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
int main(int argc, char *argv[])
{
	int fd[2], exec_ret1, exec_ret2;


	pipe(fd);

	pid_t c1, c2;

	if( !(c1 = fork()) ) {
    		dup2(fd[1], 1); /* redirect standard output to pipe write end */
		close(fd[1]);
		close(fd[0]);    		
		exec_ret1 = execl("email_filter", "email", NULL);
		if(exec_ret1 < 1)    
    		{
			printf("error in execl");
    		}		
	}

	c2 = fork();

	if(c2 == 0 ) {
    		dup2(fd[0], 0); /* redirect standard input to pipe read end */
		close(fd[1]);
		close(fd[0]);
    		exec_ret2 = execl("calendar_filter", "cal", NULL);
		if(exec_ret2 < 1)    
    		{
			printf("error in execl");
    		}
	}
	close(fd[1]);
	close(fd[0]);

	waitpid(c1, NULL, 0);
	waitpid(c2, NULL, 0);

}

