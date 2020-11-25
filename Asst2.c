#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h> 
#include <dirent.h>
#include <errno.h>

struct arg_struct
{
   int* arg1;
   pthread_mutex_t* arg2;
};

void* printTen(void* arg){
    struct arg_struct *params = arg;
    pthread_mutex_t* lock = params->arg2;
    void* pointer = params->arg1;
    pthread_mutex_lock(lock);
    int* x = (int*)pointer;
    for(int i = 1; i<= 10;i++){
        printf("%d %d\n",*x, i);
    }
    pthread_mutex_unlock(lock);
}
int validDir(DIR* dir){
    if (dir) {
        closedir(dir);
        return 1;
    } else return -1;
}
handleDir(DIR* dir){
    opendir(dir);
    readdir(dir);
}
int main(int argc, char **argv){
    // Read in directory and error if invalid
    if(argc != 2){
        printf("Invalid number of arguments");
        return -1;
    }
    char *dirname = argv[1];
    DIR* dir = opendir(dirname);
    int valid = 0;
    valid = validDir(dir);
    if(valid == -1){
        printf("Invalid folder");
        return -1;
    }
    handleDir(dir);
    
    pthread_t thread_id[5];
    pthread_mutex_t* lock;
    pthread_mutex_init(lock,NULL);
    struct arg_struct* args = malloc(5*sizeof(struct arg_struct));
    int* p = (int*)malloc(5*sizeof(int));
    for(int i = 0; i < 5; i++){
        (args + i)->arg2 = lock;
        (args + i)->arg1 = p + i;
        *(p+i) = i+1;
        pthread_create(&thread_id[i], NULL, printTen, (void *)(args + i));
    }
    for(int i = 0 ; i< 5; i++){
        pthread_join(thread_id[i], NULL);
    }
    free(args);
    free(p);
    pthread_mutex_destroy(lock);
    exit(0);
}
