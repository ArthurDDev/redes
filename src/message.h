//  Tudo que tem a ver com mensagem e manipulação dos bytes

#ifndef __MESSAGE__
#define __MESSAGE__

#include <stddef.h>

enum message_type {
    M_ACK = 0,
    M_NACK = 1,
    M_VIS = 2,
    M_INIT = 3,
    M_DATA = 4,
    M_TXT = 5,
    M_JPG = 6,
    M_MP4 = 7,
    M_RIGHT = 10,
    M_LEFT = 11,
    M_UP = 12,
    M_DOWN = 13,
    M_ERR = 15,
    M_END = 16,
};

// Representa os dados de uma transmissão, NÃO É O CORPO DA MENSAGEM EM SI,
// por isso size_t em size, para permitir envio de arquivos grandes
struct message
{
    size_t size; // Tamanho dos DADOS em bytes
    char type;  // Tipo conforme o enum
    void *data; // Buffer de dados
};
typedef struct message message;

// Cria a mensagem no buffer void a partir dos dados da mensagem m.
// Retorna o tamanho da mensagem inteira em bytes ou -1 em erro
size_t create_message(message m, void *dest);

// Destroi uma mensagem, retorna NULL
void *delete_message(void *dest);

// Decodifica a mensagem do buffer src do tamanho tam em bytes
// Retorna uma struct com os dados
message decode_message(void *src, size_t tam);

#endif