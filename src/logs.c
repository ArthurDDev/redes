#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <string.h>

#include "logs.h"

FILE *log_file;
char log_state = LOP_ALL;

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

void flog(const char *str, char type)
{
    if (log_state == LOP_NO)
        return;

    if (log_state == LOP_DATA && !(
        type == LOG_ERROR || type == LOG_PROGRESS || type != LOG_WARNING || type != LOG_TIMEOUT
    ))
        return;

    switch(type) {
        case LOG_ERROR:
            fprintf(log_file, "[\e[0;91m ERRO\e[0m ] %s\n", str);
        break;
        case LOG_REC:
            fprintf(log_file, "[\e[0;92m RECEBIDO\e[0m ] %s\n", str);
        break;
        case LOG_SENT:
            fprintf(log_file, "[\e[0;95m ENVIADO\e[0m ] %s", str);
        break;
        case LOG_WARNING:
            fprintf(log_file, "[\e[0;93m AVISO\e[0m ] %s\n", str);
        break;
        case LOG_ACK:
            //fprintf(log_file, " [\e[0;92m ACK\e[0m ]\n");
        break;
        case LOG_NACK:
            fprintf(log_file, " [\e[0;93m NACK\e[0m ]");
        break;
        case LOG_TIMEOUT:
            fprintf(log_file, " [\e[0;93m TIMEOUT\e[0m ]");
        break;
        case LOG_PROGRESS:
            fprintf(log_file, " [\e[0;96m ENVIANDO %s\e[0m ]", str);
            int siz = strlen(str) + strlen(" [\e[0;96m ENVIANDO \e[0m ]");
            fseek(log_file, -siz, SEEK_CUR);
        break;
        case LOG_SUCCESS:
            fprintf(log_file, " [\e[0;92m SUCESSO\e[0m ]\n");
        break;
        default:
            fprintf(log_file, "[ LOG ] %s\n", str);
        break;
    }
    fflush(log_file);
}

void end_log()
{
    fclose(log_file);
    log_file = NULL;
}