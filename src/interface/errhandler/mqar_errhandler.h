#ifndef MQAR_ERROR_H
#define MQAR_ERROR_H

typedef int (*mqar_err_handle_cbfunc_t)(int);

struct error_module_t {
    char *name;
    mqar_err_handle_cbfunc_t handle_err;
};
typedef struct error_module_t error_module_t;

#endif
