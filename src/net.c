#include "net.h"
#include "socket.h"
#include "message.h"

#include <time.h>
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


    message m = decode_message(buffer);

    if (CON.seq != get_seq(buffer))
        return 1;

    if (buffer[0] == CON.last_message[0] &&
        buffer[1] == CON.last_message[1] &&
        buffer[2] == CON.last_message[2]) {
            
        return 1;

    }
    
    return 0;
}

// Salva o cabeçalho da última mensagem lida
void save_header(unsigned char *buffer)
{
    for (int i = 0; i < 3; i ++)
        CON.last_message[i] = buffer[i];
}

char increment_seq()
{
    CON.seq = (CON.seq + 1) % MAX_SEQ;

    return CON.seq;
}

void send_ack()
{
    void *buffer = malloc(20);
    create_frame((message){0, M_ACK, NULL}, &buffer);

    //printf("Enviando ACK\n");

    if (send(CON.socket, buffer, 20, 0) == -1) {
        fprintf(stderr, "Erro ao enviar mensagem\n");
        return;
    }
}

char get_seq(unsigned char *buffer)
{
    return (buffer[1] << 3 | buffer[2] >> 5) & 0b00111111;
}


char recieve_ack(char seq)
{
    message m;

    unsigned char *buffer = malloc(64);
    if (buffer == NULL) {
        printf("Erro ao alocar memoria\n");
        exit(1);
    }

    time_t start_time, current_time;
    time(&start_time);
    while (1) {
        time(&current_time);
        if (current_time - start_time > TIMEOUT) {
            fprintf(stderr, "timeoutizinho\n");
            break;
        }

        if (recv(CON.socket, buffer, 64, 0) == -1) {
            continue;
        }

        if (validate_header(buffer))
            continue;

        save_header(buffer);
        m = decode_message(buffer);

        if (get_seq(buffer) != seq)
            continue;

        if (m.type == M_ACK)
            return 1;

        if (m.type == M_NACK)
            return 0;
    }

    return 0;

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

    while (1) {

        if (recv(CON.socket, buffer, 64, 0) == -1) {
            continue;
        }

        if (validate_header(buffer))
            continue;
        save_header(buffer);


        m = decode_message(buffer);

        if (m.type == M_ACK || m.type == M_NACK)
            continue;
        CON.seq = get_seq(buffer);

        break;
    }

    printf("Mensagem Recebida com tipo: %d e seq: %d \n", m.type, get_seq(buffer));

    if (m.type != M_ACK && m.type != M_NACK)
        send_ack();

    increment_seq();

    return m;
}

// IMPLEMENTADO DO JEITO BURRO IDIOTA INEFICIENTE SEM JANELA DESLIZANTE
char next_seq()
{
    return CON.seq;
}

char send_message(message m)
{
    void *buffer;
    size_t siz = create_frame(m, &buffer);
    if (siz < MIN_SIZE) {
        buffer = realloc(buffer, MIN_SIZE);
        siz = MIN_SIZE;
    }

    save_header(buffer);

    int timeouts = 0;
    do {
        timeouts ++;
        if (timeouts > MAX_TIMEOUT) {
            fprintf(stderr, "Timeout\n");
            exit(1);
        }
        if (send(CON.socket, buffer, siz, 0) == -1) {
            fprintf(stderr, "Erro ao enviar mensagem\n");
            return 1;
        }

        if (m.type == M_ACK || m.type == M_NACK)
            return 0;
    } while (!recieve_ack(CON.seq));

    //delete_message(&r);

    increment_seq();

    return 0;

}