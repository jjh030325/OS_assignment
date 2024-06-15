#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

#define BUF_SIZE 100
#define NORMAL_SIZE 20

void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(char *msg);

char name[NORMAL_SIZE] = "[DEFAULT]"; // 클라이언트 이름
char msg[BUF_SIZE];                   // 메시지 버퍼

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void *thread_return;

    if (argc != 4)
    {
        printf("Usage : %s <ip> <port> <name>\n", argv[0]);
        exit(1);
    }

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

    pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}

void *send_msg(void *arg)
{
    int sock = *((int *)arg);
    char name_msg[NORMAL_SIZE + BUF_SIZE + 1];

    // 연결되었을 때 이름 전송
    write(sock, name, strlen(name));

    while (1)
    {
        fgets(msg, BUF_SIZE, stdin);

        // 메시지 형식 설정
        sprintf(name_msg, "%s] %s", name, msg);

        // 서버로 메시지 전송
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}

void *recv_msg(void *arg)
{
    int sock = *((int *)arg);
    char recv_msg[BUF_SIZE];
    int str_len;

    while (1)
    {
        str_len = read(sock, recv_msg, sizeof(recv_msg) - 1);
        if (str_len == -1)
            return (void *)-1;
        recv_msg[str_len] = '\0';
        fputs(recv_msg, stdout);
    }
    return NULL;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

