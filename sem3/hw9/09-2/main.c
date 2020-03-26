#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char** argv)
{
    pid_t pid = 0;
    int word_count = 0;
    int status;
    char input_word[4096];
    int sum_word_count = 0;

    pid = fork();

    if (pid == 0) {
        do {
            pid = fork();
            if (pid == 0) {
                if (scanf("%s", input_word) != EOF) {
                    word_count++;
                } else {
                    exit(word_count);
                }
            }
        } while (pid == 0);
        if (pid > 0) {
            waitpid(pid, &status, 0);
            sum_word_count = WEXITSTATUS(status);
            exit(sum_word_count);
        }
    } else {
        waitpid(pid, &status, 0);
        sum_word_count = WEXITSTATUS(status);
        printf("%d\n", sum_word_count);
    }

    return 0;
}