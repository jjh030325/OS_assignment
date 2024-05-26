#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_BOOKS 10
#define NUM_THREADS 20

int books[NUM_BOOKS];
pthread_rwlock_t rwlock;

void *check_books(void *arg) {
    pthread_rwlock_rdlock(&rwlock);
    printf("Available books: ");
    for (int i = 0; i < NUM_BOOKS; i++) {
        if (books[i] == 0) {
            printf("%d ", i);
        }
    }
    printf("\n");
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *borrow_book(void *arg) {
    int book_num = rand() % NUM_BOOKS;
    pthread_rwlock_wrlock(&rwlock);
    if (books[book_num] == 0) {
        books[book_num] = 1; // Book is borrowed
        printf("Book %d borrowed.\n", book_num);
    } else {
        printf("Book %d is already borrowed.\n", book_num);
    }
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_rwlock_init(&rwlock, NULL);
    srand(time(NULL));

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i % 2 == 0) {
            pthread_create(&threads[i], NULL, check_books, NULL);
        } else {
            pthread_create(&threads[i], NULL, borrow_book, NULL);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);
    return 0;
}

