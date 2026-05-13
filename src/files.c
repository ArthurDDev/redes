#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "files.h"
#include "message.h"

// falta tratamento de erro
unsigned char *file_to_message(const char *filename, size_t *size) {
    if (!filename || !size)  {
        fprintf(stderr, "Parâmetros incorretos ao ler arquivo para o buffer.\n");
        exit(1);
    }
    
    FILE *file = fopen(filename, "rb");
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
    
    if(fread(buffer + 1, file_size, 1, file) != 1) {
        fprintf(stderr, "Erro ao ler arquivo para o buffer.\n");
        exit(1);
    }

    fclose(file);

    *size = file_size + 1;

    return buffer;
}

void message_to_file(message m) {
    if (!m.data || m.size == 0) {
        fprintf(stderr, "Parâmetros incorretos ao ler buffer para um arquivo.\n");
        exit(1);
    }

    FILE *file;
    char filename[32];
    
    filename[0] = m.data[0];
    filename[1] = '.';
    
    switch (m.type) {
        case M_TXT:
            printf("Criando arquivo .txt\n");
            filename[2] = 't';
            filename[3] = 'x';
            filename[4] = 't';
            break;
        case M_JPG:
            printf("Criando arquivo .jpg\n");
            filename[2] = 'j';
            filename[3] = 'p';
            filename[4] = 'g';
            break;
        case M_MP4:
            printf("Criando arquivo .mp4\n");
            filename[2] = 'm';
            filename[3] = 'p';
            filename[4] = '4';
            break;

        default:
            printf("Arquivo de tipo desconhecido\n");
    }

    filename[5] = '\0';

    //for (size_t i = 0; i < m.size; i ++)
    //    printf("%02x ", m.data[i]);
    //printf("\n");

    file = fopen(filename, "wb");

    fseek(file, 0, SEEK_SET);

    fwrite(m.data + 1, m.size - 1, 1, file);

    fclose(file);
}

void open_file(char *filename)
{
    int id = fork();

    if (id < 0) {
        fprintf(stderr, "Erro ao criar processo filho.\n");
        return;
    }

    // isso quer dizer que é o processo filho
    if (id == 0) {
        char *args[] = {"/usr/bin/xdg-open", filename, NULL};

        execv(args[0], args);

        fprintf(stderr, "Erro ao abrir arquivo.\n");
        exit(1);
    }
}