#ifndef __LOGS__
#define __LOGS__

#include <stdio.h>

extern FILE *log_file;

void start_log(char *filename);
void flog(const char *str);
void end_log();

#endif