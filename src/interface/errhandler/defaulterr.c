#include <stdio.h>
#include "mqar.h"
#include "mqar_errhandler.h"

static int defaulterr_select(mqar_module_t *imod);
static void defaulterr_finalize(mqar_module_t *imod);
static int default_handle_err(int err);

int
init(error_module_t *module)
{
    module = (error_module_t *)malloc(sizeof(error_module_t));
    module->name = strdup("default");
    module->priority = 10;
    module->select = defaulterr_select;
    module->finalize = defaulterr_finalize;
    module->handle_err = default_handle_err;

    return MQAR_SUCCESS;
}

static int
defaulterr_select(mqar_module_t *imod)
{
}

static void
defaulterr_finalize(mqar_module_t *imod)
{
    error_module_t *module = (error_module_t *)imod;
    free(module->name);
    free(module);
}

static int
default_handle_err(int err)
{
    //syslog err
    return MQAR_SUCCESS;
}
