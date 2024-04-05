#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
pid_t pid;

	pid = fork();

	if (pid < 0) { /* 오류가 발생했음 */
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid == 0) { /* 자식 프로세스 */
		execlp("/bin/ls","ls",NULL);
	}
	else { /* 부모 프로세스 */
		/* 부모가 자식이 완료되기를 기다릴 것임 */
		wait(NULL);
		printf("Child Complete\n");
	}

	return 0;	
}

