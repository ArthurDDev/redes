#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"

// falta tratamento de erro
unsigned char *file_to_buffer(const char *filename, size_t *size) {
    if (!filename){
        fprintf(stderr, "Parâmetros incorretos ao ler arquivo para o buffer.\n");
        exit(1);
    }
    
    FILE *file = fopen(filename, "r");
    if (!file){
        printf("Arquivo inexistente\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    
    unsigned char *buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Erro ao alocar memória para o buffer.\n");
        exit(1);
    }
    
    fseek(file, 0, SEEK_SET);
    
    buffer[0] = filename[0];
    
    fread(buffer + 1, file_size, 1, file);

    fclose(file);

    *size = file_size + 1;

    return buffer;
}

void buffer_to_file(const char *buffer, size_t size) {
    FILE *file;
    char filename[] = "x.fix";

    filename[0] = buffer[0];

    if (!buffer){
        fprintf(stderr, "Parâmetros incorretos ao ler buffer para um arquivo.\n");
        exit(1);
    }

    for (size_t i = 0; i < size; i ++)
        printf("%c", buffer[i]);
    printf("\n");

    file = fopen(filename, "w+");

    fseek(file, 0, SEEK_SET);

    fwrite(buffer + 1, size - 1, 1, file);

    fclose(file);
}