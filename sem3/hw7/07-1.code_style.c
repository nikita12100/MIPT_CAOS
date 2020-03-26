#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

struct Item {
    int value;
    uint32_t next_pointer;
};

int main(int argc, char** argv)
{
    int in_file = open(argv[1], O_RDONLY);
    if (in_file == -1) {
        goto finaly;
    }
    ssize_t in_read = 0;
    struct Item item;
    while ((in_read = read(in_file, &item, sizeof(item))) > 0) {
        off_t in = lseek(in_file, item.next_pointer, SEEK_SET);
        if (in != -1) { // all OK
            printf("%d ", item.value);
        } else { // smth wrong
            break;
        }
        if (item.next_pointer == 0) { // end of reading
            break;
        }
    }
finaly:
    close(in_file);
    return 0;
}
