#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]){
	char * args[argc - 1];
	for (int i=1;i<argc;i++) args[i-1] = argv[i];
	if (argc <= 1) exit();
	int f = fork();
	if (f) {
		wait(); 
		exit();
	}
	else {
		exec(args[0],args);
	}
}