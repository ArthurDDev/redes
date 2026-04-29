#include "message.h"
#include "net.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

size_t create_frame(message m, void **dest)
{

    size_t size = m.size + 4;
    char *str = malloc(size);
    if (!str) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(1);
    }
    char seq = next_seq();

    str[0] = 0b01111110;
    str[1] = m.size << 3 | seq >> 5;
    str[2] = seq << 3 | m.type;
    str[size-1] = 0; // TODO: CRC

    memcpy(&str[3], m.data, m.size);

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

    char *frame = src;
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