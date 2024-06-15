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
#define MAX_THREADS 10

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;
pthread_cond_t cond;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t threads[MAX_THREADS];

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    pthread_cond_init(&cond, NULL);
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

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_create(&threads[i], NULL, handle_clnt, (void *)&serv_sock);
    }

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    int serv_sock = *((int *)arg);
    int clnt_sock;
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_sz = sizeof(clnt_adr);

    while (1)
    {
        pthread_mutex_lock(&mutx);
        while (clnt_cnt >= MAX_CLNT)
        {
            pthread_cond_wait(&cond, &mutx);
        }
        pthread_mutex_unlock(&mutx);

        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutx);

        pthread_detach(pthread_self()); // 스레드를 분리하여 자원 해제

        // 클라이언트 처리 함수 호출
        // 여기서부터는 클라이언트와의 통신을 처리
        char name[NORMAL_SIZE];
        int str_len = read(clnt_sock, name, sizeof(name) - 1);
        if (str_len > 0)
        {
            name[str_len] = '\0';
            char welcome_msg[BUF_SIZE];
            sprintf(welcome_msg, "--- 새로운 사용자 %s이(가) 들어왔습니다. ---\n", name);
            send_msg(welcome_msg, strlen(welcome_msg));
            printf("client %d: %s joined.\n", clnt_sock-3, name);
        }

        char msg[BUF_SIZE];
        while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        {
            msg[str_len] = '\0';
            send_msg(msg, str_len);
            printf("%s\n", msg);
        }

        pthread_mutex_lock(&mutx);
        for (int i = 0; i < clnt_cnt; i++)
        {
            if (clnt_sock == clnt_socks[i])
            {
                for (int j = i; j < clnt_cnt - 1; j++)
                    clnt_socks[j] = clnt_socks[j + 1];
                break;
            }
        }
        clnt_cnt--;
        pthread_mutex_unlock(&mutx);

        close(clnt_sock);

        char leave_msg[BUF_SIZE];
        sprintf(leave_msg, "--- %s님이 나갔습니다. ---\n", name);
        send_msg(leave_msg, strlen(leave_msg));
        printf("client %d: %s left.\n", clnt_sock-3, name);
    }

    return NULL;
}

void send_msg(char *msg, int len)
{
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
