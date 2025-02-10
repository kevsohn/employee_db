#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144

struct header_t {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	unsigned int filesize;
};

struct employee_t {
	char name[256];
	char address[256];
	unsigned int hours;
};

int create_db_header(struct header_t **hout);
int validate_db_header(int fd, struct header_t **hout);
int create_employee_list(struct header_t *h, struct employee_t **eout);
int read_employee_list(int fd, struct header_t *h, struct employee_t **eout);
int output_db_file(int fd, struct header_t *h, struct employee_t *elist);

#endif
