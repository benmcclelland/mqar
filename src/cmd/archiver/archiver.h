#ifndef MQAR_ARCHIVER_H
#define MQAR_ARCHIVER_H

struct request_t {
    mdp_worker_t *worker;
    zframe_t *reply_to;
};

typedef struct request_t request_t;

struct archiver_t {
    mdp_worker_t *worker;
    zlist_t *archive_requests;
    zlist_t *stage_requests;
};

typedef struct archiver_t archiver_t;

typedef void (*archive_state_cbfunc_t)(int fd, short args, void* cb);

struct archive_state_t {
    char *operation;
    mqar_event_base_t *ev_base;
    archive_state_cbfunc_t cbfunc;
    int priority;
};

typedef struct archive_state_t archive_state_t;

struct archive_caddy_t {
    archiver_t *archiver;
    zframe_t *data;
    zframe_t *reply_to;
    char *operation;
    mqar_event_t ev;
};

typedef struct archive_caddy_t archive_caddy_t;

/* Global event base handles */
zhash_t *ev_bases;

#endif
