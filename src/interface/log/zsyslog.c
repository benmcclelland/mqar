#include <stdio.h>
#include <stdarg.h>
#include "mqar.h"
#include "mqar_log.h"

static int zsyslog_select(mqar_module_t *imod);
static void zsyslog_finalize(mqar_module_t *imod);
static int mqar_zsys_log(int level, const char *format, ...);

int init(log_module_t *module)
{
    char *envident;
    
    envident = getenv("MQAR_IDENT");
    zsys_set_logident(envident ? envident : "MQAR");
    zsys_set_logsystem(true);
    
    module = (log_module_t *)malloc(sizeof(log_module_t));
    module->name = strdup("zsys");
	module->priority = 10;
	module->select = zsyslog_select;
	module->finalize = zsyslog_finalize;
    module->log = mqar_zsys_log;
    
    return MQAR_SUCCESS;
}

static int zsyslog_select(mqar_module_t *imod)
{
}

static void zsyslog_finalize(mqar_module_t *imod)
{
	log_module_t *module = (log_module_t *)imod;
	free(module->name);
	free(module);
}

static int
mqar_zsys_log(int level, const char *format, ...)
{
    va_list argptr;
    
    switch (level) {
        case MQAR_LOG_DEBUG:
            zsys_debug(format, argptr);
            break;
        case MQAR_LOG_INFO:
            zsys_info(format, argptr);
            break;
        case MQAR_LOG_NOTICE:
            zsys_notice(format, argptr);
            break;
        case MQAR_LOG_WARNING:
            zsys_warning(format, argptr);
            break;
        case MQAR_LOG_ERROR:
            zsys_error(format, argptr);
            break;
        default:
            zsys_info(format, argptr);
    }
    
    return MQAR_SUCCESS;
}
