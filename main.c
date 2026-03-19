#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "config_struct.h"

bool is_dir(const char *path) {
	struct stat st;
	return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}

void daemonize() {

}

enum options recognizeArgumentType(char * arg)
{
	if(strcmp(arg, "-s") == 0) return SOURCE;
	else if (strcmp(arg, "-t")) return TARGET;
	else if (strcmp(arg, "-f")) return FREQUENCY;
	else if (strcmp(arg, "-R")) return RECURSIVE;
	else if (strcmp(arg, "-m")) return MINSIZE;
	else return ERROR;
}
struct configStruct readArguments(int argc, char * argv[])
{
	int i = 1;
	struct configStruct configurations;
	while (i < argc)
	{
		switch (recognizeArgumentType(argv[i]))
		{
		case SOURCE: 
			if(is_dir(argv[i+1])) strcpy(argv[i+1], configurations.sourceDir);
			else ;
			break;
		case TARGET:
			break;
		default:
			break;
		}
	}
	
}
/*
Potrzebujemy następujących funkcjonalności określanych argumentami funckcji main()
- target directory
- src drectory
- częstotliwośc wybudzania
- opcje --help; bo tak i to cool; opcjonalnie flagę -h
- ten -R, aka żeby robić rekursywnie
- argument do granicy rozdzielającej pliki
proponowana struktura wywołania
orthus -s ./source -t ./target -f 5 -R -m 5000
gdzie po -s następuje źródło, po -t cel, po -f co ile minut wybudzać
po -R nic, po -m ile bajtów to granica

*/
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