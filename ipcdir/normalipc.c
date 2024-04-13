#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_MSG_SIZE 256
#define FIFO_FILE "/tmp/chat_fifo"

// 함수 선언
void receive_message();
void send_message();

int main() {
    pid_t pid;

    // FIFO 파일 생성
    mkfifo(FIFO_FILE, 0666);

    // 프로세스 생성
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // 자식 프로세스 (프로세스 B)
        receive_message();
    } else { // 부모 프로세스 (프로세스 A)
        send_message();
    }

    return 0;
}

// 메시지 수신 함수
void receive_message() {
    int fd;
    char message[MAX_MSG_SIZE];

    while (1) {
        // FIFO 열기 (읽기 모드)
        fd = open(FIFO_FILE, O_RDONLY);
        read(fd, message, MAX_MSG_SIZE);
        close(fd);

        // 받은 메시지 출력
        printf("B) %s\n", message);

        // 종료 조건 확인
        if (strcmp(message, "exit\n") == 0) {

            printf("Exiting...\n");
            break;
        }

        // 수신 대기
        printf("수신대기...\n");
        sleep(1);
    }
}

// 메시지 전송 함수
void send_message() {
    int fd;
    char message[MAX_MSG_SIZE];

    while (1) {
        // 메시지 입력 받기
        printf("입력대기: ");
        fgets(message, MAX_MSG_SIZE, stdin);

        // FIFO 열기 (쓰기 모드)
        fd = open(FIFO_FILE, O_WRONLY);
        // 메시지 쓰기
        write(fd, message, strlen(message)+1);
        close(fd);

        // 종료 조건 확인
        if (strcmp(message, "exit\n") == 0) {
            printf("Exiting...\n");
            break;
        }
        sleep(1);
    }
}

