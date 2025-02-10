#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -f <db_file> -n\n", argv[0]);
	printf("\t -f: (required) path to database file\n");
	printf("\t -n: create new database file\n");
	printf("\t -a: add database entry\n");
	printf("\t -u: update database entry\n");
	printf("\t -d: delete database entry\n");
	printf("\t -g: show debug info\n");
}

int main(int argc, char *argv[]) {
	char *fpath = NULL;
	bool newfile = 0;
	bool add = 0;
	bool update = 0;
	bool delete = 0;
	bool debug = 1;

	int op;
	while ((op = getopt(argc, argv, "f:na:u:d:g")) != -1) {
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
			case 'g':
				debug = true;
				break;
			case '?':
				printf("Unknown option -%c\n", op);
				break;
			default: 
				return -1;
		}
	}
	
	if (debug) {
		printf("debug info:\n");
		printf("\t fpath = %s\n", fpath);
		printf("\t newfile = %d\n", newfile);
		printf("\t add = %d\n", add);
		printf("\t update = %d\n", update);
		printf("\t delete = %d\n", delete);
	}

	if (fpath == NULL) {
		printf("File path is a required argument\n");
		print_usage(argv);
		return 0;
	}

    struct header_t *header;
    struct employee_t *employees;
	int fd = -1;
	if (newfile) {
		fd = create_db_file(fpath); 
		if (fd == STATUS_ERROR) {
			printf("Unable to create db file\n");
			return -1;
		}
        if (create_db_header(&header) == STATUS_ERROR) {
            printf("Failed to create db header\n");
            close(fd);
            return -1;
        }
        if (create_employee_list(header, &employees) == STATUS_ERROR) {
            printf("Failed to create employee list\n");
            close(fd);
            free(header);
            return -1;
        }
	}else {
		fd = open_db_file(fpath);
		if (fd == STATUS_ERROR) {
			printf("Unable to open db file\n");
			return -1;
		}
        if (validate_db_header(fd, &header) == STATUS_ERROR) {
            printf("Failed to validate db header\n");
            close(fd);
            free(header);
            return -1;
        }
        if (read_employee_list(fd, header, &employees) == STATUS_ERROR) {
            printf("Unable to read employee list\n");
            close(fd);
            free(header);
            free(employees);
            return -1;
        }
	}

    // handles closing files and freeing ptrs
    output_db_file(fd, header, employees);
	return 0;
}
