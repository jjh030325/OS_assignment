#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

pid_t pid;
int Parent_pid, Child_pid;
void died(){
    printf("자식프로세스 종료");
    kill(Child_pid,9);
}

void kills(){
    int status;
    waitpid(-1, &status, WNOHANG);
    exit(0);
}

int main(){
    char A[80], B[80];
    int fdw, fdr;
    char *chatAtoB = "./chatAtoB";
    char *chatBtoA = "./chatBtoA";

    mkfifo(chatAtoB, 0666);
    mkfifo(chatBtoA, 0666);

    fdr = open(chatAtoB, O_RDONLY);
    fdw = open(chatBtoA, O_WRONLY);

    signal(SIGINT, died);
    signal(SIGCHLD, kills); //자식 프로세스가 죽으면 kills메소드로 이동하여 시그널을 처리한다.


    pid = fork();
    while(1){
        if(pid == 0){//자식 프로세스
            Parent_pid = getppid(); //부모 프로세스 번호 구하기
            Child_pid = getpid();   //자식 프로세스 번호 구하기

            fgets(B, 80, stdin);    //B를 입력문으로 받기
            write(fdw, B, strlen(B)+1); //내가 A한테 글을 쓴다.
        }

        else if(pid > 0) {  //부모 프로세스
            int i = 0, n = 0;

            while ((n=read(fdr, &A[i], 1)) > 0) {  //A이 나에게 쓴 글을 읽어 user배열에 저장한다.
                if (A[i] == '\0') break;
                if (i == 78) {
                    A[++i] = '\0';
                    break;
                }
                i++;
            }
            if (n <= 0) {   //자식 프로세스
                kill(Child_pid, 9);
                close(fdr);
                close(fdw);
                return 0;
            }
            printf("B : %s", A);
            sleep(1);
        }
        else{
            printf("fork 오류");
            return 0;
        }
    }
}
