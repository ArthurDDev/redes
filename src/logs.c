#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/statvfs.h>

#include "logs.h"

FILE *log_file;

void start_log(char *filename)
{
    log_file = fopen(filename, "w+");

    if (log_file == NULL) {
        fprintf(stderr, "Erro ao criar arquivo de logs\n");
        exit(1);
    }

    int id = fork();

    if (id < 0) {
        fprintf(stderr, "Erro ao criar processo filho.\n");
        return;
    }

    if (id == 0) {
        char *args[] = {"/usr/bin/kitty", "tail", "-f", filename, NULL};

        execv(args[0], args);

        fprintf(stderr, "Erro ao abrir arquivo de logs.\n");
        exit(1);
    }

}

void flog(const char *str)
{
    fprintf(log_file, "%s", str);
}

void end_log()
{
    fclose(log_file);
    log_file = NULL;
}