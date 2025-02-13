#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage (1): %s -f <db_file> -n\n", argv[0]);
	printf("Usage (2): %s -f <db_file> -a <name,address,hours>\n", argv[0]);
	printf("Usage (3): %s -f <db_file> -u <name,hours>\n", argv[0]);
	printf("Usage (4): %s -f <db_file> -r <name>\n", argv[0]);
	printf("\t-f: (required) path to database file\n");
	printf("\t-n: create new database file\n");
	printf("\t-a: add entry\n");
	printf("\t-u: update entry\n");
	printf("\t-r: remove entry\n");
	printf("\t-l: list all entries\n");
	printf("\t-g: show debug info\n");
}

int main(int argc, char *argv[]) {
	char *fpath = NULL;
	bool newfile = 0;
	bool add = 0;
	bool update = 0;
	bool remove = 0;
    bool list = 0;
	bool debug = 0;
    char *data_to_add = NULL;
    char *data_to_update = NULL;
    char *name_to_remove = NULL;

	int op;
	while ((op = getopt(argc, argv, "f:na:u:r:lg")) != -1) {
		switch(op) {
			case 'f':
				fpath = optarg;
				break;
			case 'n':
				newfile = true;
				break;
			case 'a':
				add = true;
                data_to_add = optarg;
				break;
			case 'u':
				update = true;
                data_to_update = optarg;
				break;
			case 'd':
				remove = true;
                name_to_remove = optarg;
				break;
            case 'l':
                list = true;
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
		printf("\t remove = %d\n", remove);
        printf("\t employee data = %s\n", data_to_add);
	}

	if (fpath == NULL) {
		printf("File path is a required argument\n");
		print_usage(argv);
		return -1;
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
	}else {
		fd = open_db_file(fpath);
		if (fd == STATUS_ERROR) {
			printf("Unable to open db file\n");
			return -1;
		}
        if (validate_db_header(fd, &header) == STATUS_ERROR) {
            printf("Failed to validate db header\n");
            close(fd);
            return -1;
        }
	}

    if (read_employee_list(fd, header, &employees) == STATUS_ERROR) {
        printf("Failed to read employee list\n");
        close(fd);
        return -1;
    }

    if (add) {
        if (add_employee(data_to_add, header, &employees) == STATUS_ERROR) {
            printf("Failed to add employee\n");
            close(fd);
            return -1;
        }
    }

    if (update) {
        if (update_employee(data_to_update, header, employees) == STATUS_ERROR) {
            printf("Failed to update entry\n");
            close(fd);
            return -1;
        }
    }
    
    if (remove) {
        if (remove_employee(name_to_remove, header, &employees) == STATUS_ERROR) {
            printf("Failed to remove entry\n");
            close(fd);
            return -1;
        }
    }

    if (list) {
        print_employee_list(header, employees);
    }

    // handles closing files and freeing ptrs
    output_db_file(fd, header, employees);
	return 0;
}
