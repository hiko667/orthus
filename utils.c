#include "utils.h"

bool is_dir(const char *path) 
{
	struct stat st;
	return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}
