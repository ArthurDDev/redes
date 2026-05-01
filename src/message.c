#include "message.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

size_t create_frame(message m, void **dest)
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

    unsigned char seq = 0;

    str[0] = 0b01111110;
    // bits mais significativos | bits menos significativos
    str[1] = m.size << 3 | seq >> 5;
    str[2] = seq << 3 | m.type;
    
    // copia os dados da mensagem para a string
    memcpy(&str[3], m.data, m.size);

    str[size-1] = get_crc(str, size - 1); 

    *dest = str;
    return size;
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
    m.data = malloc(m.size);
    if (m.data == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(1);
    }

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

    printf("Nosso CRC: %x\n", crc);
    return crc;
}


