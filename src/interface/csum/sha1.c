#include <openssl/sha.h>
#include "mqar.h"
#include "mqar_csum.h"

static int mqar_sha1_init(void *handle);
static int mqar_sha1_update(void *handle, const void *data, ssize_t len);
static int mqar_sha1_final(unsigned char *sum, void *handle);
static int mqar_digest_size_sha1(void);

int init(csum_module_t *module)
{
    module = (csum_module_t *)malloc(sizeof(csum_module_t));
    module->name = strdup("sha1");
    module->init = mqar_sha1_init;
    module->update = mqar_sha1_update;
    module->final = mqar_sha1_final;
    module->size = mqar_digest_size_sha1;
    
    return MQAR_SUCCESS;
}

void finalize(csum_module_t *module)
{
    free(module->name);
    free(module);
}

static int
mqar_sha1_init(void *handle)
{
    SHA_CTX *c = (SHA_CTX *)handle;
    return SHA1_Init(c);
}

static int
mqar_sha1_update(void *handle, const void *data, ssize_t len)
{
    SHA_CTX *c = (SHA_CTX *)handle;
    return SHA1_Update(c, data, (unsigned long)len);
}

static int
mqar_sha1_final(unsigned char *sum, void *handle)
{
    SHA_CTX *c = (SHA_CTX *)handle;
    return SHA1_Final(sum, c);
}

static int
mqar_digest_size_sha1()
{
    return 20;
}
