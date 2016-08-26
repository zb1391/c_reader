#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "hash.h"

/*
 * create a hash table of a given size
 * it is an array of HashEntry arrays
 * the lookup has a min of O(1) and a max of O(n)
 * each key is mapped to a certain index
 * then you can find key/values by iterating over the
 * array of that index
 */
HashTable *createHashTable(int size) {
    HashTable *hashTable;
    HashEntry **table;
    int i;

    hashTable = (HashTable *) malloc(sizeof(HashTable));
    table = (HashEntry **) malloc(size * sizeof(HashEntry *));
    for(i = 0; i < size; i++) {
        table[i] = NULL;
    }
    
    hashTable->size = size;
    hashTable->table = table;
  return hashTable;
}

/*
 * delete the hash table
 */
void deleteHashTable(HashTable *hashTable) {
    int i;
    HashEntry *row, *next;
    for(i = 0; i < hashTable->size; i++) {
        row = hashTable->table[i];
        while(row != NULL) {
            next = row->next;
            free(row->key);
            free(row->value);
            free(row);
            row = next;
        }
    }
    free(hashTable->table);
    free(hashTable);
}

/*
 * each key is mapped to an index in the hashTable->table array
 * this returns the index
 * for now i am summing up each char, dividing by the strlen
 * and modding by the size
 */
int getHashIndex(char *key, HashTable *hashTable) {
    int charVal, i;
    charVal = 0;
    for(i = 0; i < strlen(key); i++) {
        charVal += (int) key[i];
    }
    charVal = charVal / strlen(key);

    return charVal % hashTable->size;
}

/*
 * return the HashEntry for a given key in a HashTable
 * if the key is not found then return NULL
 */
HashEntry *getHashEntry(char *key, HashTable *hashTable) {
    int index;
    HashEntry *cur;

    // get the proper row in the HashTable
    index = getHashIndex(key, hashTable);
    cur = hashTable->table[index];

    // iterate over keys in row until a match is found
    while(cur && strcmp(key, cur->key) != 0) {
        cur = cur->next;
    }

    return cur;
}

/*
 * add a key, value pair to a HashTable
 * just replace the value if the key is already present
 */
void addHashEntry(char *key, char *value, HashTable *hashTable) {
    int index;
    HashEntry *cur, *prev;
    // get the proper row in the HashTable
    index = getHashIndex(key, hashTable);
    cur = hashTable->table[index];

    // find the end of the linked list or duplicate key
    while(cur && strcmp(key,cur->key) != 0) {
        prev = cur;
        cur = cur->next;
    }

    // just replace the value if key is already present
    if(cur && strcmp(key, cur->key) == 0) {
        free(cur->value);
        cur->value = strdup(value);
        return;
    }

    // create the new entry
    cur = (HashEntry *) malloc(sizeof(HashEntry));
    cur->key = strdup(key);
    cur->value = strdup(value);
    cur->next = NULL;

    // either add to head or add to tail
    if(hashTable->table[index] == NULL) {
        hashTable->table[index] = cur;
    } else {
        prev->next = cur;
    }
}

void printHashTable(HashTable *hashTable) {
    int i;
    HashEntry *cur;

    printf("--------\n");
    printf("Hash Table: size %d\n", hashTable->size);
    for(i = 0; i < hashTable->size; i++) {
        printf("%d: ", i);
        cur = hashTable->table[i];
        while(cur) {
            printf("{key: %s, val: %s} ", cur->key, cur->value);
            cur = cur->next;
        }
        printf("\n");
    }
    printf("--------\n");
}
