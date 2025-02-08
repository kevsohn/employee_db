#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	
}

/* 
-a: add
-u: update
-d: delete
-f: filename
-n: new file
*/
int main(int argc, char *argv[]) {
	int op = 0;
	while (op = getopt(argc, argv, "nf:") != -1) {
		switch(op) {
			case 'n':
				
				break;
			case 'f':
				
				break;
			case 'a':
				break;
			case 'u':
				break;
			case 'd':
				break;
		}
	}
}
