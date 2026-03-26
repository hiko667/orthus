#define _DEFAULT_SOURCE // need this cause i use arch btw
#include "utils.h"								//  I
#include <stdio.h>        						//  I
#include <dirent.h>								//  I
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

bool is_dir(const char *path) 					//  I
{												//  I
	struct stat st;								//  [------]
	return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);//I
}														 //I
int countFiles(const char * path)						// I
{													 //    I
	int count = 0;									 // FOR THIS
	struct dirent * entry;							 //    I
	DIR * dir = opendir(path);						 //    I	
	if(dir == NULL) return -1;						 //   \ /
	while ((entry = readdir(dir)) != NULL)           //    V
	{
        if (entry->d_name[0] == '.' || entry->d_type != DT_REG) continue;
        count++;
    }
	closedir(dir);
	return count;
}

int daemonize_process() {
	pid_t pid;
	int fd;

	pid=fork();

	if (pid < 0) {
		return -1;
	} else if (pid > 0) {
		exit(0);
	}

	if (setsid()<0) {
		return -1;
	}

	pid = fork();
	if (pid < 0) {
		return -1;
	} else if (pid > 0) {
		exit(0);
	}

	umask(0);

	if (chdir("/")<0) {
		return -1;
	}

	int x;
	for (x=sysconf(_SC_OPEN_MAX); x>=0; x--) close(x);

	fd = open("/dev/null", O_RDWR);
	if (fd <0) {
		return -1;
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	if (fd > 2) close(fd);

	return 0;
}