#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>

#define BUF_SIZE 100
#define NORMAL_SIZE 20

void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(char *msg);
void handle_exit(int sig);

int sock;
char name[NORMAL_SIZE] = "[DEFAULT]"; // 클라이언트 이름
char msg[BUF_SIZE];                   // 메시지 버퍼
volatile sig_atomic_t stop_flag = 0;  // 종료 신호 플래그

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;   //송신, 수신 스레드를 따로 사용
    void *thread_return;

    if (argc != 4)
    {
        printf("Usage : %s <ip> <port> <name>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, handle_exit);
    //클라이언트에서 이름 설정
    sprintf(name, "%s", argv[3]);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    printf("Client starts.\n");
    printf("Your name: %s\n", name);
    printf("Connection successful.\n");

    //송, 수신 스레드 생성 및, 종료 대기
    pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}
//메세지 보내는 함수
void *send_msg(void *arg)
{
    int sock = *((int *)arg);
    char name_msg[NORMAL_SIZE + BUF_SIZE + 1];

    // 연결되었을 때 이름 전송
    write(sock, name, strlen(name));

    while (1)
    {
        fgets(msg, BUF_SIZE, stdin);

        if (stop_flag)
            break;

        // 메시지 형식 설정
        sprintf(name_msg, "%s] %s", name, msg);

        // 서버로 메시지 전송
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}
//메세지 받는 함수
void *recv_msg(void *arg)
{
    int sock = *((int *)arg);
    char recv_msg[BUF_SIZE];
    int str_len;

    while (!stop_flag)
    {
        str_len = read(sock, recv_msg, sizeof(recv_msg) - 1);   //메세지읽기
        if (str_len == -1)
            return (void *)-1;
        recv_msg[str_len] = '\0';
        fputs(recv_msg, stdout);   //수신한 메세지 출력
    }
    return NULL;
}
//에러 처리 함수
void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
//시그널 핸들러를 통해 종료 여부 판단
void handle_exit(int sig)
{
    char c[10]; // 충분히 큰 버퍼를 사용
    printf("\n채팅을 종료할까요? (Y/N): ");
    fgets(c, sizeof(c), stdin); // 문자열 입력

    // 입력 버퍼 비우기
    if (strchr(c, '\n') == NULL) {
        int ch;
        while (((ch = getchar()) != '\n') && (ch != EOF));
    }

    if (c[0] == 'Y' || c[0] == 'y')
    {
        stop_flag = 1; // 종료 신호 설정
        close(sock);
        printf("종료합니다.\n");
        exit(0);
    }
    else
    {
        printf("채팅을 계속 진행합니다.\n");
    }
}

