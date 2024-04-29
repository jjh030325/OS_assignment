#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_STUDENTS 10000000
#define NUM_THREADS 16

//학생과 성적 구조체
typedef struct Student {
    int score;
    char grade;
} Student;

//성능 비교를 위한 구조체 배열
Student SglArr[NUM_THREADS][MAX_STUDENTS];
Student MltArr[NUM_THREADS][MAX_STUDENTS];

//배열에 학생의 점수를 랜덤하게 입력하는 함수
void input_score() {
    for (int i = 0; i < NUM_THREADS; i++) {
        for (int j = 0; j < MAX_STUDENTS; j++) {
            //아무리 성적이 랜덤이라도 0이면 서러우니 최저점 50..
            //비교를 위한 것이니 랜덤이라도 싱글과 멀티 둘 다 같은 값이 들어가야함.
            SglArr[i][j].score = rand() % 50 + 50;
            MltArr[i][j].score = SglArr[i][j].score;
        }
    }
}

int cmpfunc(const void* a, const void* b) {
    const Student* A = (const Student*)a;
    const Student* B = (const Student*)b;

    return (B->score - A->score);
}

//성적 순으로 정렬한 후 등급을 매기는 함수
void* sort_and_grade(void* args) {
    int* params = (int*)args;
    int total = params[0];
    int num = params[1];
    int mltcheck = params[2];
    int a_cut = total * 3 / 10;
    int b_cut = total * 6 / 10;

    if (mltcheck) {
        //MltArr[num] 정렬
        qsort(MltArr[num], MAX_STUDENTS, sizeof(Student), cmpfunc);

        //등급매기기
        for (int j = 0; j < MAX_STUDENTS; j++) {
            if (MltArr[num][j].score < 60) {
                MltArr[num][j].grade = 'F';
            } else if (j < a_cut) {
                MltArr[num][j].grade = 'A';
            } else if (j < b_cut) {
                MltArr[num][j].grade = 'B';
            } else {
                MltArr[num][j].grade = 'C';
            }
        }
    } else {
        //SglArr[num] 정렬
        qsort(SglArr[num], MAX_STUDENTS, sizeof(Student), cmpfunc);

        //등급매기기
        for (int j = 0; j < MAX_STUDENTS; j++) {
            if (SglArr[num][j].score < 60) {
                SglArr[num][j].grade = 'F';
            } else if (j < a_cut) {
                SglArr[num][j].grade = 'A';
            } else if (j < b_cut) {
                SglArr[num][j].grade = 'B';
            } else {
                SglArr[num][j].grade = 'C';
            }
        }
    }

    return NULL;
}

// 단일 쓰레드
struct timeval do_single_thread() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // 작업 수행
    for (int i = 0; i < NUM_THREADS; i++) {
        int params[3] = {MAX_STUDENTS, i, 0};
        sort_and_grade((void*)params);
    }

    gettimeofday(&end, NULL);
    // 시간 차이 계산
    long seconds = end.tv_sec - start.tv_sec;
    long micros = end.tv_usec - start.tv_usec;
    if (micros < 0) {
        seconds--;
        micros += 1000000;
    }
    struct timeval diff;
    diff.tv_sec = seconds;
    diff.tv_usec = micros;
    return diff;
}

// 멀티 쓰레드
struct timeval do_multi_thread() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // 작업 수행
    pthread_t threads[NUM_THREADS];
    int thread_params[NUM_THREADS][3];

    // 쓰레드 생성
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_params[i][0] = MAX_STUDENTS;
        thread_params[i][1] = i;
        thread_params[i][2] = 1;
        pthread_create(&threads[i], NULL, sort_and_grade, (void*)thread_params[i]);
    }

    // 쓰레드 종료 대기
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);
    // 시간 차이 계산
    long seconds = end.tv_sec - start.tv_sec;
    long micros = end.tv_usec - start.tv_usec;
    if (micros < 0) {
        seconds--;
        micros += 1000000;
    }
    struct timeval diff;
    diff.tv_sec = seconds;
    diff.tv_usec = micros;
    return diff;
}

// 시간 차이 출력
void print_diff(struct timeval single_thread_time, struct timeval multi_thread_time) {
	for(int i=1;i<MAX_STUDENTS;i+=(MAX_STUDENTS/10))
	{
		printf("%d등 %d점 %c학점\n", i, SglArr[0][i].score, SglArr[0][i].grade);
	}
    printf("Single Thread: %ld.%06ld 초\n", single_thread_time.tv_sec, single_thread_time.tv_usec);

	for(int i=1;i<MAX_STUDENTS;i+=(MAX_STUDENTS/10))
	{
		printf("%d등 %d점 %c학점\n", i, MltArr[0][i].score, SglArr[0][i].grade);
	}
    printf("Multi Thread: %ld.%06ld 초\n", multi_thread_time.tv_sec, multi_thread_time.tv_usec);
}

int main(int argc, char* argv[]) {
    input_score();
    struct timeval single_thread_processing_time = do_single_thread();
    struct timeval multi_thread_processing_time = do_multi_thread();

    print_diff(single_thread_processing_time, multi_thread_processing_time);
    return 0;
}

