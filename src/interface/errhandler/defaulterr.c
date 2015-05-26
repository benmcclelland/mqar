#include <stdio.h>
#include "mqar.h"
#include "mqar_errhandler.h"

static int default_handle_err(int err);

int
init(error_module_t *module)
{
    module = (error_module_t *)malloc(sizeof(error_module_t));
    module->name = strdup("default");
    module->handle_err = default_handle_err;

    return MQAR_SUCCESS;
}

static int
default_handle_err(int err)
{
    //syslog err
    return MQAR_SUCCESS;
}