#define FUSE_USE_VERSION 30   // API version 3.0
#include <fuse.h>
#include <stdio.h>
#include <stddef.h>
#include <asm/errno.h>
#include <stdint.h>
#include <zconf.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


//Реализация файловой системы - это программа-демон,
// которая ожидает запросы от ядра и обслуживает их.
//Демон использует стандартный для монтирования набор аргументов командной строки.

char* data_to_read;
uint64_t data_to_read_size = 0;
char* fls_flsname;
uint64_t fls_length = 0;

typedef struct {
    char src[2048];
    int  help;
    uint64_t size;
    uint64_t offset;
} my_options_t;

my_options_t* fls;

// callback function to be called after 'stat' system call
int my_stat(const char* path, struct stat* st, struct fuse_file_info *fi) {
    if (0 == strcmp(path, "/")) {
        st->st_mode = S_IFDIR | 0444; // regular files 0444 permission
        st->st_nlink = 2;
        return 0;
    }
    
    if ('/' == path[0]) {
        for (uint64_t i = 0; i < fls_length; ++i) {
            if (strcmp(path + 1, fls[i].src) == 0) {
                st->st_mode = S_IFREG | 0444;       // file type - regular, access read only
                st->st_nlink = 1;                   // one link to file
                st->st_size = fls[i].size;   // bytes available
                return 0;
            }
        }
    }
    
    return -ENOENT; // error: we have no files other
}

// callback function to be called after 'read' system call
int my_read(const char* path, char* out, size_t size, off_t off, struct fuse_file_info* fi) {
    if (0 == strcmp(path, "/")) {
        return -ENOENT;
    }

    if ( '/' == path[0]) {
        for (uint64_t i = 0; i < fls_length; ++i) {
            if (0 == strcmp(path + 1, fls[i].src)) {
                memcpy(out, data_to_read + fls[i].offset, fls[i].size);
                // copy contents into the buffer to be filled by 'read' system call
                return fls[i].size;
            }
        }
    }

    return -ENOENT;
}


// callback function to be called after 'readdir' system call
int my_readdir(const char* path, void* out, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi~) {
    if (0 != strcmp(path, "/")) {
        return -ENOENT;
    }
    filler(out, ".", NULL, 0);
    filler(out, "..", NULL, 0);

    for (uint64_t i = 0; i < fls_length; ++i) {
        filler(out, fls[i].src, NULL, 0);
    }

    return 0;   // success
}


int main(int argc, char** argv)
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    struct fuse_opt opt_specs[] = {
            { "--src %s", 0, 0 },
            { NULL, offsetof(my_options_t, src), 0},
            {NULL,0,0}
    };
    struct fuse_operations operations = {
            .getattr = my_stat,
            .read = my_read,
            .readdir = my_readdir,
    };

    fuse_opt_parse(&args, &fls_flsname, opt_specs, NULL);

    /// start initialization of filesystem
    FILE* f_list = fopen(fls_flsname, "r");
    fscanf(f_list, "%lu", &fls_length);

    fls = malloc(fls_length);

    uint64_t current_offset = 0;
    for (uint64_t i = 0; i < fls_length; ++i) {
        my_options_t* file = fls + i;
        fscanf(f_list, "%s %lu", file->src, &(file->size));
        file->offset = current_offset;
        current_offset += file->size;
    }

    fseek(f_list, 2, SEEK_CUR);
    struct stat fls_file_stat;
    stat(fls_flsname, &fls_file_stat);
    data_to_read_size = fls_file_stat.st_size - ftell(f_list);
    data_to_read = malloc(data_to_read_size);

    fread(data_to_read, 1, data_to_read_size, f_list);
    /// finish initialization of filesystem


    int ret = fuse_main(args.argc, args.argv,&operations,NULL);

    fclose(f_list);
    free(fls);
    free(data_to_read);

    return ret;
}