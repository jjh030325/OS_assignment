#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main() {
    int pid;
    int signal_type;

    printf("프로세스의 PID를 입력하세요: ");
    scanf("%d", &pid);

	while(1)
	{
    	printf("\n전달할 시그널을 선택하세요:\n");
    	printf("1. SIGINT\n");
    	printf("2. SIGUSR1\n");
		printf("3. SIGUSR2\n");
		printf("잘못된 입력시 종료됩니다.\n");
    	printf("입력: ");
	    scanf("%d", &signal_type);

    	switch (signal_type) {
        	case 1:
            	// SIGINT 시그널 생성하여 전달
            	kill(pid, SIGINT);
            	printf("SIGINT 시그널을 전달했습니다.\n");
            	break;
        	case 2:
        	    // SIGUSR1 시그널 생성하여 전달
    	        kill(pid, SIGUSR1);
	            printf("SIGUSR1 시그널을 전달했습니다.\n");
            	break;
			case 3:
				// SIGUSR2 시그널 생성하여 전달
				kill(pid, SIGUSR2);
				printf("SIGUSR2 시그널을 전달했습니다.\n");
				break;
        	default:
        	    printf("잘못된 입력입니다.\n");
				return 0;
	    }
	}
    return 0;
}

