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
#include "mqar_archive.h"

#define RD_BLK_SZ 8192
#define BUFF_SZ 524288

static int diskarchive_write_init(archive_handle_t *handle);
static int diskarchive_write_file(archive_handle_t *handle, fileinfo_t *filelist);
static void diskarchive_write_finalize(archive_handle_t *handle);
static int diskarchive_read_init(archive_handle_t *handle, inode_t inode, int64_t offset);
static int diskarchive_read_file(archive_handle_t *handle);
static void diskarchive_read_finalize(archive_handle_t *handle);

archive_module_t * init()
{
    archive_module_t *module;
    
    module = (archive_module_t *)malloc(sizeof(archive_module_t));
    module->name = strdup("diskarchive");
    module->write_init = diskarchive_write_init;
    module->write_file = diskarchive_write_file;
    module->write_finalize = diskarchive_write_finalize;
    module->read_init = diskarchive_read_init;
    module->read_file = diskarchive_read_file;
    module->read_finalize = diskarchive_read_finalize;
    
    return module;
}

void finalize(archive_module_t *module)
{
    free(module->name);
    free(module);
}

static int64_t
round_up(int64_t input, int multiple)
{
    if(0 == multiple)
    {
        return input;
    }
    
    int remainder = input % multiple;
    if (0 == remainder)
    {
        return input;
    }
    
    return input + multiple - remainder;
}

static char *get_next_file_name()
{
    char *filename = strdup("/tmp/testfile");
    return filename;
}

static int
diskarchive_write_init(archive_handle_t *handle)
{
    handle = (archive_handle_t *)malloc(sizeof(archive_handle_t));
    handle->buf = (char *)malloc(BUFF_SZ * sizeof(char));

    handle->a = archive_write_new();
    archive_write_add_filter_none(handle->a);
    archive_write_set_format_pax_restricted(handle->a);
    
    handle->filename = get_next_file_name();
    if (archive_write_open_filename(handle->a, handle->filename)) {
        zclock_log("E: archive_write_open_filename() %s\n",
                   archive_error_string(handle->a));
        return MQAR_ERROR;
    }
    return MQAR_SUCCESS;
}

static int
diskarchive_write_file(archive_handle_t *handle, fileinfo_t *filelist)
{
    struct archive *disk;
    int ret, fd;
    struct archive_entry *entry;
    ssize_t len;
    
    while (filelist != NULL) {
        disk = archive_read_disk_new();
        
        ret = archive_read_disk_open(disk, filelist->filename);
        if (ret != ARCHIVE_OK) {
            zclock_log("E: write file open source failed %s: ",
                       archive_error_string(disk));
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
                zclock_log("E: write file read next header failed %s: ",
                           archive_error_string(disk));
                goto error;
            }
            archive_read_disk_descend(disk);
            filelist->arch_offset = round_up(archive_filter_bytes(handle->a, 0), 512);
            ret = archive_write_header(handle->a, entry);
            if (ret < ARCHIVE_OK || ret == ARCHIVE_FATAL) {
                zclock_log("E: write file write header failed %s: ",
                           archive_error_string(disk));
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

static void
diskarchive_write_finalize(archive_handle_t *handle)
{
    archive_write_close(handle->a);
    archive_write_free(handle->a);
    free(handle->buf);
    free(handle);
}

static int
diskarchive_read_init(archive_handle_t *handle, inode_t inode, int64_t offset)
{
    int fd;
    off_t seek_offset;
    char *errbuf;
    size_t buflen;
    
    errbuf = (char *)malloc(1024 * sizeof(char));
    buflen = sizeof(errbuf);
    
    handle = (archive_handle_t *)malloc(sizeof(archive_handle_t));
    handle->buf = (char *)malloc(512 * 1024 * sizeof(char));
    
    handle->a = archive_read_new();
    archive_read_support_format_all(handle->a);
    archive_read_support_filter_none(handle->a);
    
    // will be stat file && get filename
    handle->filename = get_next_file_name();
    fd = open(handle->filename, O_RDONLY);
    if (-1 == fd) {
        zclock_log("E: couldn't open archive file: %s",
                   strerror_r(errno, errbuf, buflen));
        free(errbuf);
        return MQAR_ERROR;
    }
    seek_offset = lseek(fd, offset, SEEK_SET);
    if (-1 == seek_offset) {
        zclock_log("E: couldn't seek archive file: %s",
                   strerror_r(errno, errbuf, buflen));
        close(fd);
        free(errbuf);
        return MQAR_ERROR;
    }
    if (archive_read_open_fd(handle->a, fd, RD_BLK_SZ)) {
        zclock_log("E: archive_read_open_fd() %s\n",
                   archive_error_string(handle->a));
        close(fd);
        free(errbuf);
        return MQAR_ERROR;
    }
    
    free(errbuf);
    return MQAR_SUCCESS;
}

static int
diskarchive_read_file(archive_handle_t *handle)
{
    struct archive_entry *entry;
    int r;
    const void *buff;
    size_t rd_size;
    int64_t rd_offset;
    
    r = archive_read_next_header(handle->a, &entry);
    if (ARCHIVE_EOF == r) {
        zclock_log("E: offset at EOF");
        return MQAR_ERROR;
    }
    if (ARCHIVE_OK != r) {
        zclock_log("E: archive_read_next_header() %s",
                   archive_error_string(handle->a));
        return MQAR_ERROR;
    }
    
    printf("\tNAME: '%s'\n", archive_entry_pathname(entry));
    printf("\tSIZE: %" PRId64 "\n", archive_entry_size(entry));
    printf("\tMODE: '%s'\n", archive_entry_strmode(entry));
    printf("\tPERM: 0x%lx\n", (unsigned long)archive_entry_perm(entry));
    
    for (;;) {
        r = archive_read_data_block(handle->a, &buff, &rd_size, &rd_offset);
        if (ARCHIVE_EOF == r) {
            break;
        }
        if (ARCHIVE_OK != r) {
            zclock_log("E: archive_read_data_block() %s\n",
                       archive_error_string(handle->a));
            break;
        }
        printf("%s", (char *)buff);
    }
    printf("\n");

    return MQAR_SUCCESS;
}

static void
diskarchive_read_finalize(archive_handle_t *handle)
{
    archive_read_close(handle->a);
    archive_read_free(handle->a);
    free(handle->buf);
    free(handle);
}
