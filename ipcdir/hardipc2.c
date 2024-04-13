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
int end=0;

void died(){
    printf("자식 프로세스 종료");
    kill(Child_pid, 9);
}

void kills(){
    int status;
    waitpid(-1, &status, WNOHANG);
    exit(1);
}

int main(){
    char B[80], A[80];
    int fdw, fdr;

    char *chatAtoB = "./chatAtoB";
    char *chatBtoA = "./chatBtoA";

    mkfifo(chatAtoB, 0666);
    mkfifo(chatBtoA, 0666);

    fdw = open(chatAtoB, O_WRONLY);
    fdr = open(chatBtoA, O_RDONLY);

    signal(SIGINT, died);   //ctrl+c가 입력된 상황!
    signal(SIGCHLD, kills); //자식 프로세스가 종료된 상황!

    pid = fork();
    while(1){

        if(pid == 0){   //pid=0이면 자식 프로세스이다
            Parent_pid = getppid(); //부모 번호 구하기
            Child_pid = getpid();   //자식 번호 구하기

            fgets(A, 80, stdin); //user값을 받는다
            write(fdw, A, strlen(A)+1);
            //내가 B에게 글을 쓴다.
        }
        else if(pid > 0){ //부모 프로세스이다
            int i = 0, n = 0;

            while((n=read(fdr, &B[i], 1))>0){  //B가 나에게 쓴 글을 읽어 user배열에 넣는다.
                if(B[i] == '\0')
                    break;
                if(i == 78){
                    B[++i] = '\0';
                    break;
                }
                i++;
            }
            if(n<=0){
                kill(Child_pid, 9); //자식 프로세스를 죽인다!.
                close(fdr); //read모드 닫기
                close(fdw); //write모드 닫기
                return 0;
            }
            printf("A : %s", B);
            sleep(1);
        }
        else{
            printf("fork 오류");
            return 0;
        }
    }
}
