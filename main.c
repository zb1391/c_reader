#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#include <ctype.h>

typedef enum file_type_t {
    DEFAULT,
    C,
    ASM
} FILE_TYPE;

typedef enum text_status_t {
    COMMENT,
    STRING,
    DEFAULT
} TextStatus;

typedef struct llnode_t {
    char *text;
    struct llnode_t *next;
} LLNode;

typedef struct linked_list_t {
    LLNode *head;
    LLNode *tail;
} LinkedList;

typedef struct file_reader_t {
    FILE *fp;
    FILE_TYPE type;
    LinkedList *content; // an array for each word/space/tab/newline
} FileReader;

/*
 * create a LLNode
 */
LLNode *createNode(char *text) {
    LLNode *node;
    node = (LLNode *) malloc(sizeof(LLNode));
    node->next = NULL;
    node->text = strdup(text);
    return node;
};

/*
 * add to LinkedList list's tail
 * if the list is empty, the new node is the head and the tail
 * otherwise just push to the tail's next pointer
 */
void addToTail(LinkedList *list, LLNode *node) {
    LLNode *head, *tail;
    head = list->head;
    tail = list->tail;
    if(!head) {
        list->head = node;
    }
    else {
        list->tail->next = node;
    }

    // always update the list tail to be the new node
    list->tail = node; 
};

/* 
 * get the file type based on the path to the file
 */
FILE_TYPE getFileType(char *path) {
    char *ext;
    FILE_TYPE type = DEFAULT;

    ext = strtok(path, ".");
    // run a second time to get file extension
    ext = strtok(NULL, ".");

    if(strcmp(ext,"c") == 0) {
        type = C;
    } 
    else if(strcmp(ext,"asm") == 0) {
        type = ASM;
    }
    return type;
};

/*
 * open a file
 * return a FILE pointer
 */
FileReader *openFile(char *path) {
    FileReader *fr;
    fr = (FileReader *) malloc(sizeof(FileReader));
    fr->fp = fopen(path,"r");
    fr->type = getFileType(path);
    fr->content = (LinkedList *) malloc(sizeof(LinkedList));
    fr->content->head = NULL;
    fr->content->tail = NULL;
    return fr;
};

/*
 * free data and close file pointer
 */
void closeFile(FileReader * fr) {
  fclose(fr->fp);
  free(fr);
};

/*
 * check if the character is one of the delimiters
 * i think this is gonna be the hardest part
 * because you can do things like 100/2 and it should be two numbers
 * but i dont want to use / as a delimeter because comments can be //
 * i think i will have to make an isComment which compares cur and cur->next
 * and if they are both / or they are the multi line comment then i know
 */
int isDelim(char c) {
    return (
        c == '\n' ||
        c == ' '  ||
        c == '\t' ||
        c == ';'  ||
        c == '['  ||
        c == ']'  ||
        c == '('  ||
        c == ')'  ||
        c == '{'  ||
        c == '}'  ||
        c == ':'  ||
        c == '+'  ||
        c == '-'  ||
        c == '^'  ||
        c == '<'  ||
        c == '>'  ||
        c == '.'  ||
        c == ','  ||
        c == '/'  ||
        c == '|'
    );
};

int isNum(char *text) {
    int number = atoi(text);
    return (!(number == 0 && text[0] != '0'));
}

/*
 * read file and store in FileReader->content
 * iterates over each character in the file
 * as soon as it find a newline/tab/space
 * it copies all text from the start of the previous
 * delimiter to the current filepointer position
 * then continues on
 */
void readFile(FileReader *fr) {
    char c;
    char word[255]; //assume words are less than 255 chars for now
    long previousPos = ftell(fr->fp);
    int charcount = 0;

    do {
        c = fgetc(fr->fp);
        charcount++;
        if(isDelim(c)) {
            // just the delimiter
            if(charcount == 1) {
                char *text = (char *)calloc(2,sizeof(char));
                text[0] = c;
                addToTail(fr->content, createNode(text));
                charcount = 0;
                previousPos = ftell(fr->fp);
            }
            else {
                // stash the old previousPos and set the new one
                long prev = previousPos;
                previousPos = ftell(fr->fp) - 1;

                // rewind the file back to the start of the word
                fseek(fr->fp, prev, SEEK_SET);

                // copy the word over
                charcount--;
                char *text = (char *)calloc((charcount),sizeof(char));
                int i = 0;
                while(ftell(fr->fp) < previousPos) {
                    text[i] = fgetc(fr->fp);
                    i++;
                }
                // append to LinkedList and reset vars
                addToTail(fr->content, createNode(text));
                free(text);
                charcount = 0;
            }
        }
    } while(c != EOF);
};

// TODO: figure out a way to know the text status
// and a way to update it when the text_status should change
// ie when the comment is over
void printFile(FileReader *fr, TextStatus *ts) {
    LinkedList *list = fr->content;
    LLNode *cur = list->head;
    while(cur) {
        char *color;
        if(*(ts) == COMMENT) {
            color = BLUE;
        }
        if(isNum(cur->text)){
            color = RED;
        }
        else {
            color = RESET;
        }
        printf("%s%s", color, cur->text);
        cur = cur->next;
    }
};

int main() {
    FileReader *fr;
    
    char file[100] = "main.c";
    fr = openFile(file);
    readFile(fr);
    printFile(fr);

    // finish
    printf("\n");
    closeFile(fr);

    HashTable *hashTable = createHashTable(10);
    printHashTable(hashTable);
    addHashEntry("test", "test-value", hashTable);
    printHashTable(hashTable);
    addHashEntry("test", "test-value2", hashTable);
    printHashTable(hashTable);
    deleteHashTable(hashTable);
    printf(RESET);
    return 0;
}
