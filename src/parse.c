#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>

#include "common.h"
#include "parse.h"

// could have a backup functionality
int write_db_file(int fd, struct header_t *h, struct employee_t *elist) {
    // make sure file is always the correct size
    if (ftruncate(fd, h->filesize) == -1) {
        perror("ftruncate");
        close(fd);
        free(h);
        free(elist);
        return STATUS_ERROR;
    }

    // save val bc changing endianness causes count to be gibberish
    int realcount = h->count;

    // host endian to network endian
    h->magic = htonl(h->magic);
    h->version = htons(h->version);
    h->count = htons(h->count);
    h->filesize = htonl(h->filesize);
    
    // resets fd offset to beginning of file
    // otherwise, writes to current cursor pos
    lseek(fd, 0, SEEK_SET);

    // write header + employee list
    write(fd, h, sizeof(struct header_t));
    for (int i=0; i<realcount; ++i) {
        elist[i].hours = htonl(elist[i].hours);
        write(fd, &elist[i], sizeof(struct employee_t));
    }

    close(fd);
    free(h);
    free(elist);
    h = NULL, elist = NULL;
    return STATUS_SUCCESS;
}

int create_db_header(struct header_t **hout) {
    // malloc vs calloc: calloc inits vals to 0 where malloc is random
    struct header_t *h = calloc(1, sizeof(struct header_t));
    if (h == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }

    h->magic = HEADER_MAGIC;
    h->version = 0x1;
    h->count = 0;
    h->filesize = sizeof(struct header_t);
    *hout = h;
    return STATUS_SUCCESS;
}

// file is closed in main.c or by output_db_file
// just need to free relevant pointers
int validate_db_header(int fd, struct header_t **hout) {
    struct stat dbstat = {0};
    if (fstat(fd, &dbstat) == -1) {
        perror("fstat");
        return STATUS_ERROR;
    }
    
    struct header_t *h = calloc(1, sizeof(struct header_t));
    if (h == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }
    
    if (read(fd, h, sizeof(struct header_t)) == -1) {
        perror("read");
        free(h);
        return STATUS_ERROR;
    }

    // network endian to host endian
    // otherwise, read gibberish
    h->magic = ntohl(h->magic);
    h->version = ntohs(h->version);
    h->count = ntohs(h->count);
    h->filesize = ntohl(h->filesize);

    if (dbstat.st_size != h->filesize) {
        printf("Corrupted database: ENEMY SPOTTED\n");
        free(h);
        return STATUS_ERROR;
    }
    if (h->magic != HEADER_MAGIC) {
        printf("Invalid header\n");
        free(h);
        return STATUS_ERROR;
    }
    if (h->version != 1) {
        printf("Mismatched db version\n");
        free(h);
        return STATUS_ERROR;
    }
    if (h->count < 0) {
        printf("Negative employee count\n");
        free(h);
        return STATUS_ERROR;
    }
    *hout = h;
    return STATUS_SUCCESS;
}

int read_employee_list(int fd, struct header_t *h, struct employee_t **eout) {
    // fd should be at after header read
    struct employee_t *elist = calloc(h->count, sizeof(struct employee_t));
    if (elist == NULL) {
        perror("calloc");
        free(h);
        return STATUS_ERROR;
    }

    if (read(fd, elist, h->count*sizeof(struct employee_t)) == -1) {
        perror("read");
        free(h);
        free(elist);
        return STATUS_ERROR;
    }
    for (int i=0; i<h->count; ++i) {
        elist[i].hours = ntohl(elist[i].hours);
    }
    *eout = elist;
    return STATUS_SUCCESS; 
}

int add_employee(char *data, struct header_t *h, struct employee_t **eout) {
    char *name = strtok(data, ",");
    char *addr = strtok(NULL, ",");
    char *hours = strtok(NULL, ",");
    
    struct employee_t *elist = *eout;
    if (search_employee_name(name, h, elist) != -1) {
        printf("Employee already exists in db\n");
        free(h);
        free(elist);
        return STATUS_ERROR;
    }

    ++h->count;
    elist = realloc(*eout, h->count*sizeof(struct employee_t));
    if (elist == NULL) {
        perror("realloc");
        free(h);
        free(*eout);
        return STATUS_ERROR;
    }
    
    strncpy(elist[h->count-1].name, name, sizeof(elist[h->count-1].name));
    strncpy(elist[h->count-1].address, addr, sizeof(elist[h->count-1].address));
    elist[h->count-1].hours = atoi(hours);

    h->filesize += sizeof(struct employee_t);
    *eout = elist;
    elist = NULL;
    return STATUS_SUCCESS;
}

int update_employee(char *data, struct header_t *h, struct employee_t *elist) {
    char *name = strtok(data, ",");
    char *hours = strtok(NULL, ",");
    
    int i = -1;
    if ((i = search_employee_name(name, h, elist)) == -1) {
        printf("No such employee in db\n");
        free(h);
        free(elist);
        return STATUS_ERROR;
    }
    elist[i].hours = atoi(hours);   
    return STATUS_SUCCESS;
}

int remove_employee(char *name, struct header_t *h, struct employee_t **eout) {
    struct employee_t *elist = *eout;
    int i = -1;
    if ((i = search_employee_name(name, h, elist)) == -1) {
        printf("No such employee in db\n");
        free(h);
        free(elist);
        return STATUS_ERROR;
    }

    --h->count;
    for (; i<h->count; ++i) {
        strncpy(elist[i].name, elist[i+1].name, sizeof(elist[i].name));
        strncpy(elist[i].address, elist[i+1].address, sizeof(elist[i].address));
        elist[i].hours = elist[i+1].hours;
    }
    
    *eout = realloc(elist, h->count*sizeof(struct employee_t));
    if (*eout == NULL) {
        perror("realloc");
        free(h);
        free(elist);
        return STATUS_ERROR;
    }
    h->filesize -= sizeof(struct employee_t);
    return STATUS_SUCCESS;
}

int search_employee_name(char *name, struct header_t *h, struct employee_t *elist) {
    for (int i=0; i<h->count; ++i) {
        if (!strcmp(elist[i].name, name))
            return i;
    }
    return -1;
}

void print_employee_list(struct header_t *h, struct employee_t *elist) {
    for (int i=0; i<h->count; ++i) {
        printf("Employee %d:\n", i);
        printf("\tName = %s\n", elist[i].name);
        printf("\tAddress = %s\n", elist[i].address);
        printf("\tHours = %u\n", elist[i].hours);
    }
}
