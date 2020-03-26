#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

int main()
{
    char buffer[100000];
    char programm[15000];
    char* program_pat = "#include <stdio.h>\n"
                        "int main()\n"
                        "{\n"
                        "  int res = (%s);\n"
                        "  printf(\"%%d\", res);\n"
                        "  return 0;\n"
                        "}";
    const char* file_name = "my_programm.c";
    fgets(buffer, sizeof(buffer), stdin);
    char* slash_n_pos = strchr(buffer, '\n');
    if (slash_n_pos) {
        *slash_n_pos = '\0';
    }
    if (0 == strnlen(buffer, sizeof(buffer))) {
        return 0;
    }
    FILE* file;
    file = fopen(file_name, "w");
    fprintf(file, program_pat, buffer);
    fclose(file);

    if (0 == fork()) {
        execlp("gcc", "gcc", file_name, NULL);
        perror("exec");
        exit(1);
    }
    waitpid(-1, NULL, 0);
    if (0 == fork()) {
        execlp("./a.out", "./a.out", NULL);
        perror("exec");
        exit(1);
    }
    waitpid(-1, NULL, 0);
    unlink(file_name);
    unlink("a.out");
}