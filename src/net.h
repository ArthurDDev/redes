// Envio e recebimento de mensagens

#ifndef __NET__
#define __NET__

#include "message.h"
#include <stddef.h>

#define MAX_SEQ 8
#define MIN_SIZE 14

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
message recieve_data();

#endif