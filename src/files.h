#ifndef __FILES__
#define __FILES__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// lê um arquivo e retorna um ponteiro para os dados e o tamanho do arquivo
unsigned char *file_to_buffer(const char *filename, size_t size);

// lê um buffer e cria um arquivo 
void buffer_to_file(const char *buffer);

#endif