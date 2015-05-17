#include "mqar.h"
#include "libevent2022.h"
#include "progress_threads.h"
#include "mdp.h"
#include "archiver.h"


static request_t *
request_new(mdp_worker_t *worker, zframe_t *reply_to)
{
    assert(worker);

    request_t *self = (request_t *)zmalloc(sizeof *self);

    //  Initialize request
    self->worker = worker;
    self->reply_to = reply_to;
    return self;
}

static void
request_destroy(request_t **self_p)
{
    assert(self_p);

    if (*self_p) {
        request_t *self = *self_p;
        zframe_destroy(&self->reply_to);
        free(self);
        *self_p = NULL;
    }
}

static void
archive_state_destruct(void *data)
{
    archive_state_t *p = (archive_state_t *)data;
    mqar_stop_progress_thread(p->operation, true);
    free(p->operation);
}

static void
send_request_status(request_t *self, int status, char *operation)
{
    assert(self);
    
    //  Prepare and send report to the client
    zmsg_t *report = zmsg_new();
    
    if (status == 0)
        zmsg_pushstr(report, "SUCCESS");
    else
        zmsg_pushstr(report, "FAIL");
    
    zmsg_pushstr(report, operation);
    
    mdp_worker_send(self->worker, &report, self->reply_to);
}

static archiver_t *
archiver_new(char *broker, char *service, int verbose)
{
    archiver_t *self = (archiver_t *)zmalloc(sizeof *self);

    //  Initialize engine state
    self->worker = mdp_worker_new(NULL, broker, service, verbose);
    self->archive_requests = zlist_new();
    self->stage_requests = zlist_new();
    return self;
}

static void
archiver_destroy(archiver_t **self_p)
{
    assert(self_p);
    if (*self_p) {
        archiver_t *self = *self_p;
        mdp_worker_destroy (&self->worker);

        //  Destroy remaining archive requests
        while (zlist_size(self->archive_requests) > 0) {
            request_t *request = (request_t *)zlist_pop(self->archive_requests);
            request_destroy(&request);
        }
        zlist_destroy(&self->archive_requests);

        //  Destroy remaining stage requests
        while (zlist_size(self->stage_requests) > 0) {
            request_t *request = (request_t *)zlist_pop(self->stage_requests);
            request_destroy(&request);
        }
        zlist_destroy(&self->stage_requests);
        
        free(self);
        *self_p = NULL;
    }
}

static void
archiver_handle_archive_request(int fd, short args, void* cbdata)
{
    archive_caddy_t *caddy = (archive_caddy_t *)cbdata;
    archiver_t *self = caddy->archiver;

    // unserialize caddy->data
    
    zframe_destroy(&caddy->data);

    // init archivefile
    // open archivefile
    // write files to archivefile
    
    request_t *request = request_new(self->worker, caddy->reply_to);
    send_request_status(request, 0, caddy->operation);
    request_destroy(&request);
    
    free(caddy->operation);
    free(caddy);
}

static void
archiver_handle_stage_request(int fd, short args, void* cbdata)
{
    archive_caddy_t *caddy = (archive_caddy_t *)cbdata;
    archiver_t *self = caddy->archiver;
    
    // unserialize caddy->data
    
    zframe_destroy(&caddy->data);
    
    // init archivefile
    // seek archivefile
    // stage file
    
    request_t *request = request_new(self->worker, caddy->reply_to);
    send_request_status(request, 0, caddy->operation);
    request_destroy(&request);
    
    free(caddy->operation);
    free(caddy);
}

static void
archiver_handle_test_request(int fd, short args, void* cbdata)
{
    archive_caddy_t *caddy = (archive_caddy_t *)cbdata;
    archiver_t *self = caddy->archiver;
    
    // unserialize caddy->data
    char *data = zframe_strdup(caddy->data);
    
    printf("Got an event on socket %d:%s%s%s%s\n",
           (int) fd,
           (args&EV_TIMEOUT) ? " timeout" : "",
           (args&EV_READ)    ? " read" : "",
           (args&EV_WRITE)   ? " write" : "",
           (args&EV_SIGNAL)  ? " signal" : "");


    fprintf(stderr, "handling test request, got data: %s\n", data);
    free(data);
    sleep(30);
    fprintf(stderr, "sending response back\n");
    zframe_destroy(&caddy->data);
    
    // init archivefile
    // seek archivefile
    // stage file
    
    request_t *request = request_new(self->worker, caddy->reply_to);
    send_request_status(request, 0, caddy->operation);
    request_destroy(&request);
    
    free(caddy->operation);
    free(caddy);
}

