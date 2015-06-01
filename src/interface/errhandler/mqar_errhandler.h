#ifndef MQAR_ERROR_H
#define MQAR_ERROR_H

#include "plugin_loader.h"

typedef int (*mqar_err_handle_func_t)(int);

struct error_module_t {
    char *name;
	int priority;
	mqar_module_select_func_t select;
	mqar_module_finalize_func_t finalize;
    mqar_err_handle_func_t handle_err;
};
typedef struct error_module_t error_module_t;

#endif
