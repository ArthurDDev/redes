#include "message.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

size_t create_message(message m, void **dest)
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
    str[size-1] = 0; // TODO CRC

    // copia os dados da mensgaem para a string
    memcpy(&str[3], m.data, m.size);

    *dest = str;
    return size;
}