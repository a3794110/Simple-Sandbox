#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int used_by_main = 0;

int allowed_dir_check(const char* file){
    const char* dir_allowed = getenv("DIR_ALLOWED");
    const char* dir_here = getenv("HERE");
    char* absolute_path_file = (char*)malloc(sizeof(char*) * 200);
    realpath(file, absolute_path_file);
    if (dir_allowed == NULL){ //defaut path check
        if (strncmp(absolute_path_file, dir_here, strlen(dir_here) )==0){            
            return 0;
        }
        else{ 
            //printf("not allowed\n");
            errno = EACCES;
            return 1;
        }
    }
    else { 
        char* absolute_path_dir_allowed = (char*)malloc(sizeof(char*) * 200);
        realpath(dir_allowed, absolute_path_dir_allowed);
        if (strncmp(absolute_path_file, dir_here, strlen(dir_here) )==0)
            return 0;
        else if (strncmp(absolute_path_file, absolute_path_dir_allowed, strlen(absolute_path_dir_allowed) )==0)
            return 0;
        else{ 
            //printf("not allowed\n");
            errno = EACCES;
            return 1;
        }
    }
    //return 1: not allowed; 0: allowed
}

// ##absolute block##
int execvp(const char *file, char *const argv[]){
    //printf("execvp\n");

    if (used_by_main == 0){
        static int (*original_function ) (const char *, char *const []) = NULL; 

        if (!original_function)
        {
            original_function = (int(*)(const char *, char *const [])) dlsym(RTLD_NEXT, "execvp");
        }

        used_by_main++;

        return original_function(file, argv);
    }
    else{
        printf("[sandbox] execvp(%s): not allowed\n", file);
        errno = EACCES;
        return -1;
    }
}

int execve(const char *filename, char *const argv[], char *const envp[]){

    printf("[sandbox] execve(%s): not allowed\n", filename);
    errno = EACCES; 
    return -1;
}

int execl(const char *path, const char *arg, ...){
    printf("[sandbox] execl(%s): not allowed\n", path);
    errno = EACCES;
    return -1;    
}

int execle(const char *path, const char *arg, ...){

    printf("[sandbox] execle(%s): not allowed\n", path);
    errno = EACCES;
    return -1;    
}

int execlp(const char *file, const char *arg, ...){
    printf("[sandbox] execlp(%s): not allowed\n", file);
    errno = EACCES;
    return -1;     
}

int execv(const char *path, char *const argv[]){
    printf("[sandbox] execv(%s): not allowed\n", path);
    errno = EACCES;
    return -1;    
}
int system(const char *command){
    printf("[sandbox] system(%s): not allowed\n", command);
    errno = EACCES;
    return -1;
}

// ##absolute block##


//## monitored functions##
DIR *opendir(const char *name){
    //printf("opendir\n");
    static DIR* (*original_function ) (const char *) = NULL;

    if(allowed_dir_check(name) == 1){
        printf("[sandbox] opendir: access to %s is not allowed\n", name);
        errno = EACCES;
        return 0;
    }
    else{
        if (!original_function)
        {
            original_function = ( DIR*(*)(const char *)) dlsym(RTLD_NEXT, "opendir");
        }

        return original_function(name);
    }
}


int open(const char *pathname, int flags, ...)
{
    typedef int (*orig_open_f_type)(const char *pathname, int flags);
    //printf("open\n");
    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] open: access to %s is not allowed\n", pathname);
        errno = EACCES;
        return 0;
    }
    else {
        orig_open_f_type orig_open;
        orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
        return orig_open(pathname,flags);
    }
}


int __xstat(int ver, const char * path, struct stat * stat_buf){
    //printf("__xstat\n");
    static int (*original_function ) (int ver, const char *path, struct stat *stat_buf) = NULL;

    if(allowed_dir_check(path) == 1){
        printf("[sandbox] __xstat: access to %s is not allowed\n", path);
        errno = EACCES;
        return 0;
    }
    else{
        if (!original_function)
        {   //printf("success\n");
            original_function = ( int(*)(int, const char *, struct stat*)) dlsym(RTLD_NEXT, "__xstat");
        }

        return original_function(ver, path, stat_buf);
    }
}

FILE *fopen(const char *path, const char *mode){
    //printf("fopen\n");
    static FILE* (*original_function ) (const char *path, const char *mode) = NULL;

    if(allowed_dir_check(path) == 1){
        printf("[sandbox] fopen: access to %s is not allowed\n", path);
        errno = EACCES;
        return 0;
    }
    else{
        if (!original_function)
        {
            original_function = ( FILE*(*)(const char *, const char*)) dlsym(RTLD_NEXT, "fopen");
        }

        return original_function(path, mode);
    }
}

int chdir(const char *path){
    //printf("chdir\n");
    static int (*original_function ) (const char *path) = NULL;
    //printf("chdir\n");
    if(allowed_dir_check(path) == 1){
        printf("[sandbox] chdir: access to %s is not allowed\n", path);
        errno = EACCES;
        return 0;
    }
    else{
        if (!original_function)
        {   //printf("chdir\n");
            original_function = ( int(*)(const char *)) dlsym(RTLD_NEXT, "chdir");
        }

        return original_function(path);
    }
}

