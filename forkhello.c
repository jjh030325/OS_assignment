#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	for(int i=0;i<2;i++)
	{
		pid = fork();
		if(pid < 0) { //오류 발생
			fprintf(stderr, "ERROR");
			return 1;
		}else if(pid == 0)
		{
			//정상 작동
			printf("Hello\n");
		}else
		{
			wait(NULL);
			printf("Hello\n");
		}
	}

	return 0;	
}

