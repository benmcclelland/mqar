#include <sys/types.h>
#include <sys/stat.h>

#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int
diskarchive_init(archive_handle_t *handle)
{
    struct archive *a;
    struct archive *disk;
    struct archive_entry *entry;
}

static int
diskarchive_write_file()
{
}