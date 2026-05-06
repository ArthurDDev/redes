//  Tudo que tem a ver com mensagem e manipulação dos bytes

#ifndef __MESSAGE__
#define __MESSAGE__

#include <stddef.h>

#define MIN_SIZE 14
#define MAX_DATA 31

// Estrutura do frame:
//
// Marcador de Início | Tamanho | Sequencia | Tipo   | Dados   | CRC
// 1 byte             | 5 bits  | 6 bits    | 5 bits | n bytes | 1 byte

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
    unsigned char *data; // Buffer de dados
};
typedef struct message message;

// Cria o frame no buffer void a partir dos dados da mensagem m.
// Retorna o tamanho da mensagem inteira em bytes ou -1 em erro
size_t create_frame(message m, void **dest);

// Destroi um frame, retorna NULL
void *delete_frame(void *dest);

// Deleta uma mensagem
void delete_message(message *m);

// Decodifica a mensagem do buffer src
// Retorna uma struct com os dados
message decode_message(void *src);

#endif