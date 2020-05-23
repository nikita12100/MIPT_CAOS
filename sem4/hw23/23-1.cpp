#define FUSE_USE_VERSION 30   // API version 3.0
#include <stdio.h>
#include <stddef.h>
#include <asm/errno.h>
#include <stdint.h>
#include <zconf.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <fuse3/fuse.h>


typedef struct {
    char* src;
    int  help;
} my_options_t;

// initialize operator '<' for dates
bool operator <(const timespec& x, const timespec& y) {
    return (x.tv_sec < y.tv_sec) || ((x.tv_nsec < y.tv_nsec) && (x.tv_sec == y.tv_sec));
}

class MyUnionFS {
public:
    void set_file_path(const char* filepath);
    void initialize_source(char* source_folder);

    std::vector<std::string> folders;
    std::string original_path;
};

MyUnionFS fls;

void MyUnionFS::set_file_path(const char* file) {
    std::string path_file(file);
    struct timespec newest_time = {0};

    struct stat st;
    for (auto name : folders) {
        std::string opened_file = name + path_file;

        if (stat(opened_file.c_str(), &st) == 0 && newest_time < st.st_mtim) {
            newest_time = st.st_mtim;
            original_path = std::move(opened_file);
        }
    }
}

void MyUnionFS::initialize_source(char* source_folder){
    char* shift = source_folder;
    char* prev_find = source_folder;
    char real_path[PATH_MAX];

    while (0 != (shift = strstr(prev_find, ":"))) {
        shift[0] = '\0';
        //realpath() expands all symbolic links and resolves references to /./,
        // /../ and extra '/' characters in the null-terminated string named by
        // path to produce a canonicalized absolute pathname.
        realpath(prev_find, real_path);
        folders.emplace_back(real_path);
        prev_find = shift + 1;
    }

    realpath(prev_find, real_path);
    folders.emplace_back(real_path);
}


int my_stat(const char* path, struct stat* st, struct fuse_file_info *fi) {
    fls.set_file_path(path);
    if (fls.original_path == "")
        return -ENOENT;

    stat(fls.original_path.c_str(), st);

    if (S_ISDIR(st->st_mode))
        st->st_mode &= S_IFDIR | 0555;
    else
        if (S_ISREG(st->st_mode))
            st->st_mode &= S_IFREG | 0444;

    return 0;
}

int my_prev_stat(const char* path, struct stat* st, struct fuse_file_info *fi) {
    if (0 == strcmp(path, "/")) {
        st->st_mode = S_IFDIR | 0444; // regular files 0444 permission
        st->st_nlink = 2;
        return 0;
    }

    if ('/' == path[0]) {
        for (uint64_t i = 0; i < sizeof(path); ++i) {
//            if (strcmp(path + 1, path[i]) == 0) {
//                st->st_mode = S_IFREG | 0444;       // file type - regular, access read only
//                st->st_nlink = 1;                   // one link to file
//                st->st_size = fls[i].size;   // bytes available
//                return 0;
//            }
        }
    }

    return -ENOENT; // error: we have no files other
}

int my_read(const char* path, char* out, size_t size, off_t off, struct fuse_file_info* fi) {
    fls.set_file_path(path);
    if (fls.original_path == "/")
        return -ENOENT;

    struct stat st;
    stat(fls.original_path.c_str(), &st);
    if (off >= st.st_size)
        return 0;

    int file = open(fls.original_path.c_str(), O_RDONLY);
    lseek(file, off, SEEK_SET);
    int read_total, read_curr_bytes = 0;
    while ((read_total = read(file, out + read_curr_bytes, size - read_curr_bytes)) > 0) {
        read_curr_bytes += read_total;
    }
    close(file);

    return read_curr_bytes;
}


int my_readdir(const char* path, void* out, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
    filler(out, ".", NULL, 0, fuse_fill_dir_flags(0));
    filler(out, "..", NULL, 0, fuse_fill_dir_flags(0));

    std::string curr_path(path);

    std::set<std::string> files_name;
    for (auto name : fls.folders) {
        std::string full_path = name + curr_path;
        DIR *pd = opendir(full_path.c_str());
        if (pd != NULL)
            for (dirent *pDirent = readdir(pd); pDirent != NULL; pDirent = readdir(pd))
                files_name.emplace(pDirent->d_name);
        closedir(pd);
    }

    for (const std::string& name : files_name)
        filler(out, name.c_str(), NULL, 0, fuse_fill_dir_flags(0));

    return 0;  // success
}

int my_open(const char *filepath, struct fuse_file_info* fi) {
    fls.set_file_path(filepath);
    if ("" == fls.original_path) {
        return -ENOENT;
    }
    if ((fi->flags & O_ACCMODE) == O_RDONLY) {
        return 0;
    }
    return -EACCES;
}


int main(int argc, char** argv)
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    struct fuse_opt opt_specs[] = {
            { "--src %s", 0, 0 },
            {NULL,0,0}
    };
//    struct fuse_operations operations = {
//            .getattr = my_stat,
//            .open = my_open,
//            .read = my_read,
//            .readdir = my_readdir,
//    };
    struct fuse_operations operations;
    memset(&operations, 0, sizeof(operations));
    operations.getattr = my_stat;
    operations.open    = my_open;
    operations.read    = my_read;
    operations.readdir = my_readdir;


    my_options_t my_options;
    memset(&my_options, 0, sizeof(my_options));

    fuse_opt_parse(&args, &my_options, opt_specs, NULL);

    if (my_options.src) {
        fls.initialize_source(my_options.src);
    }

    int ret = fuse_main(args.argc, args.argv,&operations,NULL);

    return ret;
}

//find_package(PkgConfig REQUIRED)
//pkg_check_modules(FUSE REQUIRED fuse3)kkk
//
//link_libraries(${FUSE_LIBRARIES})         # -lfuse3 -lpthread
//include_directories(${FUSE_INCLUDE_DIRS}) # -I/usr/include/fuse3
//compile_options(${FUSE_CFLAGS_OTHER})     # empty since fuse 3.0

// $ g++ 23-1.cpp -o n -D_FILE_OFFSET_BITS=64

/// to test file system

//# start empty FUSE implementation
//> ./my_filesystem work_dir
//
//# try to get filesystem contents
//> ls work_dir
//ls: cannot access 'work_dir': Function not implemented
//
//# umount filesystem and stop the daemon
//> fusermount3 -u work_dir

/// For test programm you need to make dir "testmount", next compile program
// g++ save.cpp -o n -D_FILE_OFFSET_BITS=64 -pthread -lfuse -lrt -ldl
// ./n testmount
// and you can see it in mounted devices by "$ mount"
// to unmount "$ fusermount3 -u testmount"
