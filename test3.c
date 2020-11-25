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


/*
struct token_struct{

}token;

struct folder_struct{

    struct folder_struct next;
}folder;
*/
int analyzeFiles(){

}
int initialize(){

}

int validDir(char* dirname){
    DIR* dir;
    if ((dir = opendir(dirname)) == NULL) {
        printf("Cannot open %s", dirname);
        return -1;
    }
     else return 1;
}

int handleDir(char* dirname){
    if(validDir(dirname) == -1){
        return -1;
    }
    DIR* dir = opendir(dirname);
    struct dirent *dp;
    struct stat path_stat;
    
    while((dp = readdir(dir))){
        printf("%s\n",dp->d_name);
        stat(dp->d_name, &path_stat);
        // is a directory
        if (S_ISDIR(path_stat.st_mode) && strcmp(dp->d_name, ".")!=0 && strcmp(dp->d_name, "..")!=0){
            // new thread
            char str[80];
            strcpy(str,dirname);
            strcat(str,"/");
            strcat(str,dp->d_name);
            printf("%s",str);

            handleDir(str);
            
        // is a regular file
        } else if (S_ISREG(path_stat.st_mode)) {

        }
    }
    closedir(dir);
}


int main(int argc, char **argv){
    // Read in directory and error if invalid number of arguments
    if(argc != 2){
        printf("Invalid number of arguments");
        return -1;
    }
    char *dirname = argv[1];
    DIR* dir = opendir(dirname);
    if(validDir(dirname) == -1){
        printf("Invalid folder");
        return -1;
    }
    closedir(dir);
    initialize();
    handleDir(dirname);
    analyzeFiles();

}