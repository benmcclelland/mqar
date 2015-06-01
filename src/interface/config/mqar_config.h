#ifndef MQAR_CONFIG_H
#define MQAR_CONFIG_H

#include "plugin_loader.h"

typedef int (*mqar_parse_config_func_t)(void);

struct config_module_t {
    char *name;
	int priority;
	mqar_module_select_func_t select;
	mqar_module_finalize_func_t finalize;
    mqar_parse_config_func_t parse;
};
typedef struct config_module_t config_module_t;

#endif
