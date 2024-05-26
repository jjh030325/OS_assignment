#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_BOOKS 10
#define NUM_THREADS 20

int books[NUM_BOOKS];
pthread_mutex_t lock;

void *check_books(void *arg) {
    pthread_mutex_lock(&lock);
    printf("Available books: ");
    for (int i = 0; i < NUM_BOOKS; i++) {
        if (books[i] == 0) {
            printf("%d ", i);
        }
    }
    printf("\n");
    pthread_mutex_unlock(&lock);
    return NULL;
}

void *borrow_book(void *arg) {
    int book_num = rand() % NUM_BOOKS;
    pthread_mutex_lock(&lock);
    if (books[book_num] == 0) {
        books[book_num] = 1; // Book is borrowed
        printf("Book %d borrowed.\n", book_num);
    } else {
        printf("Book %d is already borrowed.\n", book_num);
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);
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

    pthread_mutex_destroy(&lock);
    return 0;
}

