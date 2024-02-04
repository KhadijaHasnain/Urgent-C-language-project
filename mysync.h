#ifndef MYSYNC_H
#define MYSYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
char *glob2regex(char *glob);

#define MAX_FILENAME_LENGTH 255

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    time_t modification_time;
} File;

void process_directory(const char *dirname);
void synchronize_files(const char *src, const char *dest);

#endif // MYSYNC_H