int chmod(const char *pathname, mode_t mode){
//    printf("chmod\n");
    static int (*original_function ) (const char *path, mode_t mode) = NULL;
    //printf("chmod\n");

    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] chmod: access to %s is not allowed\n", pathname);
        errno = EACCES;
        return 0;
    }
    else{
        if (!original_function)
        {
            original_function = ( int(*)(const char *, mode_t)) dlsym(RTLD_NEXT, "chmod");
        }

        return original_function(pathname, mode);
    }
}
int chown(const char *pathname, uid_t owner, gid_t group){
    //printf("chown\n");
    static int (*original_function ) (const char *pathname, uid_t owner, gid_t group) = NULL;

    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] chown: access to %s is not allowed\n", pathname);
        errno = EACCES;
        return 0;
    }
    else{
        if (!original_function)
        {
            original_function = ( int(*)(const char *, uid_t, gid_t)) dlsym(RTLD_NEXT, "chown");
        }

        return original_function(pathname, owner, group);
    }
}

int creat(const char *pathname, mode_t mode){
    //printf("creat\n");
    static int (*original_function ) (const char *pathname, mode_t mode) = NULL;

    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] creat: access to %s is not allowed\n", pathname);
        errno = EACCES;
        return 0;
    }
    else{
        if (!original_function)
        {
            original_function = ( int(*)(const char *, mode_t)) dlsym(RTLD_NEXT, "creat");
        }

        return original_function(pathname, mode);
    }
}

int link(const char *oldpath, const char *newpath){
    //printf("link\n");
    static int (*original_function )( const char *oldpath, const char *newpath) = NULL;

    if(allowed_dir_check(oldpath) == 1){
        printf("[sandbox] link: access to %s is not allowed\n", oldpath);
        return 0;
    }
    else if(allowed_dir_check(newpath) == 1){
        printf("[sandbox] link: access to %s is not allowed\n", newpath);
        return 0;
    }
    else{
        if (!original_function)
        {
            original_function = ( int(*)(const char *, const char*)) dlsym(RTLD_NEXT, "link");
        }

        return original_function(oldpath, newpath);
    }
}

int mkdir(const char *pathname, mode_t mode){
    //printf("mkdir\n");
    static int (*original_function ) (const char *pathname, mode_t mode) = NULL;

    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] creat: access to %s is not allowed\n", pathname);
        return 0;
    }
    else{
        if (!original_function)
        {
            original_function = ( int(*)(const char *, mode_t)) dlsym(RTLD_NEXT, "mkdir");
        }

        return original_function(pathname, mode);
    }
}

int openat(int dirfd, const char *pathname, int flags, ...)
{
    typedef int (*orig_open_f_type)(int dirfd, const char *pathname, int flags);
    //printf("openat\n");
    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] openat: access to %s is not allowed\n", pathname);
        return 0;
    }
    else {
        orig_open_f_type orig_open;
        orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"openat");
        return orig_open(dirfd, pathname,flags);
    }
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
    typedef ssize_t (*orig_open_f_type)(const char *pathname, char *buf, size_t bufsiz);
    //printf("readlink\n");
    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] readlink: access to %s is not allowed\n", pathname);
        return 0;
    }
    else {
        orig_open_f_type orig_open;
        orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"readlink");
        return orig_open(pathname, buf, bufsiz);
    }
}

int remove(const char *pathname)
{
    typedef int (*orig_open_f_type)(const char *pathname );
    //printf("remove\n");
    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] remove: access to %s is not allowed\n", pathname);
        return 0;
    }
    else {
        orig_open_f_type orig_open;
        orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"remove");
        return orig_open( pathname );
    }
}
int rename(const char *oldpath, const char *newpath){
    //printf("rename\n");
    static int (*original_function )( const char *oldpath, const char *newpath) = NULL;

    if(allowed_dir_check(oldpath) == 1){
        printf("[sandbox] rename: access to %s is not allowed\n", oldpath);
        return 0;
    }
    else if(allowed_dir_check(newpath) == 1){
        printf("[sandbox] rename: access to %s is not allowed\n", newpath);
        return 0;
    }
    else{
        if (!original_function)
        {
            original_function = ( int(*)(const char *, const char*)) dlsym(RTLD_NEXT, "rename");
        }

        return original_function(oldpath, newpath);
    }
}

int rmdir(const char *pathname)
{
    typedef int (*orig_open_f_type)(const char *pathname );
    //printf("rmdir\n");
    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] rmdir: access to %s is not allowed\n", pathname);
        return 0;
    }
    else {
        orig_open_f_type orig_open;
        orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"rmdir");
        return orig_open( pathname );
    }
}

int symlink(const char *target, const char *linkpath)
{
    typedef int (*orig_open_f_type)(const char *target, const char *linkpath);
    //printf("sympath\n");
    if(allowed_dir_check(linkpath) == 1){
        printf("[sandbox] symlink: access to %s is not allowed\n", linkpath);
        return 0;
    }
    else {
        orig_open_f_type orig_open;
        orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"symlink");
        return orig_open( target, linkpath );
    }
}

int unlink(const char *pathname)
{
    typedef int (*orig_open_f_type)(const char *pathname );
    //printf("unlink\n");
    if(allowed_dir_check(pathname) == 1){
        printf("[sandbox] unlink: access to %s is not allowed\n", pathname);
        return 0;
    }
    else {
        orig_open_f_type orig_open;
        orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"unlink");
        return orig_open( pathname );
    }
}
//## monitored functions##
