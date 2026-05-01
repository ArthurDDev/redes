#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"

// falta tratamento de erro
unsigned char *file_to_buffer(const char *filename, size_t size) {
    if (!filename || !size){
        fprintf(stderr, "Parâmetros incorretos ao ler arquivo para o buffer.\n");
        exit(1);
    }
    
    FILE *file = fopen(filename, "r");
    if (!file){
        printf("Arquivo inexistente\n");
        exit(1);
    }
    
    unsigned char *buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Erro ao alocar memória para o buffer.\n");
        exit(1);
    }

    buffer[0] = filename[0];

    fseek(file, 0, SEEK_END);

    fread(buffer + 1, size, 1, file);

    fclose(file);

    return buffer;
}

void buffer_to_file(const char *buffer) {
    FILE *file;
    char filename[] = "x.fix";
    size_t file_size;

    filename[0] = buffer[0];

    if (!buffer){
        fprintf(stderr, "Parâmetros incorretos ao ler buffer para um arquivo.\n");
        exit(1);
    }

    file = fopen(filename, "w");

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);

    fwrite(buffer + 1, file_size, 1, file);

    fclose(file);

    abrir_arquivo();
}