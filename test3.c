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


void addThread(pthreads **head, pthread_t* new){    
    pthreads* ref = *head;
    pthreads* newThread = (pthreads*)malloc(sizeof(pthreads));
    newThread->thread_id = new;
    newThread->next = NULL;
    int counter = 0;
    if(*head == NULL){
        *head = newThread;
        printf("    1st threads added\n");
        return;
    } 
    while(ref->next != NULL){
        counter+=1;
        ref = ref->next;
    }
    ref->next = newThread;
    printf("    %dth threads added\n",counter+2);
    return;
}

 #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

void* joinThread(pthreads** phead){
    //printf("piss\n");
    // if(phead == NULL){
    //     printf("fuuuuuuuu");
    // }
    int count = 0;
    //printf("|||%d|||\n",count);
    pthreads* ref = *phead;
    pthreads* prev;
    while(ref != NULL){
        prev = ref;
        count+=1;
        pthread_t* thread_id = ref->thread_id;
        int a = pthread_join(*thread_id, NULL);
        if(a!=0){
            handle_error_en(a, "pthread_join");
        }
        ref = ref->next;
        free(prev);
    }

    //printf("|||%d|||\n",count);
}


void printThread(pthreads **head){
    printf("counting threads\n");
    int count = 0;
    pthreads* ref = *head;
    while(ref != NULL){
        count +=1;
        ref = ref->next;
    }
    printf("count is %d\n",count);
}

int validDir(char* dirname){
    DIR* dir;
    if ((dir = opendir(dirname)) == NULL) {
        printf("Cannot open %s\n", dirname);
        return -1;
    }
    
     else {
     closedir(dir);
     return 1;
     }
}

void* handleDir(void *vinput){
    printf("                   thread id is %d\n", pthread_self());
    tInput *input = vinput;
    char* dirname = input->name;
    printf("File path: %s\n",input->name);
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
        tInput* new = (tInput*)malloc(sizeof(tInput));
        int size = strlen(dirname) + strlen(dp->d_name) + 2;
        new->name = (char*)malloc(size*sizeof(char));
        for(int i = 0; i<size-1; i++){
            if(i<strlen(dirname)){
                new->name[i] = dirname[i];
            } else if(i == strlen(dirname)){
                new->name[i] = '/';
            } else {
                new->name[i] = (dp->d_name)[i-strlen(dirname)-1];
            }
        } new->name[size-1] = '\0';

        stat(new->name, &path_stat);
        printf("((%s))--------------------------------------------------\n",new->name);
        
        new->phead = input->phead;
        new->front = input->front;
        new->lock = input->lock;
        
        pthread_t* thread_id;
        // is a directory
        if (S_ISDIR(path_stat.st_mode)){
            pthread_mutex_lock(new->lock);
            addThread(&(new->phead), thread_id);
            pthread_mutex_unlock(new->lock);
            pthread_create(thread_id, NULL, handleDir, (void*)new);
        // is a regular file
        } else if (S_ISREG(path_stat.st_mode)) {
            
            pthread_mutex_lock(new->lock);
            addThread(&(new->phead), thread_id);
            pthread_mutex_unlock(new->lock);

            printf("goin into file %s\n", new->name);
            //pthread_create(&thread_id, NULL, handleFile, new);
        }
    }
    free(input->name);
    free(input);
    closedir(dir);
    pthread_exit(NULL);
}




int main(int argc, char **argv){

    // Read in directory and error if invalid number of arguments
    if(argc != 2){
//        printf("Invalid number of arguments");
        return -1;
    }
    char *dirname = argv[1];
    if(validDir(dirname) == -1){
//        printf("Invalid folder\n");
        exit(0);
    }

    tInput* input = (tInput*)malloc(sizeof(tInput));
    input->name = (char*)malloc(sizeof(char)*(strlen(dirname)));
    pthreads* phead = NULL;

    pthread_mutex_t* mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_t mutexe = PTHREAD_MUTEX_INITIALIZER;
    *mutex = mutexe;
    struct FileNode* front = NULL;

    input->front = front;
    input->lock = mutex;
    strcpy(input->name, dirname);
    input->phead = phead;
    //printf("oooooooo\n");
    //printf("%s\n",input->name);
    pthread_t* thread_id = malloc(sizeof(pthread_t));
    pthread_mutex_lock(input->lock);

    addThread(&(input->phead), thread_id);

    pthread_mutex_unlock(input->lock);
    //printThread(&(input->phead));

    pthread_create(thread_id, NULL, handleDir, (void*)input);
    //printThread(phead);
    
    //joinThread(&(input->phead));

    
    //printf("|||%d|||\n",count);
    pthreads* ref = input->phead;
    pthreads* prev;
    while(ref != NULL){
        prev = ref;
        pthread_t* thread_id = ref->thread_id;
        int a = pthread_join(*thread_id, NULL);
        if(a!=0){
            handle_error_en(a, "pthread_join");
        }
        ref = ref->next;
        free(prev);
    }
    
    pthread_mutex_destroy(input->lock);
    
    //free(input);
}