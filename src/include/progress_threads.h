/*
 * Copyright (c) 2014      Intel, Inc.  All rights reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef MQAR_PROGRESS_THREADS_H
#define MQAR_PROGRESS_THREADS_H

/* start a progress thread, assigning it the provided name for
 * tracking purposes. If create_block is true, then this function
 * will also create a pipe so that libevent has something to block
 * against, thus keeping the thread from free-running
 */
mqar_event_base_t *mqar_start_progress_thread(char *name, bool create_block);

/* stop the progress thread of the provided name. This function will
 * also cleanup the blocking pipes and release the event base if
 * the cleanup param is true */
void mqar_stop_progress_thread(char *name, bool cleanup);

/* restart the progress thread of the provided name */
int mqar_restart_progress_thread(char *name);

#endif
