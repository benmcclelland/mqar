#ifndef MQAR_CSUM_H
#define MQAR_CSUM_H

typedef int (*mqar_csum_init_cbfunc_t)(void *handle);
typedef int (*mqar_csum_update_cbfunc_t)(void *handle, const void *data, ssize_t len);
typedef int (*mqar_csum_final_cbfunc_t)(unsigned char *sum, void *handle);
typedef int (*mqar_digest_size_cbfunc_t)(void);

struct csum_module_t {
    char *name;
    mqar_csum_init_cbfunc_t init;
    mqar_csum_update_cbfunc_t update;
    mqar_csum_final_cbfunc_t final;
    mqar_digest_size_cbfunc_t size;
};
typedef struct csum_module_t csum_module_t;

#endif
