#include "mdp_common.h"
#include "mdp_client.h"

int
main (int argc, char *argv [])
{
    int verbose = (argc > 1 && streq (argv [1], "-v"));
    mdp_client_t *client = mdp_client_new("tcp://localhost:12345", verbose);

    zmsg_t *request = zmsg_new();
    zmsg_pushstr(request, "DATA");
    zmsg_pushstr(request, "TEST");
    mdp_client_send(client, "DK1", &request);

    char *command = NULL;
    char *service = NULL;
    zmsg_t *report = mdp_client_recv(client, &command, &service);
    if (report == NULL) {
        printf("invalid response.\n");
        return 1;
    }
    assert(zmsg_size (report) >= 2);
    zframe_t *report_type = zmsg_pop(report);
    char *report_type_str = zframe_strdup(report_type);
    zframe_destroy(&report_type);
    zframe_t *status = zmsg_pop(report);
    char *status_str = zframe_strdup(status);
    zframe_destroy(&status);
    
    printf("%s: %s %s\n", service, report_type_str, status_str);
    
    free(command);
    free(service);
    free(report_type_str);
    free(status_str);
    zmsg_destroy(&report);

    mdp_client_destroy(&client);
    return 0;
}
