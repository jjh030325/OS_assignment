#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<time.h>

#define BUF_SIZE 100
#define MAX_CLNT 100

void * handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

typedef struct {
    int clnt_sock;
    int clnt_num;
} client_info;

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
int next_clnt_num=1; // 클라이언트 번호를 고유하게 관리하기 위한 변수
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    printf("Server starts..\n");
    printf("Waiting for clients.\n");

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt] = clnt_sock;
        int current_clnt_num = next_clnt_num++; // 고유한 클라이언트 번호 할당
        clnt_cnt++;
        pthread_mutex_unlock(&mutx);

        client_info *clnt_info = (client_info *)malloc(sizeof(client_info));
        clnt_info->clnt_sock = clnt_sock;
        clnt_info->clnt_num = current_clnt_num;

        pthread_create(&t_id, NULL, handle_clnt, (void*)clnt_info);
        pthread_detach(t_id);
        printf("Client%d: joined\n", current_clnt_num);
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    client_info clnt_info = *((client_info*)arg);
    int clnt_sock = clnt_info.clnt_sock;
    int clnt_num = clnt_info.clnt_num;
    free(arg);

    int str_len = 0, i;
    char msg[BUF_SIZE];

    // 클라이언트가 접속했음을 다른 클라이언트에게 알림
    snprintf(msg, sizeof(msg), "--- Client%d has joined ---\n", clnt_num);
    send_msg(msg, strlen(msg));

    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
    {
        if (clnt_sock == clnt_socks[i])
        {
            while (i++ < clnt_cnt - 1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);

    // 클라이언트가 나갔음을 다른 클라이언트에게 알림
    snprintf(msg, sizeof(msg), "--- Client%d has left ---\n", clnt_num);
    send_msg(msg, strlen(msg));

    printf("Client%d: left\n", clnt_num);
    return NULL;
}

void send_msg(char* msg, int len)
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

