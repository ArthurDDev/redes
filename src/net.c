#include "net.h"
#include "socket.h"
#include "message.h"

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

struct connection CON;

void setup_connection(char* interface)
{
    CON.socket = cria_raw_socket(interface);
    CON.seq = 0;
    for (int i = 0; i < 3; i ++)
        CON.last_message[i] = 0;
}

// Compara o cabeçalho é valido
// Retorna 1 se é inválido, 0 se é válido
int validate_header(unsigned char *buffer)
{
    if (buffer[0] != 0b01111110)
        return 1;

    printf("Header Repitido\n");

    if (buffer[0] == CON.last_message[0] &&
        buffer[1] == CON.last_message[1] &&
        buffer[2] == CON.last_message[2])
        return 1;
    
    return 0;
}

// Salva o cabeçalho da última mensagem lida
void save_header(unsigned char *buffer)
{
    for (int i = 0; i < 3; i ++)
        CON.last_message[i] = buffer[i];
}

message receive_data()
{ //TODO: CRC
  //TODO: Mensagens maiores
  //TODO: Timeout
    message m;

    unsigned char *buffer = malloc(64);
    if (buffer == NULL) {
        printf("Erro ao alocar memoria\n");
        exit(1);
    }

    printf("Lendo dados...\n");
    while (1) {
        if (recv(CON.socket, buffer, 64, 0) == -1) {
            fprintf(stderr, "Erro ao ler frame\n");
            continue;
        }

        if (validate_header(buffer))
            continue;

        save_header(buffer);

        m = decode_message(buffer);
        break;
    }

    free(buffer);

    if (m.type == M_ACK || m.type == M_NACK)
        send_message((message){0, M_ACK, NULL});

    return m;
}

// IMPLEMENTADO DO JEITO BURRO IDIOTA INEFICIENTE SEM JANELA DESLIZANTE
char next_seq()
{
    return (CON.seq + 1) % MAX_SEQ;
}

char send_message(message m)
{
    void *buffer;
    size_t siz = create_frame(m, &buffer);
    if (siz < MIN_SIZE) {
        buffer = realloc(buffer, MIN_SIZE);
        siz = MIN_SIZE;
    }

    if (send(CON.socket, buffer, siz, 0) == -1) {
        fprintf(stderr, "Erro ao enviar mensagem\n");
        return 1;
    }

    if (m.type == M_ACK || m.type == M_NACK)
        return 0;

    message r;

    while (1) {
        r = receive_data();

        if (r.type == M_ACK)
            break;

        if (r.type != M_NACK) {
            delete_message(&r);
            break;
        }

        if (send(CON.socket, buffer, siz, 0) == -1) {
            fprintf(stderr, "Erro ao enviar mensagem\n");
            return 1;
        }
    }
    CON.seq ++;
    delete_message(&r);

    return 0;

}