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
  
void *handle_clnt(void *arg);         //클라이언트 처리 함수 
void send_msg(char *msg, int len); //메세지 전송 함수 
void error_handling(char *msg);     //에러 처리 함 
int clnt_cnt = 0;			//연결된 클라이언트 수 

int clnt_socks[MAX_CLNT];	           //클라이언트 소켓 배열 
pthread_mutex_t mutx;                //뮤텍스 객체 
pthread_cond_t cond;                 //조건 변수 객체 
  
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
 //초기화 및 소켓 생성 
    pthread_mutex_init(&mutx, NULL); 
    pthread_cond_init(&cond, NULL); 
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); 

    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_adr.sin_port = htons(atoi(argv[1])); 
  
    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) 
        error_handling("bind() error");   //소켓에 주소 바인딩 
    if (listen(serv_sock, 5) == -1) 
        error_handling("listen() error"); //소켓을 수신 대기 상태로 설정 
 //과제 시나리오 사진에서 있던 부분. 서버에서 출력 
    printf("Server starts..\n"); 
    printf("Waiting for clients.\n"); 
 //클라이언트 처리 스레드 생성. 미리 만들어 생성, 삭제에 드는 자원을 아끼는 스레드 풀 
    for (int i = 0; i < MAX_THREADS; ++i) { 
        pthread_create(&threads[i], NULL, handle_clnt, (void *)&serv_sock); 
    } 
 //스레드 종료 대기 
    for (int i = 0; i < MAX_THREADS; ++i) { 
        pthread_join(threads[i], NULL); 
    } 
 //소켓 닫기 
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
            pthread_cond_wait(&cond, &mutx);   //스레드 수가 최대치 넘으면 대기 
        } 
        pthread_mutex_unlock(&mutx); 
 //클라이언트 연결 수락 
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz); 
  
        pthread_mutex_lock(&mutx); 
        clnt_socks[clnt_cnt++] = clnt_sock;    //클라이언트 소켓 저장 
        pthread_cond_signal(&cond); 
        pthread_mutex_unlock(&mutx); 
  
        pthread_detach(pthread_self()); // 스레드를 분리하여 자원 해제 
  
        // 클라이언트 처리 함수 호출 
        // 여기서부터는 클라이언트와의 통신을 처리 
        char name[NORMAL_SIZE]; 
        int str_len = read(clnt_sock, name, sizeof(name) - 1);   //클라이언트 이름 읽기 
        if (str_len > 0) 
        { 
            name[str_len] = '\0'; 
            char welcome_msg[BUF_SIZE]; 
            sprintf(welcome_msg, "--- 새로운 사용자 %s이(가) 들어왔습니다. ---\n", name); 
            send_msg(welcome_msg, strlen(welcome_msg));  //모든 클라이언트에 전송 
            printf("client %d: %s joined.\n", clnt_sock-3, name);  //서버에 표기 
        } 
  
        char msg[BUF_SIZE]; 
        while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)//클라이언트 메세지 읽기 
        { 
            msg[str_len] = '\0'; 
            send_msg(msg, str_len);         //모든 클라이언트에 전송 
            printf("%s\n", msg);             //서버에 표기 
        } 
  
        pthread_mutex_lock(&mutx); 
        for (int i = 0; i < clnt_cnt; i++) 
        { 
            if (clnt_sock == clnt_socks[i]) 
            { 
                for (int j = i; j < clnt_cnt - 1; j++) 
                    clnt_socks[j] = clnt_socks[j + 1];   //클라이언트 소켓 배열에서 삭제 
                break; 
            } 
        } 
        clnt_cnt--; 
        pthread_mutex_unlock(&mutx); 
  
        close(clnt_sock);    //클라이언트 소켓 닫기 

        char leave_msg[BUF_SIZE]; 
        sprintf(leave_msg, "--- %s님이 나갔습니다. ---\n", name); 
        send_msg(leave_msg, strlen(leave_msg));    //모든 클라이언트에 퇴장 메세지 전송 
        printf("client %d: %s left.\n", clnt_sock-3, name);   //서버에 출력 
    } 
  
    return NULL; 
} 
 //메세지 송신 함수 
void send_msg(char *msg, int len) 
{ 
    pthread_mutex_lock(&mutx); 
    for (int i = 0; i < clnt_cnt; i++) 

        write(clnt_socks[i], msg, len);    //모든 클라이언트에 메세지 전송 
    pthread_mutex_unlock(&mutx); 

} 
 //에러 처리 함수 
void error_handling(char *msg) 
{ 

    fputs(msg, stderr);   //에러 메세지 출력 
    fputc('\n', stderr); 
    exit(1);                 //종료 

} 
