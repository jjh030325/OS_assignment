#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	for(int i=0;i<2;i++)
	{
		pid = fork(); //부모에서 두 번, 자식에서 네 번 작동되도록
		if(pid < 0) { //오류 발생
			fprintf(stderr, "ERROR");
			return 1;
		}else if(pid == 0)
		{
			//자식 프로세서
			printf("Hello\n");
		}else
		{
			//부모 프로세서
			wait(NULL);
			printf("Hello\n");
		}
	}

	return 0;	
}

