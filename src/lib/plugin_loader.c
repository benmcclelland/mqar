#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "mqar.h"

mqar_module_t *
find_plugin(char *interface)
{
    DIR *dir;
    struct dirent *dp;
    char *filematch;
    char *dirname;
    char *fullpath;
    char *envname;
    void *libhandle;
    int (*initfunc)(mqar_module_t);
	zlist *plugins;
	mqar_module_t *module;
	mqar_module_t *retmod = NULL;
	int max_prio = 0;
	bool found = false;

    envname = getenv("MQAR_PLUGIN_DIR");
    filematch = strdup("mqar_");
	asprintf(filematch, "%s_%s_", filematch, interface);

    if (!envname) {
        dirname = strdup("/usr/lib64/mqar_plugin");
    } else {
        dirname = strdup(envname);
    }

    dir = opendir(dirname);

	plugins = zlist_new();

    while (NULL != (dp = readdir(dir))) {
        printf("debug: %s\n", dp->d_name);
        if (0 == strncmp(dp->d_name, filematch, sizeof(filematch))) {
            printf("found match: \"%s\"\n", dp->d_name);
            asprintf(fullpath, "%s/%s", dirname, dp->d_name);
            libhandle = dlopen(fullpath, RTLD_NOW|RTLD_GLOBAL);
            free(fullpath);
            fullpath = NULL;

            if (!libhandle) {
                printf("debug: could not dlopen %s: %s\n", dp->d_name, dlerror());
            } else {
                *(void **)(&initfunc) = dlsym(libhandle, "init");
                if (!initfunc) {
                    printf("debug: could not find init function %s: %s\n", dp->d_name, dlerror());
                    dlclose(libhandle);
                } else {
                    if (0 != (*initfunc)(module)) {
                        printf("debug: plugin init function returned error\n");
                        dlclose(libhandle);
                    } else {
						/* 0 priority means don't select it, so just call
						 * the module's finalize routine */
						if (0 == module->priority) {
							if (module->finalize) {
								module->finalize(module);
								continue;
							}
						}
						if (module->priority > max_prio) {
							max_prio = module->priority;
						}
						zlist_append(plugins, (void *)module);
                    }
                }
            }
        }
    }

	module = (mqar_module_t *)zlist_first(plugins);
	while (module) {
		if (!found) {
			if (max_prio == module->priority) {
				if (module->select) {
					module->select(module);
				}
				retmod = module;
				found = true;
			} else {
				if (module->finalize) {
					module->finalize(module);
				}
			}
		} else {
			if (module->finalize) {
				module->finalize(module);
			}
		}
		module = (mqar_module_t *)zlist_next(plugins);
	}

	zlist_destroy(&plugins);
    free(dirname);
    closedir(dir);
    return retmod;
}

zlist_t *
find_plugins(char *interface)
{
	DIR *dir;
	struct dirent *dp;
	char *filematch;
	char *dirname;
	char *fullpath;
	char *envname;
	void *libhandle;
	int (*initfunc)(mqar_module_t);
	zlist_t *plugins;
	mqar_module_t *module;
	int max_prio = 0;

	envname = getenv("MQAR_PLUGIN_DIR");
	filematch = strdup("mqar_");
	asprintf(filematch, "%s_%s_", filematch, interface);

	if (!envname) {
		dirname = strdup("/usr/lib64/mqar_plugin");
	} else {
		dirname = strdup(envname);
	}

	dir = opendir(dirname);

	plugins = zlist_new();

	while (NULL != (dp = readdir(dir))) {
		printf("debug: %s\n", dp->d_name);
		if (0 == strncmp(dp->d_name, filematch, sizeof(filematch))) {
			printf("found match: \"%s\"\n", dp->d_name);
			asprintf(fullpath, "%s/%s", dirname, dp->d_name);
			libhandle = dlopen(fullpath, RTLD_NOW|RTLD_GLOBAL);
			free(fullpath);
			fullpath = NULL;

			if (!libhandle) {
				printf("debug: could not dlopen %s: %s\n", dp->d_name, dlerror());
			} else {
				*(void **)(&initfunc) = dlsym(libhandle, "init");
				if (!initfunc) {
					printf("debug: could not find init function %s: %s\n", dp->d_name, dlerror());
					dlclose(libhandle);
				} else {
					if (0 != (*initfunc)(module)) {
						printf("debug: plugin init function returned error\n");
						dlclose(libhandle);
					} else {
						/* 0 priority means don't select it, so just call
						 * the module's finalize routine */
						if (0 == module->priority) {
							if (module->finalize) {
								module->finalize(module);
								continue;
							}
						}
						if (module->priority > max_prio) {
							max_prio = module->priority;
						}
						if (module->select) {
							module->select(module);
						}
						zlist_append(plugins, (void *)module);
					}
				}
			}
		}
	}

	free(dirname);
	closedir(dir);
	return plugins;
}
