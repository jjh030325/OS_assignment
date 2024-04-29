#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_STUDENTS 1000

//학생과 성적 구조체
typedef struct Student{
	int score;
	char grade;
};

//성능 비교를 위한 구조체 배열

struct Student SglArr[10][MAX_STUDENTS];
struct Student MltArr[10][MAX_STUDENTS];

//배열에 학생의 점수를 랜덤하게 입력하는 함수
void input_score()
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<MAX_STUDENTS;j++)
		{
			//아무리 성적이 랜덤이라도 0이면 서러우니 최저점 50..
			//비교를 위한 것이니 랜덤이라도 싱글과 멀티 둘 다 같은 값이 들어가야함.
			SglArr[i][j].score = rand() % 50 + 50;
			MltArr[i][j].score = SglArr[i][j].score;
		}
	}
}

int compfunc(const void* a, const void* b) {
    struct Student* A = (struct Student *)a;
    struct Student* B = (struct Student *)b;

    return (B->score - A->score);
}

//성적 순으로 정렬한 후 등급을 매기는 함수
void* sort_and_grade(int* total, int num, bool mltcheck)
{
	int a_cut = total * 3 / 10;
	int b_cut = total * 6 / 10;

	if(mltcheck)
	{
		//MltArr[num] 정렬
		qsort((Student *)MltArr[num], MAX_STUDENTS, sizeof(Student), cmpfunc);

		//등급매기기
		for(int j=0;j<MAX_STUDENTS;j++)
		{
			if(MltArr[num][j].score < 60)
			{
				MltArr[num][j].grade = 'F';
			}else if(j < a_cut)
			{
				MltArr[num][j].grade = 'A';	
			}else if(j < b_cut)
			{
				MltArr[num][j].grade = 'B';
			}else
			{
				MltArr[num][j].grade = 'C';
			}
		}
	}else
	{
		//SglArr[num] 정렬
		qsort((Student *)MltArr[num], MAX_STUDENTS, sizeof(Student), cmpfunc);

		//등급매기기
		for(int j=0;j<MAX_STUDENTS;j++)
		{
			if(SglArr[num][j].score < 60)
			{
				SglArr[num][j].grade = 'F';
			}else if(j < a_cut)
			{
				SglArr[num][j].grade = 'A';
			}else if(j < b_cut)
			{
				SglArr[num][j].grade = 'B';
			}else
			{
				Sglarr[num][j].grade = 'C';
			}
		}
	}
}

// 단일 쓰레드
struct timeval do_single_thread() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // 작업 수행
	for(int i=0;i<10;i++)
	{
		sort_and_grade(MAX_STUDENTS, i, false);	
	}

    gettimeofday(&end, NULL);
    return end - start;
}

// 멀티 쓰레드
struct timeval do_multi_thread() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // 작업 수행
	pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // 쓰레드 생성
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, sort_and_grade(MAX_STUDENTS, i, true), (void*)&thread_ids[i]);
    }

    // 쓰레드 종료 대기
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);
    return end - start;
}

// 시간 차이 출력
void print_diff(struct timeval single_thread_time, struct timeval multi_thread_time) {
	printf("%ld.06ld\n %ld.06ld\n", single_thread_time.tv_sec, single_thread_time.tv_usec, multi_thread_time.tv_sec, multi_thread_time.tv_usec);
}

int main(int argc, char* argv[]) {
    input_score();
	struct timeval single_thread_processing_time = do_single_thread();
    struct timeval multi_thread_processing_time = do_multi_thread();

    print_diff(single_thread_processing_time, multi_thread_processing_time);
    return 0;
}

