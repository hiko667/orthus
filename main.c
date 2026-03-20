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
bool setSource(struct configStruct * configurations, const char * path)
{
	if(!is_dir(path)) return false;
	strcpy(configurations->sourceDir, path);
}
bool setTarget(struct configStruct * configurations, const char * path)
{
	if(!is_dir(path)) return false;
	strcpy(configurations->targetDir, path);
}
void giveHelp()
{

}
bool readArguments(struct configStruct * configurations, int argc, char * argv[])
{
	int i = 1;
	struct configStruct configurations;
	while (i < argc)
	{
		switch (argv[i][1])
		{
		case 's' : if(!setSource(&configurations, argv[i+1]) || strlen(argv[i]) > 2) 
			return false; i += 2; break;
		case 't' : 
			if(!setTarget(&configurations, argv[i+1]) || strlen(argv[i]) > 2)
				return false; 
			i += 2; break;
		case 'h' : giveHelp(); return false;
		case 'R' : 
			if(strlen(argv[i]) > 2);
				return false;
			configurations->recursive = true; i ++; break;
		case 'm':;
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