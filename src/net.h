// Envio e recebimento de mensagens

#ifndef __NET__
#define __NET__

#include "message.h"
#include <stddef.h>


struct connection {
    int socket;
    short seq;
};

extern struct connection CON;

// Inicia a conexão
void setup_connection(char* interface);

// Envia uma mensagem m
// Retorna 0 se sucesso, 1 em erro
size_t send_message(message m);

// Envia dados grandes, podendo ser separados em varias mensagens
// usa send_message internamente
size_t send_data(message m);

// Recebe uma mensagem
message recieve_data(int socket);

#endif