//touch.c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[512];

int
main(int argc, char *argv[]){
	int file;

	//Too few arguments
	if(argc < 2){
		printf(1, "touch file1 file2 ... filen");
		exit();
	}
	else{
		for(int i = 1; i < argc; i++){
			if((file = open(argv[i], O_CREATE|O_RDWR)) < 0){
				printf(1, "touch: can't open %s\n", *argv[i]);
				exit();
			}
			else{
				close(file);
			}
		}
	}
	exit();
}
