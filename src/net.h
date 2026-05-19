// Envio e recebimento de mensagens

#ifndef __NET__
#define __NET__

#include "message.h"
#include <stddef.h>

#define MAX_SEQ 8
#define TIMEOUT 2
#define MAX_TIMEOUT 5

struct connection {
    int socket;
    short seq;
    char last_message[3];
};

extern struct connection CON;

// Retorna o próximo seq
char next_seq();

// Inicia a conexão
void setup_connection(char* interface);

// Envia uma mensagem m
// Retorna 0 se sucesso, 1 em erro
char send_message(message m);

// Envia dados grandes, podendo ser separados em varias mensagens
// usa send_message internamente
size_t send_data(message m);

// Recebe mensagens, retorna uma mensagem com tamanho negativo em caso de erro
message receive_data();
message receive_message();

size_t restore_buffer(unsigned char **buffer, size_t size);
size_t format_buffer(unsigned char **buffer, size_t size);


char next_seq();

char get_seq(unsigned char *buffer);

#endif
