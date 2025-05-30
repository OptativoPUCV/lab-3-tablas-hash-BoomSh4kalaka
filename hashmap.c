#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    long indice = hash(key, map->capacity);
    long original = indice;

    while (map->buckets[indice] != NULL && map->buckets[indice]->key != NULL) {
        if (is_equal(map->buckets[indice]->key, key)) {
            return;
        }

        indice = (indice + 1) % map->capacity;
        if (indice == original) {
            return;
        }
    }
    map->buckets[indice] = createPair(key, value);
    map->size++;
    map->current = indice;
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    Pair ** old_buckets = map->buckets;
    map->capacity *= 2;
    map->buckets = (Pair **)malloc(sizeof(Pair *) * map->capacity);
    if (map->buckets == NULL) {
        free(old_buckets);
        return;
    }
    for (int i = 0; i < map->capacity; i++) {
        map->buckets[i] = NULL;
    }
    map->size = 0;
    for (int i = 0; i < map->capacity / 2; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
            free(old_buckets[i]);
        }
    }
    free(old_buckets);
}


HashMap * createMap(long capacity) {
    HashMap * map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) return NULL;
    map->buckets = (Pair **)malloc(sizeof(Pair *) * capacity);
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }
    for (int i = 0; i < capacity; i++) {
        map->buckets[i] = NULL;
    }
    map->size = 0;
    map->capacity = capacity;
    map->current = -1;
    return map;
}

void eraseMap(HashMap * map,  char * key) {    
    long indice = hash(key, map->capacity);
    long original = indice;
    while (map->buckets[indice] != NULL) {
        if (map->buckets[indice]->key != NULL && is_equal(map->buckets[indice]->key, key)) {
            map->buckets[indice] -> key = NULL;
            map->size--;
            return;
        }
        indice = (indice + 1) % map->capacity;
        if (indice == original) {
            break;
        }
    }

}

Pair * searchMap(HashMap * map,  char * key) {   
    long indice = hash(key, map->capacity);
    long original = indice;
    while (map->buckets[indice] != NULL) {
        if (map->buckets[indice]->key != NULL && is_equal(map->buckets[indice]->key, key)) {
            map->current = indice;
            return map->buckets[indice];
        }
        indice = (indice + 1) % map->capacity;
        if (indice == original) {
            return NULL;
        }
    }

    return NULL;
}

Pair * firstMap(HashMap * map) {
    if (map->size == 0) {
        return NULL;
    }
    for (int i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    if (map->current == -1) {
        return NULL;
    }
    for (int i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}
