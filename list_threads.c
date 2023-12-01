#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "list.h"

int thread1 = 1;
int thread2 = 1;
int thread3 = 1;

const int ELEMENTS_COUNT = 100;

void *func1(void *arg) {
    Storage *s = (Storage *)arg;
    Node *cur = s->first;

    while (1) {
        int pairs_count = 0;
        int steps = 0;
        while (1) {
            if (steps == 0) {
                pthread_mutex_lock(&(cur->sync));
            }
            steps++;
            if (cur->next == NULL) {
                pthread_mutex_unlock(&(cur->sync));
                break;
            }
            if (strlen(cur->next->value) > strlen(cur->value)) {
                pairs_count++;
            }
            Node* tmp = cur;
            cur = cur->next;
            pthread_mutex_lock(&(cur->sync));
            pthread_mutex_unlock(&(tmp->sync));
        }
        cur = s->first;
        printf("Thread - 1, iter - %d, pairs count - %d, steps - %d\n", thread1++, pairs_count, steps);
    }
}

void *func2(void *arg) {
    Storage *s = (Storage *)arg;
    Node *cur = s->first;
    while (1) {
        int pairs_count = 0;
        int steps = 0;
        while (1) {
            if (steps == 0) {
                pthread_mutex_lock(&(cur->sync));
            }
            steps++;
            if (cur->next == NULL) {
                pthread_mutex_unlock(&(cur->sync));
                break;
            }
            if (strlen(cur->next->value) < strlen(cur->value)) {
                pairs_count++;
            }
            Node* tmp = cur;
            cur = cur->next;
            pthread_mutex_lock(&(cur->sync));
            pthread_mutex_unlock(&(tmp->sync));
        }
        cur = s->first;
        printf("Thread - 2, iter - %d, pairs count - %d, steps - %d\n", thread2++, pairs_count, steps);
    }
}

void *func3(void *arg) {
    Storage *s = (Storage *)arg;
    Node *cur = s->first;
    while (1) {
        int steps = 0;
        int pairs_count = 0;
        while (1) {
            if (steps == 0) {
                pthread_mutex_lock(&(cur->sync));
            }
            steps++;
            if (cur->next == NULL) {
                pthread_mutex_unlock(&(cur->sync));
                break;
            }
            if (strlen(cur->next->value) == strlen(cur->value)) {
                pairs_count++;
            }
            Node* tmp = cur;
            cur = cur->next;
            pthread_mutex_lock(&(cur->sync));
            pthread_mutex_unlock(&(tmp->sync));
        }
        cur = s->first;
        printf("Thread - 3, iter - %d, pairs count - %d, steps - %d\n", thread3++, pairs_count, steps);
    }
}

void *func4(void *arg) {
    srand(time(NULL));

    Storage *s = (Storage *)arg;
    Node *cur = s->first;

    while (1) {
        while (cur->next->next) {
            pthread_mutex_lock(&(cur->sync));
            if (cur->next == NULL || cur->next->next == NULL) {
                break;
            }
            pthread_mutex_lock(&(cur->next->sync));
            pthread_mutex_lock(&(cur->next->next->sync));
            int ran = rand() % 2;
            if (ran == 0) {
                storage_swap(cur);
            }
            Node* tmp = cur;
            cur = cur->next;
            pthread_mutex_unlock(&(tmp->sync));
            pthread_mutex_unlock(&(tmp->next->sync));
            pthread_mutex_unlock(&(tmp->next->next->sync));
        }
        printf("Thread - 4 finish iter\n");
        cur = s->first;
    }
}

int main() {
    pthread_t tid1, tid2, tid3;
    Storage *s;
    int err;

    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

    s = storage_init();

    for (int i = 0; i < ELEMENTS_COUNT; ++i) {
        char *str = (char *)malloc((i + 1) * sizeof(char));
        if (str == NULL) {
            perror("malloc error");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < i; ++j) {
            str[j] = 'a';
        }
        str[i] = '\0';
        storage_add(s, str);
    }

    err = pthread_create(&tid1, NULL, func1, s);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid2, NULL, func2, s);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid3, NULL, func3, s);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    for (int i = 0; i < 3; ++i) {
        err = pthread_create(&tid3, NULL, func4, s);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    pthread_exit(NULL);

    return 0;
}
