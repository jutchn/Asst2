#include<stdio.h> 
#include<stdlib.h>
#include <math.h>

#define HASHSIZE 10

struct ListNode {
    char* str;
    float freq;
    struct ListNode* next;
};
struct FileNode {
    char* str;
    int count;
    struct ListNode** tokens;
    struct FileNode* next;
};

int getHashKey(char* str) {
    int val = 0;
    for (int i = 0; i < strlen(str); i++) {
        val += (int)(str[i]);
    }
    return val % HASHSIZE;
}

struct ListNode* searchList(struct ListNode* head, char* target) {
    struct ListNode* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->str, target) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

struct FileNode* searchFile(struct FileNode* head, char* target) {
    struct FileNode* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->str, target) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

struct ListNode* searchHash(struct ListNode** hash, char* target) {
    int key = getHashKey(target);
    return searchList(hash[key], target);
}

struct ListNode* addFile(struct FileNode* head, char* string, float count) {
    if (head == NULL) {
        struct FileNode* newNode = (struct FileNode*)malloc(sizeof(struct FileNode));
        newNode->str = (char*)malloc(sizeof(char) * (strlen(string) + 1));
        strcpy(newNode->str, string);
        newNode->tokens = (struct ListNode**)malloc(sizeof(struct ListNode*) * HASHSIZE);
        for (int i = 0; i < HASHSIZE; i++) {
            newNode->tokens[i] = NULL;
        }
        newNode->count = count;
        newNode->next = NULL;
        return newNode;
    }
    struct FileNode* temp = searchFile(head, string);
    if (temp == NULL) {
        struct FileNode* newNode = (struct ListNode*)malloc(sizeof(struct FileNode));
        newNode->str = (char*)malloc(sizeof(char) * (strlen(string) + 1));
        strcpy(newNode->str, string);
        newNode->tokens = (char**)malloc(sizeof(char*) * HASHSIZE);
        for (int i = 0; i < HASHSIZE; i++) {
            newNode->tokens[i] = NULL;
        }
        newNode->count = count;
        newNode->next = head;
        return newNode;
    }
    else {
        return head;
    }
}

struct ListNode* addList(struct ListNode* head, char* string, float count) {
    if (head == NULL) {
        struct ListNode* newNode = (struct ListNode*)malloc(sizeof(struct ListNode));
        newNode->str = (char*)malloc(sizeof(char) * (strlen(string) + 1));
        strcpy(newNode->str, string);
        newNode->freq = count;
        newNode->next = NULL;
        return newNode;
    }
    struct ListNode* temp = searchList(head, string);
    if (temp == NULL) {
        struct ListNode* newNode = (struct ListNode*)malloc(sizeof(struct ListNode));
        newNode->str = (char*)malloc(sizeof(char) * (strlen(string) + 1));
        strcpy(newNode->str, string);
        newNode->freq = count;
        newNode->next = head;
        return newNode;
    }
    else {
        temp->freq += count;
        return head;
    }
}

struct ListNode** addHash(struct ListNode** hash, char* target, float count) {
    int key = getHashKey(target);
    hash[key] = addList(hash[key], target, count);
    return hash;
}

void printList(struct ListNode* head) {
    if (head == NULL) {
        printf("NULL\n");
        return;
    }
    struct ListNode* temp = head;
    while (temp != NULL) {
        printf("\"%s\" %d\n", temp->str, temp->freq);
        temp = temp->next;
    }
}

void printHash(struct ListNode** hash) {
    for (int i = 0; i < HASHSIZE; i++) {
        printf("[%d]\n", i + 1);
        printList(hash[i]);
    }
    return;
}

void printFiles(struct FileNode* head) {
    if (head == NULL) {
        printf("NULL\n");
        return;
    }
    struct FileNode* temp = head;
    while (temp != NULL) {
        printf("\"%s\" %d ----------------------------\n", temp->str, temp->count);
        printHash(temp->tokens);
        temp = temp->next;
    }
}

struct FilePair {
    char* file1;
    char* file2;
    float distance;
};

struct FilePair* jsDistance(struct FileNode* first, struct FileNode* second) {
    if (first == NULL || second == NULL) {
        return NULL;
    }
    struct ListNode** firstList = first->tokens;
    struct ListNode** secondList = second->tokens;
    
