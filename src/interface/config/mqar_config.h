#ifndef MQAR_CONFIG_H
#define MQAR_CONFIG_H

typedef int (*mqar_parse_config_cbfunc_t)(void);

struct config_module_t {
    char *name;
    mqar_parse_config_cbfunc_t parse;
};
typedef struct config_module_t config_module_t;

#endif
