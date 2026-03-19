#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int is_dir(const char *path) {
	struct stat st;
	return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}

void daemonize() {

}

int main(int argc, char * argv[])
{
	if (argc < 3) {
		printf("Please provide two paths to directories you wish to synchronize. \n");
		return 1;
	}

	if (!is_dir(argv[1]) || !is_dir(argv[2])) {
		printf("Please provide valid paths to directories. \n");
		return 1;
	}


    return 0;
}