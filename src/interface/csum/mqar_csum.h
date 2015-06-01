#ifndef MQAR_CSUM_H
#define MQAR_CSUM_H

#include "plugin_loader.h"

typedef int (*mqar_csum_init_func_t)(void *handle);
typedef int (*mqar_csum_update_func_t)(void *handle, const void *data, ssize_t len);
typedef int (*mqar_csum_final_func_t)(unsigned char *sum, void *handle);
typedef int (*mqar_digest_size_func_t)(void);

struct csum_module_t {
    char *name;
	int priority;
	mqar_module_select_func_t select;
	mqar_module_finalize_func_t finalize;
    mqar_csum_init_func_t init;
    mqar_csum_update_func_t update;
    mqar_csum_final_func_t final;
    mqar_digest_size_func_t size;
};
typedef struct csum_module_t csum_module_t;

#endif
