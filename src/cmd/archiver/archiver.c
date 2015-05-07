#include "mdp.h"

struct _request_t {
    mdp_worker_t *worker;
    zframe_t *reply_to;
};

typedef struct _request_t request_t;

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
send_request_status(request_t *self, int status)
{
    assert(self);
    
    //  Prepare and send report to the client
    zmsg_t *report = zmsg_new();
    
    if (status == 0)
        zmsg_pushstr(report, "SUCCESS");
    else
        zmsg_pushstr(report, "FAIL");
    
    mdp_worker_send(self->worker, &report, self->reply_to);
}

struct _archiver_t {
    mdp_worker_t *worker;
    zlist_t *archive_requests;
    zlist_t *stage_requests;
};

typedef struct _archiver_t archiver_t;

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
archiver_handle_archive_request(archiver_t *self, zframe_t *data, zframe_t *reply_to)
{
    // init archivefile
    // open archivefile
    // write files to archivefile
    request_t *archive_reqeust = request_new(self->worker, reply_to);
    send_request_status(archive_reqeust, 0);
    request_destroy(&archive_reqeust);
}

static void
archiver_handle_stage_request(archiver_t *self, zframe_t *data, zframe_t *reply_to)
{
    // init archivefile
    // seek archivefile
    // stage file
    request_t *stage_reqeust = request_new(self->worker, reply_to);
    send_request_status(stage_reqeust, 0);
    request_destroy(&stage_reqeust);
}

static void
archive_handle_request(archiver_t *self, zmsg_t *request, zframe_t *reply_to)
{
    assert(zmsg_size(request) >= 3);

    zframe_t *operation = zmsg_pop(request);
    zframe_t *data = zmsg_pop(request);

    if (zframe_streq(operation, "ARCHIVE"))
        archiver_handle_archive_request(self, data, reply_to);
    else
    if (zframe_streq(operation, "STAGE"))
        archiver_handle_stage_request(self, data, reply_to);
    else {
        zclock_log("E: invalid message: ");
        zmsg_dump(request);
    }

    zframe_destroy(&operation);
    zframe_destroy(&data);
    zmsg_destroy(&request);
}

int main(int argc, char *argv [])
{
    int verbose = (argc > 1 && streq(argv [1], "-v"));
    archiver_t *archiver = archiver_new("tcp://localhost:12345", "DK1", verbose);

    while (1) {
        zframe_t *reply_to = NULL;
        zmsg_t *request = mdp_worker_recv (archiver->worker, &reply_to);
        if (request == NULL)
            break;          // Worker has been interrupted
        archive_handle_request(archiver, request, reply_to);
    }
    archiver_destroy(&archiver);
    return 0;
}
