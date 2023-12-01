//
// Created by я on 26.11.2023.
//
#define _GNU_SOURCE

#include <pthread.h>

#include "list.h"

Storage *storage_init() {
    int err;

    Storage *s = malloc(sizeof(Storage));
    if (!s) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    s->first = NULL;

    return s;
}

void storage_destroy(Storage *s) {
    Node *current = s->first;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        pthread_mutex_destroy(&(temp->sync));
        free(temp);
    }
    free(s);
}

int storage_add(Storage *s, char *value) {
    \
    Node *new_node = (Node *) malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("Cannot allocate memory for new node\n");
        return -1;
    }

    strncpy(new_node->value, value, sizeof(new_node->value) - 1);
    new_node->value[sizeof(new_node->value) - 1] = '\0';
    new_node->next = NULL;
    pthread_mutex_init(&(new_node->sync), NULL);

    //pthread_mutex_lock(&(s->first->sync));
    new_node->next = s->first;
    s->first = new_node;
    //pthread_mutex_unlock(&(s->first->sync));

    return 0;
}

void storage_swap(Node *n) {
    Node *tmp = n->next;

    n->next = tmp->next;
    tmp->next = tmp->next->next;
    n->next->next = tmp;
}

int queue_get(Storage *s, char *value) {
    Node *tmp = s->first;

    strncpy(value, s->first->value, sizeof(value) - 1);
    s->first = s->first->next;

    free(tmp);
    return 0;
}

void printList(Storage *s) {
    Node * cur = s->first;
    while (cur) {
        printf("%lu ", strlen(cur->value));
        cur = cur->next;
    }
    printf("\n");
}

