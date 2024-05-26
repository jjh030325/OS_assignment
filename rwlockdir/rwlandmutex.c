#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_BOOKS 10
#define NUM_THREADS 20

int books[NUM_BOOKS];
pthread_mutex_t mutex_lock;
pthread_rwlock_t rw_lock;

//mutex에서 빌릴 수 있는 책 목록 출력
void *check_books_mutex(void *arg) {
    pthread_mutex_lock(&mutex_lock);
    printf("Mutex - 빌릴 수 있는 책 번호: ");
    for (int i = 0; i < NUM_BOOKS; i++) {
        if (books[i] == 0) {
            printf("%d ", i);
        }
    }
    printf("\n");
    pthread_mutex_unlock(&mutex_lock);
    return NULL;
}

//mutex에서 책을 대출
void *borrow_book_mutex(void *arg) {
    int book_num = rand() % NUM_BOOKS;
    pthread_mutex_lock(&mutex_lock);
    if (books[book_num] == 0) {
        books[book_num] = 1; // 빌린 책
        printf("Mutex - %d번 책을 빌렸습니다.\n", book_num);
    } else {
        printf("Mutex - %d번 책은 이미 대출된 책입니다.\n", book_num);
    }
    pthread_mutex_unlock(&mutex_lock);
    return NULL;
}

//rwlock에서 빌릴 수 있는 책 목록 출력
void *check_books_rwlock(void *arg) {
    pthread_rwlock_rdlock(&rw_lock);
    printf("RWLock - 빌릴 수 있는 책 번호: ");
    for (int i = 0; i < NUM_BOOKS; i++) {
        if (books[i] == 0) {
            printf("%d ", i);
        }
    }
    printf("\n");
    pthread_rwlock_unlock(&rw_lock);
    return NULL;
}

//rwlock에서 책을 대출
void *borrow_book_rwlock(void *arg) {
    int book_num = rand() % NUM_BOOKS;
    pthread_rwlock_wrlock(&rw_lock);
    if (books[book_num] == 0) {
        books[book_num] = 1; // 빌린 책
        printf("RWLock - %d번 책을 빌렸습니다.\n", book_num);
    } else {
        printf("RWLock - %d번 책은 이미 대출된 책입니다.\n", book_num);
    }
    pthread_rwlock_unlock(&rw_lock);
    return NULL;
}

//모든 책이 대출되지 않은 상태로 설정하는 함수
void reset_books() {
    for (int i = 0; i < NUM_BOOKS; i++) {
        books[i] = 0;
    }
}

//mutex로 실행
void run_mutex() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex_lock, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i % 2 == 0) {
            pthread_create(&threads[i], NULL, check_books_mutex, NULL);
        } else {
            pthread_create(&threads[i], NULL, borrow_book_mutex, NULL);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex_lock);
}

//rwlock으로 실행
void run_rwlock() {
    pthread_t threads[NUM_THREADS];
    pthread_rwlock_init(&rw_lock, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i % 2 == 0) {
            pthread_create(&threads[i], NULL, check_books_rwlock, NULL);
        } else {
            pthread_create(&threads[i], NULL, borrow_book_rwlock, NULL);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_rwlock_destroy(&rw_lock);
}

int main() {
    srand(time(NULL));

    // Mutex time
    reset_books();
    clock_t start_mutex = clock();
    run_mutex();
    clock_t end_mutex = clock();
    double time_mutex = (double)(start_mutex - end_mutex) / CLOCKS_PER_SEC;

    // RWLock time
    reset_books();
    clock_t start_rwlock = clock();
    run_rwlock();
    clock_t end_rwlock = clock();
    double time_rwlock = (double)(start_rwlock - end_rwlock) / CLOCKS_PER_SEC;

    printf("Mutex기반 : %f seconds\n", time_mutex);
    printf("RWLock기반 : %f seconds\n", time_rwlock);

    return 0;
}

