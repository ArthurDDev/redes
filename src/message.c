#include "message.h"
#include "net.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

size_t create_frame(message m, unsigned char **dest)
{
    // Marcador de Início |   Tamanho   |   Sequencia   |   Tipo   |   Dados   |   CRC
    //       1 byte       |   5 bits    |    6 bits     |  5 bits  |  n bytes  |  1 byte

    // tamanho da mensagem + 4 bytes (marcador, tamanho,sequencia , tipo, CRC) 
    size_t size = m.size + 4;
    unsigned char *str = malloc(size);
    if (!str) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(1);
    }

    unsigned char seq = next_seq();

    str[0] = 0b01111110;
    // bits mais significativos | bits menos significativos
    str[1] = (m.size << 3) | (seq >> 3);
    str[2] = (seq << 5) | m.type;

    // copia os dados da mensgaem para a string
    memcpy(&str[3], m.data, m.size);

    str[size-1] = get_crc(str, size - 1); 

    *dest = str;
    return size;
}

int validate_frame(unsigned char *frame, size_t size)
{
    if (!frame ) {
        fprintf(stderr, "Ponteiro nulo\n");
        exit(1);
    }

    unsigned char crc = get_crc(frame, size-1);

    if (crc != frame[size-1])
        return 0;

    return 1;
}

void *delete_frame(void *dest)
{
    if (dest != NULL)
        free(dest);
    return NULL;
}

void delete_message(message *m)
{
    if (m == NULL)
        return;

    if (m->data != NULL)
        free(m->data);

    m->data = NULL;
    m->size = 0;
}

message decode_message(void *src)
{
    if (src == NULL) {
        fprintf(stderr, "Ponteiro de mensagem inválido\n");
        exit(1);
    }
    unsigned char *frame = src;
    message m;
    m.size = frame[1] >> 3;
    if (m.size != 0) {
        m.data = malloc(32);
        if (m.data == NULL) {
            fprintf(stderr, "Erro ao alocar memória\n");
            exit(1);
        }
    }
    else
        m.data = NULL;
    memcpy(m.data, &frame[3], m.size);
    m.type = frame[2] & 0b00011111;
    return m;
}

uint8_t get_crc(const uint8_t *data, size_t size)
{
    //valor inicial
    uint8_t crc = 0x00;
    //polinomio gerador
    uint8_t polinomio = 0xD5;

    for (size_t i = 0; i < size; i++){
        crc ^= data[i];
        
        for (uint8_t bit = 0; bit < 8; bit++){
            if (crc & 0x80) {
                crc = (crc << 1) ^polinomio;
            }
            else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

size_t format_buffer(unsigned char **buffer, size_t size)
{
    *buffer = realloc(*buffer, size * 2);


    size_t siz = size;
    for (size_t i = 0; i < size * 2 - 1; i ++) {
        if ((*buffer)[i] == 0x88 || (*buffer)[i] == 0x81) {
            siz ++;
            for (size_t j = size * 2 - 2; j > i; j --)
                (*buffer)[j + 1] = (*buffer)[j];
            (*buffer)[i + 1] = 0xFF;
        }
    }


    return siz;
}

size_t restore_buffer(unsigned char **buffer, size_t size)
{
    size_t siz = size;
    for (size_t i = 1; i < size; i ++) {
        if ((*buffer)[i] == 0xFF && ((*buffer)[i-1] == 0x88 || (*buffer)[i-1] == 0x81)) {
            siz --;
            for (size_t j = i; j < size - 1; j ++)
                (*buffer)[j] = (*buffer)[j + 1];
        }
    }
    return siz;
}

char is_file(message m) {
    if (m.type == M_TXT
        || m.type == M_JPG
        || m.type == M_MP4)
        return 1;
    
    return 0;
}