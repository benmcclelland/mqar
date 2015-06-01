#ifndef MQAR_ARCHIVE_H
#define MQAR_ARCHIVE_H

#include "plugin_loader.h"

struct archive_handle_t {
    struct archive *a;
    char *filename;
    char *buf;
};
typedef struct archive_handle_t archive_handle_t;

struct fileinfo_t {
    char *filename;
    int64_t arch_offset;
};
typedef struct fileinfo_t fileinfo_t;

struct inode_t {
    int inode;
};
typedef struct inode_t inode_t;

typedef int (*mqar_archive_write_init_func_t)(archive_handle_t *handle);
typedef int (*mqar_archive_write_file_func_t)(archive_handle_t *handle, fileinfo_t *filelist);
typedef void (*mqar_archive_write_finalize_func_t)(archive_handle_t *handle);
typedef int (*mqar_archive_read_init_func_t)(archive_handle_t *handle, inode_t inode, int64_t offset);
typedef int (*mqar_archive_read_file_func_t)(archive_handle_t *handle);
typedef void (*mqar_archive_read_finalize_func_t)(archive_handle_t *handle);

struct archive_module_t {
    char *name;
	int priority;
	mqar_module_select_func_t select;
	mqar_module_finalize_func_t finalize;
    mqar_archive_write_init_func_t write_init;
    mqar_archive_write_file_func_t write_file;
    mqar_archive_write_finalize_func_t write_finalize;
    mqar_archive_read_init_func_t read_init;
    mqar_archive_read_file_func_t read_file;
    mqar_archive_read_finalize_func_t read_finalize;
};
typedef struct archive_module_t archive_module_t;

struct vsn_t {
    union {
        char *filename;
        int vsn;
    } data;
};
typedef struct vsn_t vsn_t;

#endif
