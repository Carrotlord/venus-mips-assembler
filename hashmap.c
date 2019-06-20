#include "hashmap.h"

Hashmap hashmapCreate() {
    Hashmap hMap = {.buckets = NULL, .quantity = 0, .length = 0,
                    .threshold = 0.75};
    return hMap;
}

Hashmap hashmapSizeCreate(unsigned int amount, unsigned int capacity) {
    Hashmap hMap = {.buckets = NULL, .quantity = amount,
                    .length = capacity, .threshold = 0.75};
    if ((double) hMap.quantity / (double) hMap.length > hMap.threshold) {
        printf("Cannot create hashmap with insufficient capacity:\n%d/%d",
               hMap.quantity, hMap.length);
        exit(EXIT_FAILURE);
    } else {
        hMap.buckets = malloc(sizeof(BindingList *) * hMap.length);
        int i;
        for (i = 0; i < hMap.length; ++i) {
            hMap.buckets[i] = NULL;
        }
    }
    return hMap;
}

/* Modifies first so that it will be concatenated to second.
 * Does not deallocate second, since the pointer to second
 * must be preserved.
 */
void bListConcat(BindingList *first, BindingList *second) {
    if (first == NULL) {
        printf("Cannot concatenate to null BindingList\n");
    } else if (second == NULL) {
        return;
    } else {
        if (first->tail == NULL) {
            first->tail = second;
        } else {
            bListConcat(first->tail, second);
        }
    }
}

/* Expands a hashmap by copying over all former entries.
 * Please expand before adding entries that would go over
 * the threshold.
 * Frees the from hashmap.
 * Important: Do not hashmapDestroy(...) the from hashmap, because
 * pointers from the old hashmap are copied to the new one.
 */
Hashmap hashmapExpand(Hashmap *from, unsigned int newSize, char onStack) {
    Hashmap to = hashmapSizeCreate(from->quantity, newSize);
    int i;
    for (i = 0; i < from->length; ++i) {
        if (from->buckets[i] != NULL) {
            int hash = jenkinsHash((from->buckets[i])->head.key);
            int newIndex = hash % to.length;
            if (to.buckets[newIndex] == NULL) {
                /* This bucket is empty, overwrite it: */
                to.buckets[newIndex] = from->buckets[i];
            } else {
                /* This bucket is not empty. Append the bindings. */
                bListConcat(to.buckets[newIndex], from->buckets[i]);
            }
        }
    }
    if (!onStack) {
        free(from);
    }
    return to;
}

void bListDestroy(BindingList *bucket) {
    if (bucket != NULL) {
        if (bucket->tail == NULL) {
            free(bucket->head.key);
            free(bucket);
        } else {
            bListDestroy(bucket->tail);
            free(bucket->head.key);
            free(bucket);
        }
    }
}

void hashmapDestroy(Hashmap *hMap, char onStack) {
    int i;
    for (i = 0; i < hMap->length; ++i) {
        bListDestroy((hMap->buckets)[i]);
    }
    free(hMap->buckets);
    if (!onStack) {
        free(hMap);
    }
}

void bListPrint(BindingList *bucket) {
    printf("[\n");
    while (bucket != NULL) {
        printf("\t%s => %d\n", bucket->head.key, bucket->head.value);
        bucket = bucket->tail;
    }
    printf("]\n");
}

/* Appends the binding item to the linked list bucket. */
void bListAppend(BindingList *bucket, Binding item) {
    if (bucket == NULL) {
        printf("Cannot append to null BindingList.\n");
    } else {
        if (bucket->tail == NULL) {
            bucket->tail = malloc(sizeof(BindingList));
            bucket->tail->head = item;
            bucket->tail->tail = NULL;
        } else {
            bListAppend(bucket->tail, item);
        }
    }
}

void hashmapPrint(Hashmap h, char verbose) {
    int i;
    printf("{\n");
    for (i = 0; i < h.length; ++i) {
        char isEmpty = h.buckets[i] == NULL;
        if (verbose && isEmpty) {
            printf("%d : EMPTY\n", i);
        }
        if (!isEmpty) {
            printf("%d :\n", i);
            bListPrint(h.buckets[i]);
        }
    }
    printf("}\n");
}

/** Adds a key value pair (a binding) to the hashmap.
 *  TODO : Test this function (more?). */
Hashmap hashmapAdd(Hashmap h, Binding b) {
    if (h.length == 0) {
        /* Expand the hashmap to a reasonable size: */
        h.length = 10;
        h.buckets = malloc(sizeof(BindingList *) * h.length);
        int i;
        for (i = 0; i < h.length; ++i) {
            h.buckets[i] = NULL;
        }
        ++h.quantity;
        unsigned int index = jenkinsHash(b.key) % h.length;
        BindingList *bList = malloc(sizeof(BindingList));
        bList->head = b;
        bList->tail = NULL;
        h.buckets[index] = bList;
    } else {
        ++h.quantity;
        if ((double) h.quantity / (double) h.length > h.threshold) {
            /* Resize the hashmap: */
            int newSize = (int) (h.length * 1.5);
            if (newSize <= h.length) {
                ++newSize;
            }
            Hashmap bigger = hashmapExpand(&h, newSize, 1);
            return hashmapAdd(bigger, b);
        } else {
            unsigned int index = jenkinsHash(b.key) % h.length;
            if (h.buckets[index] != NULL) {
                bListAppend(h.buckets[index], b);
            } else {
                BindingList *bList = malloc(sizeof(BindingList));
                bList->head = b;
                bList->tail = NULL;
                h.buckets[index] = bList;
            }
        }
    }
    return h;
}

/** Returns true if the BindingList has
 *  string key k
 */
char bListHasKey(BindingList *list, char *k) {
    if (list == NULL) {
        return 0;
    } else {
        if (strcmp(list->head.key, k) == 0) {
            return 1;
        } else {
            return bListHasKey(list->tail, k);
        }
    }
}

char hashmapHasKey(Hashmap h, char *k) {
    if (h.length == 0) {
        // The label table is empty:
        return 0;
    }
    unsigned int hash = jenkinsHash(k);
    unsigned int index = hash % h.length;
    return bListHasKey(h.buckets[index], k);
}

unsigned int bListGetValue(BindingList *list, char *k) {
    if (list == NULL) {
        return 0;
    } else {
        if (strcmp(list->head.key, k) == 0) {
            return list->head.value;
        } else {
            return bListGetValue(list->tail, k);
        }
    }
}

unsigned int hashmapGetValue(Hashmap h, char *k) {
    unsigned int hash = jenkinsHash(k);
    unsigned int index = hash % h.length;
    return bListGetValue(h.buckets[index], k);
}

/** See:
 *  http://en.wikipedia.org/wiki/Jenkins_hash_function
 */
unsigned int jenkinsHash(char *string) {
    unsigned int hash, i;
    int length = strlen(string);
    for (hash = i = 0; i < length; ++i) {
        hash += string[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}
