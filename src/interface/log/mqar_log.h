#ifndef MQAR_CONFIG_H
#define MQAR_CONFIG_H

#include "plugin_loader.h"

typedef int (*mqar_log_func_t)(int, const char *, ...);

struct log_module_t {
    char *name;
    int priority;
    mqar_module_select_func_t select;
    mqar_module_finalize_func_t finalize;
    mqar_log_func_t log;
};
typedef struct log_module_t log_module_t;

#endif
