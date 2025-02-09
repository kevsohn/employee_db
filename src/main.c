#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -f <db path> -n\n", argv[0]);
	printf("\t -f: (required) path to database file\n");
	printf("\t -n: create new database file\n");
}

/* 
-a: add
-u: update
-d: delete
-f: filepath
-n: newfile
*/
int main(int argc, char *argv[]) {
	char *fpath = NULL;
	bool newfile = 0;
	bool add = 0;
	bool update = 0;
	bool delete = 0;
	bool debug = 0;

	int op;
	while ((op = getopt(argc, argv, "nf:a:u:d:")) != -1) {
		switch(op) {
			case 'f':
				fpath = optarg;
				break;
			case 'n':
				newfile = true;
				break;
			case 'a':
				add = true;
				break;
			case 'u':
				update = true;
				break;
			case 'd':
				delete = true;
				break;
			case 'g';
				debug = true;
				break;
			case '?':
				printf("Unknown option -%c\n", op);
				break;
			default: 
				return STATUS_ERROR;
		}
	}
	
	if (fpath == NULL) {
		printf("File path is a required argument\n");
		print_usage(argv);
		return STATUS_ERROR;
	}

	if (debug) {
		printf("debug info:\n");
		printf("\t fpath = %s\n", fpath);
		printf("\t newfile = %d\n", newfile);
		printf("\t add = %d\n", add);
		printf("\t update = %d\n", update);
		printf("\t delete = %d\n", delete);
	}

	int fd = -1;
	if (newfile) 
		fd = create_db_file(fpath);
	else 
		fd = open_db_file(fpath);
	if (fd == -1) return STATUS_ERROR;

	close(fd);
	return STATUS_SUCCESS;
}
