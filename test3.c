#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h> 
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

#define HASHSIZE 10

pthread_mutex_t lock;

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

struct token_struct{
   int* arg1;
   pthread_mutex_t* arg2;
}token;

typedef struct folder_struct
{
   char* buf;
   pthread_mutex_t* mutex;
}folder;

/*
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
*/

typedef struct pthreads {
    pthread_t* thread_id;
    struct pthreads* next;
}pthreads;

typedef struct tInput {
    char* name;
    pthreads* phead;
    struct FileNode* front;
    pthread_mutex_t* lock;
}tInput;





pthreads* addThread(pthreads *head, pthread_t* new){
    pthreads* newThread = (pthreads*)malloc(sizeof(pthreads));
    newThread->thread_id = new;
    newThread->next = NULL;
    int counter = 0;
    if(head == NULL){
        return newThread;
    }
    pthreads* ref = head;
    while(ref->next != NULL){
        counter+=1;
        ref = ref->next;
    }
    ref->next = newThread;
    printf("   added thread %d\n",counter);
    return head;
}

void printThread(pthreads *head){
    printf("counting threads\n");
    int count = 0;
    while(head->next != NULL){
        count +=1;
    }
    printf("count is %d\n",count);
}

int validDir(char* dirname){
    DIR* dir;
    if ((dir = opendir(dirname)) == NULL) {
        printf("Cannot open %s\n", dirname);
        return -1;
    }
     else return 1;
}

void* handleDir(void *vinput){
    printf("peeee");
    //printf("thread id is %d\n", pthread_self());
    tInput *input = vinput;
    char* dirname = input->name;
    
    printf("%s\n",input->name);

    if(validDir(dirname) == -1){
        return NULL;
    }
    
    DIR* dir = opendir(dirname);
    struct dirent *dp;
    struct stat path_stat;
    
    while((dp = readdir(dir))){
        if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
            continue;
        }
        int size = strlen(dirname) + strlen(dp->d_name) + 1;
        char *buf = malloc(size*sizeof(char));
        strcpy(buf,dirname);
        strcat(buf,"/");
        strcat(buf,dp->d_name);
        stat(buf, &path_stat);
        
        tInput* new = (tInput*)malloc(sizeof(tInput));
        new->name = buf;
        new->phead = input->phead;
        new->front = input->front;
        new->lock = input->lock;
        
        // is a directory
        if (S_ISDIR(path_stat.st_mode)){
            pthread_t thread_id;
            pthread_mutex_lock(new->lock);
            new->phead = addThread(new->phead, &thread_id);
            pthread_mutex_unlock(new->lock);
            pthread_create(&thread_id, NULL, handleDir, (void*)new);
            //handleDir((void*)new);
        // is a regular file
        } else if (S_ISREG(path_stat.st_mode)) {
            pthread_t thread_id;
            pthread_mutex_lock(new->lock);
            new->phead = addThread(new->phead, &thread_id);
            pthread_mutex_unlock(new->lock);

            printf("goin into file %s\n", new->name);
            //pthread_create(&thread_id, NULL, handleFile, new);
        }
    }    
    closedir(dir);
    free(input->name);
    free(input);
    pthread_exit(NULL);
}

void joinThread(pthreads* phead){
    while(phead->next != NULL){
        pthread_join(*(phead->thread_id), NULL);
    }
}





int main(int argc, char **argv){
    
    // Read in directory and error if invalid number of arguments
    if(argc != 2){
        printf("Invalid number of arguments");
        return -1;
    }
    char *dirname = argv[1];
    if(validDir(dirname) == -1){
        printf("Invalid folder\n");
        exit(0);
    }
    
    tInput* input = (tInput*)malloc(sizeof(tInput)); 
    pthreads* phead = NULL;
    
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    struct FileNode* front = NULL;

    input->front = front;
    input->lock = &mutex;
    input->name = dirname;
    input->phead = phead; 
    //printf("%s\n",input->name);
    pthread_t thread_id;
    input->phead = addThread(input->phead, &thread_id);
    pthread_create(&thread_id, NULL, handleDir, (void*)input);
    //printThread(phead);
    
    
    
    joinThread(phead);
    //free(input);
    printf("the n word");
}