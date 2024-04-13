#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pipe_fd[2];

    // 파이프 생성
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    char buffer[100];
    read(pipe_fd[0], buffer, sizeof(buffer)); // 파이프로부터 데이터 읽기
    printf("Received message in child process: %s\n", buffer);
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    return 0;
}

