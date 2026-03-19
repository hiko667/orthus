#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int is_dir(const char *path) {
	struct stat st;
	return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}

void daemonize() {

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