#ifndef __LOGS__
#define __LOGS__

#include <stdio.h>

extern FILE *log_file;
extern char log_state;

// Faz log de todas as mensagens enviadas
// Fica tudo mais lento de tanto print
#define LOG_EVERYTHING 0

enum log_type {
    LOG_PROGRESS,
    LOG_SENT,
    LOG_REC,
    LOG_WARNING,
    LOG_ERROR,
    LOG_ACK,
    LOG_NACK,
    LOG_TIMEOUT,
    LOG_SUCCESS
};

enum log_states {
    LOP_ALL,
    LOP_DATA,
    LOP_NO
};

void start_log(char *filename);
void flog(const char *str, char flag);
void end_log();

#endif