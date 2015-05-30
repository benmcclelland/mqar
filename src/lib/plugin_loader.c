#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "mqar.h"

static int
find_plugin(char *interface)
{
    DIR *dir;
    struct dirent *dp;
    char *filematch;
    char *dirname;
    char *fullpath;
    char *envname;
    void *libhandle;
    int (*initfunc)(void);
    
    envname = getenv("MQAR_PLUGIN_DIR");
    filematch = strdup("mqar_archive_");
    
    if (!envname) {
        dirname = strdup("/usr/lib64/mqar_plugin");
    } else {
        dirname = strdup(envname);
    }
    
    dir = opendir(dirname);
    
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
                    if (0 != (*initfunc)()) {
                        printf("debug: plugin init function returned error\n");
                        dlclose(libhandle);
                    } else {
                        //save module
                    }
                }
            }
        }
    }
    
    free(dirname);
    closedir(dir);
    return MQAR_SUCCESS;
}