static void
archive_handle_request(archiver_t *self, zmsg_t *request, zframe_t *reply_to)
{
    assert(zmsg_size(request) >= 2);

    archive_caddy_t *caddy;
    archive_state_t *op_state;
    zframe_t *z_operation = zmsg_pop(request);
    zframe_t *z_data = zmsg_pop(request);
    char *operation = zframe_strdup(z_operation);

    op_state = (archive_state_t *)zhash_lookup(ev_bases, operation);

    if (NULL != op_state) {
        if (NULL != op_state->ev_base) {
            if (NULL != op_state->cbfunc) {
                zclock_log("I: activating cbfunc for op (%s): ", operation);
                caddy = (archive_caddy_t *)malloc(sizeof(archive_caddy_t));
                caddy->archiver = self;
                caddy->data = z_data;
                caddy->reply_to = reply_to;
                caddy->operation = operation;
                mqar_event_set(op_state->ev_base, &caddy->ev, -1,
                               MQAR_EV_WRITE, op_state->cbfunc, caddy);
                mqar_event_set_priority(&caddy->ev, op_state->priority);
                mqar_event_active(&caddy->ev, MQAR_EV_WRITE, 1);
            } else {
                zclock_log("E: no function for op: ");
                zmsg_dump(request);
            }
        } else {
            zclock_log("E: invalid message: ");
            zmsg_dump(request);
        }
    } else {
        zclock_log("E: invalid operation (%s): ", operation);
        zmsg_dump(request);
    }

    zframe_destroy(&z_operation);
    zmsg_destroy(&request);
}

int
main(int argc, char *argv [])
{
    int verbose = (argc > 1 && streq(argv [1], "-v"));
    archive_state_t *archive_state, *stage_state, *test_state, *item;
    archiver_t *archiver = archiver_new("tcp://localhost:12345", "DK1", verbose);
    
    ev_bases = zhash_new();
    mqar_init_progress_threads();

    archive_state = (archive_state_t *)malloc(sizeof(archive_state_t));
    archive_state->operation = strdup("ARCHIVE");
    archive_state->ev_base = mqar_start_progress_thread(archive_state->operation, true);
    archive_state->cbfunc = archiver_handle_archive_request;
    archive_state->priority = 1;
    zhash_insert(ev_bases, archive_state->operation, (void *)archive_state);
    zhash_freefn(ev_bases, archive_state->operation, archive_state_destruct);
    
    stage_state = (archive_state_t *)malloc(sizeof(archive_state_t));
    stage_state->operation = strdup("STAGE");
    stage_state->ev_base = mqar_start_progress_thread(stage_state->operation, true);
    stage_state->cbfunc = archiver_handle_stage_request;
    stage_state->priority = 1;
    zhash_insert(ev_bases, stage_state->operation, (void *)stage_state);
    zhash_freefn(ev_bases, stage_state->operation, archive_state_destruct);
    
    test_state = (archive_state_t *)malloc(sizeof(archive_state_t));
    test_state->operation = strdup("TEST");
    test_state->ev_base = mqar_start_progress_thread(test_state->operation, true);
    test_state->cbfunc = archiver_handle_test_request;
    test_state->priority = 1;
    zhash_insert(ev_bases, test_state->operation, (void *)test_state);
    zhash_freefn(ev_bases, test_state->operation, archive_state_destruct);
    
    while (1) {
        zframe_t *reply_to = NULL;
        zmsg_t *request = mdp_worker_recv (archiver->worker, &reply_to);
        if (request == NULL)
            break;          // Worker has been interrupted
        archive_handle_request(archiver, request, reply_to);
    }

    archiver_destroy(&archiver);
    zhash_destroy(&ev_bases);
    
    return 0;
}
