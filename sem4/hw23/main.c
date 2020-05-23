#define FUSE_USE_VERSION 30   // API version 3.0
#include <stdio.h>
#include <fuse.h>
#include <asm/errno.h>
#include <zconf.h>
#include "string.h"


// contents to be accessed by reading files

static const char DummyData[] = "Hello, World!\n";

// callback function to be called after 'stat' system call
int my_stat(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    // check if accessing root directory
    if (0==strcmp("/", path)) {
        st->st_mode = 0555 | S_IFDIR;  // file type - dir, access read only
        st->st_nlink = 2;              // at least 2 links: '.' and parent
        return 0;                      // success!
    }
    if (0!=strcmp("/a.txt", path) && 0!=strcmp("/b.txt", path)) {
        return -ENOENT; // error: we have no files other than a.txt and b.txt
    }

    st->st_mode = S_IFREG | 0444;    // file type - regular, access read only
    st->st_nlink = 1;                // one link to file
    st->st_size = sizeof(DummyData); // bytes available
    return 0;                        // success!
}

// callback function to be called after 'readdir' system call
int my_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
               struct fuse_file_info *fi, enum fuse_readdir_flags *flags)
{
    if (0 != strcmp(path, "/")) {
        return -ENOENT;  // we do not have subdirectories
    }

    // two mandatory entries: the directory itself and its parent
    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    // directory contents
    filler(out, "a.txt", NULL, 0, 0);
    filler(out, "b.txt", NULL, 0, 0);

    return 0;  // success
}

// callback function to be called after 'open' system call
int my_open(const char *path, struct fuse_file_info *fi)
{
    if (0!=strcmp("/a.txt", path) && 0!=strcmp("/b.txt", path)) {
        return -ENOENT;   // we have only two files in out filesystem
    }
    if (O_RDONLY != (fi->flags & O_ACCMODE)) {
        return -EACCES;   // file system is read-only, so can't write
    }
    return 0;  // success!
}


// callback function to be called after 'read' system call
int my_read(const char *path, char *out, size_t size, off_t off,
            struct fuse_file_info *fi)
{
    // 'read' might be called with arbitary arguments, so check them
    if (off > sizeof(DummyData))
        return 0;
    // reading might be called within some non-zero offset
    if (off+size > sizeof(DummyData))
        size = sizeof(DummyData) - off;
    const void *data = DummyData + off;
    // copy contents into the buffer to be filled by 'read' system call
    memcpy(out, data, size);
    // return value is bytes count (0 or positive) or an error (negative)
    return size;
}

// register functions as callbacks
struct fuse_operations operations = {
        int (*readdir) my_readdir,
        .readdir = my_readdir,
        .getattr = my_stat,
        .open    = my_open,
        .read    = my_read,
};


int main(int argc, char** argv)
{
    // initialize modificable array {argc, argv}
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // struct to be filled by options parsing
    typedef struct {
        char *src;
        int  help;
    } my_options_t;

    my_options_t my_options;
    memset(&my_options, 0, sizeof(my_options));
}

