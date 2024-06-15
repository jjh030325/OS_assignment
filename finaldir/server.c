#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 100
#define NORMAL_SIZE 20

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    printf("Server starts..\n");
    printf("Waiting for clients.\n");

    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        pthread_detach(t_id);
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock = *((int *)arg);
    int str_len = 0, i;
    char msg[BUF_SIZE];
    char name[NORMAL_SIZE];

    // 클라이언트가 연결되었을 때 이름을 읽음
    if ((str_len = read(clnt_sock, name, sizeof(name) - 1)) != 0)
    {
        name[str_len] = '\0'; // 이름 끝에 널 문자 추가

        // 새로운 클라이언트가 연결되었음을 모든 클라이언트에게 알림
        char welcome_msg[BUF_SIZE];
        sprintf(welcome_msg, "--- 새로운 사용자 %s이 들어왔습니다. ---\n", name);
        send_msg(welcome_msg, strlen(welcome_msg));

        // 서버 콘솔에 출력
        printf("client %d: %s joined.\n", clnt_sock-3, name);
    }

    while ((str_len = read(clnt_sock, msg, sizeof(msg) - 1)) != 0)
    {
        msg[str_len] = '\0'; // 읽어온 데이터 끝에 널 문자 추가
        send_msg(msg, str_len); // 클라이언트로부터 받은 메시지를 모든 클라이언트에게 전송
        printf("%s\n", msg); // 서버 콘솔에 클라이언트가 보낸 메시지 출력
    }

    // 클라이언트가 연결을 끊었을 때
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
    {
        if (clnt_sock == clnt_socks[i])
        {
            // 현재 클라이언트를 클라이언트 배열에서 제거하고 나머지 클라이언트들을 앞으로 이동
            for (int j = i; j < clnt_cnt - 1; j++)
                clnt_socks[j] = clnt_socks[j + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);

    // 클라이언트가 나갔음을 모든 클라이언트에게 알림
    sprintf(msg, "--- %s님이 나갔습니다. ---\n", name);
    send_msg(msg, strlen(msg));

    // 서버 콘솔에 출력
    printf("client %d: %s left.\n", clnt_sock-3, name);

    return NULL;
}

void send_msg(char *msg, int len)
{
    int i;
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

