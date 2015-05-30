#ifndef MQAR_CONFIG_H
#define MQAR_CONFIG_H

typedef int (*mqar_log_cbfunc_t)(int, const char *, ...);

struct log_module_t {
    char *name;
    mqar_log_cbfunc_t log;
};
typedef struct log_module_t log_module_t;

#endif
