#include <fcntl.h>
#include <inttypes.h>
#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int is_correct_executable(struct stat current_stat, char* current_path)
{
    if (!(current_stat.st_mode & S_IXGRP || current_stat.st_mode & S_IXOTH ||
          current_stat.st_mode & S_IXUSR)) {
        return 0; // non executable with different permission
    }
    int fd = open(current_path, O_RDONLY);
    if (fd == -1) {
        return 0; // can not open
    }
    char first_line[PATH_MAX + 2];
    if (read(fd, first_line, sizeof(first_line)) == -1) {
        return 0; // problems with reading
    }
    char* nl = memchr(first_line, '\n', sizeof(first_line));
    if (nl) {
        *nl = '\0';
    }
    if (strncmp(first_line, "\177ELF", 4) == 0) {
        return 1; // correct file, standart begining of ELF file
    }
    if (lstat(first_line + 2, &current_stat) == -1 ||
        strncmp(first_line, "#!", 2) != 0) {
        return 0; // do not begin from correct "#!"
    }
    close(fd);
    return is_correct_executable(current_stat, first_line + 2);
}

int main()
{
    char cur_path[PATH_MAX];
    struct stat cur_stat;
    int return_value = 0;
    while (fgets(cur_path, sizeof(cur_path), stdin)) {
        static char* nl;
        nl = memchr(cur_path, '\n', sizeof(cur_path));
        if (nl) {
            *nl = '\0';
        }
        if ((return_value = lstat(cur_path, &cur_stat)) == -1) {
            return 1;
        } else {
            if (cur_stat.st_mode & S_IXUSR || cur_stat.st_mode & S_IXGRP ||
                cur_stat.st_mode & S_IXOTH) {
                if (is_correct_executable(cur_stat, cur_path) != 1) {
                    printf("%s\n", cur_path);
                }
            }
        }
    }
    return 0;
}
