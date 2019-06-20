#ifndef HASHMAP_H
#define HASHMAP_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Binding {
    char *key;
    unsigned int value;
} Binding;

typedef struct bindList {
    Binding head;
    struct bindList *tail;
} BindingList;

typedef struct Hashmap {
    BindingList **buckets;
    unsigned int quantity;
    unsigned int length;
    double threshold;
} Hashmap;

Hashmap hashmapCreate();
Hashmap hashmapSizeCreate(unsigned int amount, unsigned int capacity);

unsigned int jenkinsHash(char *string);
void bListAppend(BindingList *bucket, Binding item);

void bListConcat(BindingList *first, BindingList *second);
Hashmap hashmapExpand(Hashmap *from, unsigned int newSize, char onStack);
void bListDestroy(BindingList *bucket);
void hashmapDestroy(Hashmap *hMap, char onStack);
void bListPrint(BindingList *bucket);
void hashmapPrint(Hashmap h, char verbose);
Hashmap hashmapAdd(Hashmap h, Binding b);
char bListHasKey(BindingList *list, char *k);
char hashmapHasKey(Hashmap h, char *k);
unsigned int bListGetValue(BindingList *list, char *k);
unsigned int hashmapGetValue(Hashmap h, char *k);

#endif