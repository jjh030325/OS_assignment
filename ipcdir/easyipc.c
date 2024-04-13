#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
  
#define BUFFER_SIZE 1024
  
int main() {
   int pipefd[2]; // 파이프용 파일 디스크립터 배열
   pid_t pid; // 프로세스 ID
   char buffer[BUFFER_SIZE];
 
   // 파이프 생성
   if (pipe(pipefd) == -1) {
       perror("pipe");
       exit(EXIT_FAILURE);
   }
 
   // fork() 호출을 사용하여 자식 프로세스 생성
   pid = fork();
 
   if (pid == -1) {
       perror("fork");
       exit(EXIT_FAILURE);
   }
 
   if (pid == 0) { // 자식 프로세스
       close(pipefd[1]); // 자식 프로세스는 쓰기용 파이프 닫음
 
       // 부모 프로세스로부터 메시지 읽기
       read(pipefd[0], buffer, BUFFER_SIZE);
       printf("Child Process: Received '%s' from Parent\n", buffer);
 
       close(pipefd[0]); // 읽기용 파이프 닫음
       exit(EXIT_SUCCESS);
   } else { // 부모 프로세스
       close(pipefd[0]); // 부모 프로세스는 읽기용 파이프 닫음
 
       char *message = "1234";
 
       // 부모 프로세스에서 메시지를 쓰기
       write(pipefd[1], message, strlen(message)+1);
       printf("Parent Process: Sent '%s' to Child...\n", message);
 
       close(pipefd[1]); // 쓰기용 파이프 닫음
       wait(NULL); // 자식 프로세스의 종료를 기다림
       exit(EXIT_SUCCESS);
   }
}
