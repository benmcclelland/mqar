#include <sys/types.h>
#include <sys/stat.h>

#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mqar.h"

struct archive_handle_t {
    struct archive *a;
    char *filename;
    char buf[16384];
};
typedef struct archive_handle_t archive_handle_t;

static int
diskarchive_init(archive_handle_t *handle)
{
    handle = (archive_handle_t *)malloc(sizeof(archive_handle_t));
    
    handle->a = archive_write_new();
    archive_write_add_filter_none(handle->a);
    archive_write_set_format_pax_restricted(handle->a);
    
    /* get filename from next seqnum */
    handle->filename = strdup("/tmp/file.tar");
    archive_write_open_filename(handle->a, handle->filename);
}

static int
diskarchive_write_file(archive_handle_t *handle, const char **filelist)
{
    struct archive *disk;
    int ret, fd;
    struct archive_entry *entry;
    ssize_t len;
    
    while (*filelist != NULL) {
        disk = archive_read_disk_new();
        
        ret = archive_read_disk_open(disk, *filelist);
        if (ret != ARCHIVE_OK) {
            zclock_log("E: write file open source failed %s: ", archive_error_string(disk));
            archive_read_free(disk);
            return(MQAR_ERROR);
        }
        
        for (;;) {
            entry = archive_entry_new();
            ret = archive_read_next_header2(disk, entry);
            if (ret == ARCHIVE_EOF)
                archive_entry_free(entry);
                break;
            if (ret != ARCHIVE_OK) {
                zclock_log("E: write file read next header failed %s: ", archive_error_string(disk));
                goto error;
            }
            archive_read_disk_descend(disk);
            ret = archive_write_header(handle->a, entry);
            if (ret < ARCHIVE_OK || ret == ARCHIVE_FATAL) {
                zclock_log("E: write file write header failed %s: ", archive_error_string(disk));
                goto error;
            }
            if (ret > ARCHIVE_FAILED) {
                /* copy data into the target archive. */
                fd = open(archive_entry_sourcepath(entry), O_RDONLY);
                len = read(fd, handle->buf, sizeof(handle->buf));
                while (len > 0) {
                    archive_write_data(handle->a, handle->buf, len);
                    len = read(fd, handle->buf, sizeof(handle->buf));
                }
                close(fd);
            }
            archive_entry_free(entry);
        }
        archive_read_close(disk);
        archive_read_free(disk);
        filelist++;
    }
    return MQAR_SUCCESS;
    
error:
    archive_entry_free(entry);
    archive_read_close(disk);
    archive_read_free(disk);
    return(MQAR_ERROR);
}

static int
diskarchive_finalize(archive_handle_t *handle)
{
    archive_write_close(handle->a);
    archive_write_free(handle->a);
    free(handle);
}