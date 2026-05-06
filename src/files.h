#ifndef __FILES__
#define __FILES__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"

// lê um arquivo e retorna um ponteiro para os dados e o tamanho do arquivo
unsigned char *file_to_message(const char *filename, size_t *size);

// lê um buffer e cria um arquivo 
void message_to_file(message m);

#endif