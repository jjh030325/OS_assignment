#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signum) {
	switch(signum)
	{
	case 2:
		printf("시그널 SIGINT를 받았습니다.\n");
		break;
	case 10:
		printf("시그널 SIGUSR1을 받았습니다.\n");
		break;
	case 12:
		printf("시그널 SIGUSR2을 받았습니다.\n");
		break; 
	}
}

// SIGINT 핸들러 함수
void sigint_handler(int signum) {
    char choice;
    printf("\n종료할까요? (y/n): ");
    scanf(" %c", &choice);

    if(choice == 'y' || choice == 'Y') {
        printf("프로그램을 종료합니다.\n");
        exit(0);
    }
    else {
        printf("프로그램을 계속 실행합니다.\n");
    }
}

int main() {
	// 현재 프로세스의 PID 가져오기
    pid_t pid = getpid();

    printf("현재 프로세스의 PID: %d\n", pid);

    // SIGINT 핸들러 등록 (종료용)
    signal(SIGINT, sigint_handler);

	// 핸들러 등록
	signal(SIGUSR1, sig_handler);	
	signal(SIGUSR2, sig_handler);
    printf("프로그램을 실행중입니다. CTRL+C를 눌러 종료할 수 있습니다.\n");

    while(1) {
        // 무한 루프로 대기
    }

    return 0;
}

