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
			SglArr[i][j] = rand() % 50 + 50;
			MltArr[i][j] = SglArr[i][j];
		}
	}
}

//성적 순으로 정렬한 후 등급을 매기는 함수
void* sort_and_grade(int* total)
{
	int a_cut = total * 3 / 10;
	int b_cut = total * 6 / 10;

	//정렬 시키기
}

// 단일 쓰레드
struct timeval do_single_thread() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // 작업 수행

    gettimeofday(&end, NULL);
    return end - start;
}

// 멀티 쓰레드
struct timeval do_multi_thread() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // 작업 수행

    gettimeofday(&end, NULL);
    return end - start;
}

// 시간 차이 출력
void print_diff(struct timeval single_thread_time, struct timeval multi_thread_time) {
	printf("%ld.06ld\n %ld.06ld\n", single_thread_time.tv_sec, single_thread_time.tv_usec, multi_thread_time.tv_sec, multi_thread_time.tv_usec);
}

int main(int argc, char* argv[]) {
    struct timeval single_thread_processing_time = do_single_thread();
    struct timeval multi_thread_processing_time = do_multi_thread();

    print_diff(single_thread_processing_time, multi_thread_processing_time);
    return 0;
}

