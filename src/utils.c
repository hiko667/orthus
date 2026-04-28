#define _DEFAULT_SOURCE
#include "utils.h"
#include "logs.h"
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <utime.h>
#include <sys/mman.h>

char *BuildPath(const char *dir, const char *name)
{
	size_t len = strlen(dir) + strlen(name) + 2;
	char *path = malloc(len);
	if (!path)
		return NULL;

	snprintf(path, len, "%s/%s", dir, name);
	return path;
}

bool IsDir(const char *path) 
{							
	struct stat st;	
	return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}				

int CountFiles(const char * path)
{	
	int count = 0;
	struct dirent * entry;
	DIR * dir = opendir(path);
	if(dir == NULL) return -1;
	while((entry = readdir(dir)) != NULL)
	{
		if (entry->d_name[0] == '.' || entry->d_type != DT_REG) continue;
		count++;
	}
	closedir(dir);
	return count;
}

bool CopyFile(const char *srcPath, const char *dstPath, long long minSizeToBeBig)
{
	int srcFd = open(srcPath, O_RDONLY);
	if (srcFd < 0)
		return false;

	int dstFd = open(dstPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dstFd < 0)
	{
		close(srcFd);
		return false;
	}

	struct stat st;
	if (fstat(srcFd, &st) != 0)
	{ 
		close(srcFd); 
		return false; 
	}

	// Check if the source file classifies as big
	bool isFileBig = (st.st_size >= minSizeToBeBig);

	if (isFileBig && st.st_size > 0)
	{
		void *srcMap = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);

		if (srcMap == MAP_FAILED)
		{
			close(srcFd);
			close(dstFd);
			return false; 
		}

		if (write(dstFd, srcMap, st.st_size) != st.st_size)
		{
			munmap(srcMap, st.st_size);
			close(srcFd);
			close(dstFd);
			return false;
		}

		munmap(srcMap, st.st_size);
	}
	else
	{
		char buffer[8192];
		ssize_t bytesRead;
		// Read and write until everything is written
		while ((bytesRead = read(srcFd, buffer, sizeof(buffer))) > 0)
		{
			ssize_t written = 0;
			while (written < bytesRead)
			{
				ssize_t w = write(dstFd, buffer + written, bytesRead - written);
				if (w < 0)
				{
					close(srcFd);
					close(dstFd);
					return false;
				}
				written += w;
			}
		}

		if (bytesRead < 0)
		{
			close(srcFd);
			close(dstFd);
			return false;
		}
	}

	// Set the target file's modification date
	struct utimbuf times;
	times.actime = st.st_atime;
	times.modtime = st.st_mtime;
	utime(dstPath, &times);

	close(srcFd);
	close(dstFd);
	return true;
}

bool RemovePath(const char *path)
{
	return unlink(path) == 0;
}

int DaemonizeProcess()
{
	pid_t pid;
	int fd;
	
	// Allow parent to exit so the child can continue in the background
	pid = fork();

	if (pid < 0)
		return -1;
	else if (pid > 0)
		exit(0);

	// Create a new session and detach from controlling terminal
	if (setsid() < 0)
		return -1;

	// Prevent deamon from getting the terminal again
	pid = fork();
	if (pid < 0)
		return -1;
	else if (pid > 0)
		exit(0);

	umask(0);

	// Change working directory to root
	if (chdir("/") < 0)
		return -1;

	// Close all inherited file descriptors
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
		close(x);

	// Redirect standard input/output/error to /dev/null
	fd = open("/dev/null", O_RDWR);
	if (fd < 0)
		return -1;

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	// Close extra descriptor if not stdin/stdout/stderr
	if (fd > 2)
		close(fd);

	return 0;
}