#ifndef HASH_H_ 
#define HASH_H_

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

typedef struct hash_entry_t {
    char *key;
    char *value;
    struct hash_entry_t *next;
} HashEntry;

typedef struct hash_table_t {
    int size;
    HashEntry **table;
} HashTable;

HashTable *createHashTable(int size);

void deleteHashTable(HashTable *hashTable);

int getHashIndex(char *key, HashTable *hashTable);

HashEntry *getHashEntry(char *key, HashTable *hashTable);

void addHashEntry(char *key, char *value, HashTable *hashTable);

void printHashTable(HashTable *hashTable);
#endif // HASH_H_