    float KLD1 = 0;
    float KLD2 = 0;
    char** checkedStrings = (char**) malloc(sizeof(char*) * (first->count + second->count));
    int checkedStringsCounter = 0;
    for (int i = 0; i < HASHSIZE; i++) {
        struct ListNode* first1 = firstList[i];
        while (first1 != NULL) {
            struct ListNode* second1 = searchHash(secondList, first1->str);
            checkedStrings[checkedStringsCounter] = (char*)malloc(sizeof(char) * strlen(first1->str));
            strcpy(checkedStrings[checkedStringsCounter++], first1->str);

            if (second1 == NULL) {
                float mean = (first1->freq) / 2;
                KLD1 += (first1->freq) * log10(first1->freq / mean);
            }
            else {
                float mean = (first1->freq + second1->freq) / 2;
                KLD1 += (first1->freq) * log10(first1->freq / mean);
                KLD2 += (second1->freq) * log10(second1->freq / mean);
            }
            //printf("%f\n%f\n", KLD1, KLD2);
            first1 = first1->next;
        }
    }
    for (int i = 0; i < HASHSIZE; i++) {
        struct ListNode* first2 = secondList[i];
        while (first2 != NULL) {
            int checked = 0;
            for (int i = 0; i < checkedStringsCounter; i++) {
                if (strcmp(checkedStrings[i], first2->str) == 0) {
                    checked = 1;
                    break;
                }
            }
            if (!checked) {
                struct ListNode* second2 = searchHash(firstList, first2->str);
                if (second2 == NULL) {
                    float mean = (first2->freq) / 2;
                    KLD1 += (first2->freq) * log10(first2->freq / mean);
                }
                else {
                    float mean = (first2->freq + second2->freq) / 2;
                    KLD1 += (first2->freq) * log10(first2->freq / mean);
                    KLD2 += (second2->freq) * log10(second2->freq / mean);
                }
            }

            first2 = first2->next;
        }

    }

    struct FilePair* pair = (struct FilePair*)malloc(sizeof(struct FilePair));
    pair->file1 = first->str;
    pair->file2 = second->str;
    pair->distance = (KLD1 + KLD2) / 2;
    
    return(pair);
}
void printPairs(struct FileNode* head) {
    struct FileNode* first = head;
    while (first->next != NULL) {
        struct FileNode* second = first->next;
        while (second != NULL) {
            struct FilePair* pair = jsDistance(first, second);
            if (pair->distance <= 0.1) {
                printf("\033[0;31m");
                printf("%0.4f ", pair->distance);
                printf("\033[0m");
            }
            else if (pair->distance <= 0.15) {
                printf("\033[0;33m");
                printf("%0.4f ", pair->distance);
                printf("\033[0m");
            }
            else if (pair->distance <= 0.2) {
                printf("\033[0;32m");
                printf("%0.4f ", pair->distance);
                printf("\033[0m");
            }
            else if (pair->distance <= 0.25) {
                printf("\033[0;36m");
                printf("%0.4f ", pair->distance);
                printf("\033[0m");
            }
            else if (pair->distance <= 0.3) {
                printf("\033[0;34m");
                printf("%0.4f ", pair->distance);
                printf("\033[0m");
            } else{
                printf("%0.4f ", pair->distance);
            }
            printf("\"%s\" and \"%s\" \n", pair->file1, pair->file2);
            second = second->next;
        }
        first = first->next;
    }
}
int main() {
    struct FileNode* head = NULL;

    head = addFile(head, "a.txt", 2);
    struct ListNode** hash = head->tokens;
    hash = head->tokens;
    addHash(hash, "there", 0.5);
    addHash(hash, "hi", 0.5);

    head = addFile(head, "File4", 3);
    hash = head->tokens;
    addHash(hash, "cat", 0.5);
    addHash(hash, "dog", 0.375);
    addHash(hash, "doge", 0.125);

    head = addFile(head, "c.txt", 3);
    hash = head->tokens;
    addHash(hash, "hi", 0.5);
    addHash(hash, "out", 0.25);
    addHash(hash, "there", 0.25);

    head = addFile(head, "b.txt", 2);
    hash = head->tokens;
    addHash(hash, "there", 0.333);
    addHash(hash, "hi", 0.667);

    //printf("%f", test->distance);
    printPairs(head);
    return 0;
}