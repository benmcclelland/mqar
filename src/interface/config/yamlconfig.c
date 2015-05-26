#include <stdio.h>
#include <yaml.h>
#include "mqar.h"
#include "mqar_config.h"

static int yaml_parse_config(void);

int init(config_module_t *module)
{
    char *fname;
    char *envpath;
    
    envpath = getenv("MQAR_CONFIG_DIR");
    
    if (!envpath) {
        fname = strdup("/etc/mqar");
        asprintf(fname, "%s/config.yaml", fname);
    } else {
        asprintf(fname, "%s/config.yaml", envpath);
    }
    
    if (-1 != access(fname, F_OK)) {
        module = (config_module_t *)malloc(sizeof(config_module_t));
        module->name = strdup("yaml");
        module->parse = yaml_parse_config;

        free(fname);
        return MQAR_SUCCESS;
    } else {
        free(fname);
        return MQAR_ERROR;
    }
}

static int
yaml_parse_config(void)
{
    FILE *fh;
    yaml_parser_t parser;
    yaml_token_t  token;
    char *fname;
    char *envpath;
    
    envpath = getenv("MQAR_CONFIG_DIR");
    
    if (!envpath) {
        fname = strdup("/etc/mqar");
        asprintf(fname, "%s/config.yaml", fname);
    } else {
        asprintf(fname, "%s/config.yaml", envpath);
    }
    
    fh = fopen(fname, "r");
    free(fname);

    if(!yaml_parser_initialize(&parser))
        fprintf(stderr, "Failed to initialize parser!\n");
    if(fh == NULL)
        fprintf(stderr, "Failed to open file!\n");
    
    yaml_parser_set_input_file(&parser, fh);
    
    do {
        yaml_parser_scan(&parser, &token);
        switch(token.type)
        {
            case YAML_STREAM_START_TOKEN:
                printf("STREAM START\n");
                break;
            case YAML_STREAM_END_TOKEN:
                printf("STREAM END\n");
                break;
            case YAML_KEY_TOKEN:
                printf("(Key token)   ");
                break;
            case YAML_VALUE_TOKEN:
                printf("(Value token) ");
                break;
            case YAML_BLOCK_SEQUENCE_START_TOKEN:
                printf("<b>Start Block (Sequence)</b>");
                break;
            case YAML_BLOCK_ENTRY_TOKEN:
                printf("<b>Start Block (Entry)</b>\n");
                break;
            case YAML_BLOCK_END_TOKEN:
                printf("<b>End block</b>");
                break;
            case YAML_BLOCK_MAPPING_START_TOKEN:
                printf("[Block mapping]\n");
                break;
            case YAML_SCALAR_TOKEN:
                printf("scalar %s \n", token.data.scalar.value);
                break;
            default:
                printf("Got token of type %d\n", token.type);
        }
        if(token.type != YAML_STREAM_END_TOKEN)
            yaml_token_delete(&token);
    } while(token.type != YAML_STREAM_END_TOKEN);
    yaml_token_delete(&token);
    
    yaml_parser_delete(&parser);
    fclose(fh);
    return 0;
}
