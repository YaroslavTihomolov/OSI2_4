//
// Created by я on 26.11.2023.
//

#ifndef OSI2_LIST_H
#define OSI2_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _Node {
    char value[100];
    struct _Node* next;
    pthread_mutex_t sync;
} Node;

typedef struct _Storage {
    Node *first;
} Storage;


Storage* storage_init();
void storage_destroy(Storage *n);
int storage_add(Storage *n, char* value);
int storage_get(Storage *n, char* value);
void storage_swap(Node* n);
void printList(Storage *s);
#endif //OSI2_LIST_H
