#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "modern_types.h"

typedef FILE File;

#define FILE_MODE_READ "r"
#define FILE_MODE_WRITE "w"
#define FILE_MODE_READ_BINARY "rb"
#define FILE_MODE_WRITE_BINARY "wb"
#define FILE_MODE_APPEND "a"
#define FILE_MODE_APPEND_BINARY "ab"
#define FILE_MODE_READ_WRITE "r+"
#define FILE_MODE_READ_WRITE_BINARY "rb+"
#define FILE_MODE_WRITE_READ "w+"
#define FILE_MODE_WRITE_READ_BINARY "wb+"
#define FILE_MODE_APPEND_READ "a+"
#define FILE_MODE_APPEND_READ_BINARY "ab+"


static inline FILE* file_open(const char* filename, const char* mode) {
    return fopen(filename, mode);
}

static inline char* file_get_line(char* str, int num, File* file) {
    return fgets(str, num, file);
}

static inline int file_get_char(File* file) {
    return fgetc(file);
}

static inline size_t file_read(void* ptr, size_t size, size_t count, File* file) {
    return fread(ptr, size, count, file);
}

static inline int file_seek(File* file, long offset, int origin) {
    return fseek(file, offset, origin);
}

static inline void file_write_str(File* file, const char* str){
    fputs(str,file);
}

static inline void file_close(File* file){
    fclose(file);
}


u64 file_get_size(File* file){
    u64 size;
    fseek(file,0,SEEK_END);
    size = ftell(file);
    rewind(file);
    return size;
}


char* file_get_contents(File* file){
    if (file == NULL) {
        return NULL; }

    u64 size = file_get_size(file);
    if (size == 0) {
        return NULL; 
    }

    char* str = (char*)malloc(size + 1);
    if (str == NULL) {
        return NULL; 
    }

    if (file_read(str, size, 1, file) != 1) {
        free(str); 
        return NULL;
    }

    str[size] = '\0';
    return str;
}