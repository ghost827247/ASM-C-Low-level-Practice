#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define buffer_size 50000


void print_perms(mode_t mode) {
	// Check if Directory
	printf((mode & S_IFDIR) ? "d" : "-");

	// Check Owner Permissions
	printf((mode & S_IRUSR) ? "r" : "-");
	printf((mode & S_IWUSR) ? "w" : "-");
	printf((mode & S_IXUSR) ? "x" : "-");
	// Check Group Permissions
	printf((mode & S_IRGRP) ? "r" : "-");
	printf((mode & S_IWGRP) ? "w" : "-");
	printf((mode & S_IXGRP) ? "x" : "-");
	// Check Other Perms
	printf((mode & S_IROTH) ? "r" : "-");
	printf((mode & S_IWOTH) ? "w" : "-");
	printf((mode & S_IXOTH) ? "x" : "-");
	printf("%-5s", " ");

}

void print_help() {
	printf("USAGE: yelles <directory> <args>\n");
	printf("-v Verbose Mode, Show All Files Including Hidden Ones\n");
	printf("If Directory Arg Is Empty, Will Default to Current Directory(.)\n");
}

// Struct To Hold Filenames, Amount Of Bytes Ect
struct linux_dirent64 {
    uint64_t d_ino;    // inode number
    int64_t d_off;     // offset to the next dirent
    unsigned short d_reclen; // length of this record
    unsigned char d_type;    // file type
    char d_name[];     // filename (variable length)
};


int main(int argc, char *argv[])
{
	// Buffer To Hold Recevied Data From dirents64
	char buffer[buffer_size];
	char* name = ".";
	int verbose = 1;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
	    print_help();
	    return 0;
	}

	// Quickly Parse and Check Args
	if (argc == 2) {
		// Check If Verbose
	    if (strcmp(argv[1], "-v") == 0) {
	        verbose = 0; // Enable verbose mode
	    } else {
	        name = argv[1]; 
	    }
	} else if (argc == 3 && strcmp(argv[1], "-v") == 0) {
	    verbose = 0; 
	    name = argv[2]; 
	} else if (argc == 3 && strcmp(argv[2], "-v") == 0) {
	    verbose = 0; // Enable verbose mode
	    name = argv[1]; // Set the directory name
	} else if (argc > 3) {
	    printf("Too many arguments.\n");
	    return 1;
	}

	// Open File Location, If respones is -1 it means we Failed
	int fd = open(name, O_RDONLY | O_DIRECTORY);
	if (fd == -1) {
		printf("Failed Opening Directory, Does It Exist?");
		return 1;
	}

	// Make Syscall To Run getdents64 and store it into the buffer
	ssize_t amount = syscall(SYS_getdents64, fd, &buffer, sizeof(buffer));
	printf("[*] Bytes Read: %d\n", amount);

	// Create a pointer to point to start of buffer
	char* p = buffer;

	// Loop Through All Data In Buffer
	while(p < buffer + amount) {
		// Cast To Buffer to the Dirents struct and make it point the buffer 
		struct linux_dirent64 *drent = (struct linux_dirent64 *)p;

		if (verbose)  {
			if (drent->d_name[0] == '.') {
				p += drent->d_reclen;
				continue;
			}
		}
		
		// check if the filename is .(current dir) or  ..(one dir back) and skip it
		if (strcmp(drent->d_name, "..") == 0 || strcmp(drent->d_name, ".") == 0) {
			p += drent->d_reclen;
			continue;
		}

		char* type;
		// Check If Its a File, dir ect
		switch(drent->d_type) {
			case DT_REG: type = "file"; break;
			case DT_DIR: type = "dir"; break;
			case DT_LNK: type = "Sym_Link"; break;
			default: type = "other"; break;
		}

		// If its a File, We Check the Size Of It
		
		char buf[5000];
		int f;
		// Create Full Path
		snprintf(buf, sizeof(buf), "%s/%s", name, drent->d_name);
			// Open The File Read only Mode
		if (drent->d_type == DT_REG) {
			f = open(buf, O_RDONLY);
		}

		else if (drent->d_type == DT_DIR) {
			f = open(buf, O_RDONLY | O_DIRECTORY);
		}
		

		// Declare Our Struct That fstat() Will Populate
		struct stat st;
		// Run fstat on the file
		fstat(f, &st);
		print_perms(st.st_mode);

		// Declare passwd and group structs to populate after checking pwd folder
		struct passwd *pwd;
		struct group *gid;

		pwd = getpwuid(st.st_uid);
		gid = getgrgid(st.st_gid);


		// Print the File
		//printf("%s:%-15s %-10s %-6lld %-12s \n", pwd->pw_name, gid->gr_name, type, (long long)st.st_size, drent->d_name);
		printf("%-10s %-10s %-10s %-10lld %-s\n", pwd->pw_name, gid->gr_name, type, (long long)st.st_size, drent->d_name);



		
		// else {
		// 	printf("%-30s %-6s\n", drent->d_name, type);
		// }

		// Move To The Next Value in the buffer
		p += drent->d_reclen;
	}

	return 0;
}
