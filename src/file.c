#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"

int create_db_file(char *fpath) {
	int fd = open(fpath, O_CREAT, 0644);
	if (fd == -1) {
		perror("Error in creating db file\n");
	}
	return fd;
}

int open_db_file(char *fpath) {
	int fd = open(fpath, O_RDONLY);
	if (fd == -1) {
		perror("Error in opening db file\n");
	}
	return fd;
}

