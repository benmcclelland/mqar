#ifndef MQAR_PLUGIN_LOADER_H
#define MQAR_PLUGIN_LOADER_H

struct mqar_module_t;
typedef struct mqar_module_t mqar_module_t;

typedef int (*mqar_module_select_func_t)(mqar_module_t *);
typedef void (*mqar_module_finalize_func_t)(mqar_module_t *);

struct mqar_module_t {
	char *name;
	int priority;
	mqar_module_select_func_t select;
	mqar_module_finalize_func_t finalize;
};

/* return highest priority, sucessfully initialized module */
mqar_module_t *
find_plugin(char *interface);

/* return list of non-0 priority, sucessfully initialized  modules */
zlist_t *
find_plugins(char *interface)

#endif
