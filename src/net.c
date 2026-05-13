#include "net.h"
#include "socket.h"
#include "message.h"

#include <string.h>
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

    if (send(CON.socket, buffer, 20, 0) == -1) {
        fprintf(stderr, "Erro ao enviar ACK\n");
        return;
    }
}

void send_nack()
{
    void *buffer = malloc(20);
    create_frame((message){0, M_NACK, NULL}, &buffer);

    printf("ENVIANDO NACK\n");

    if (send(CON.socket, buffer, 20, 0) == -1) {
        fprintf(stderr, "Erro ao enviar NACK\n");
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

message receive_message()
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
        if (!validade_frame(m.data, m.data + 4)) {
            send_nack();
            continue;
        }

        if (m.type == M_ACK || m.type == M_NACK)
            continue;
        CON.seq = get_seq(buffer);

        break;
    }

    if (m.type != M_ACK && m.type != M_NACK)
        send_ack();

    increment_seq();

    return m;
}

message receive_data()
{
    message m = receive_message();

    if (!(m.type == M_VIS
        || m.type == M_DATA
        || m.type == M_TXT
        || m.type == M_JPG
        || m.type == M_MP4 )) {
            return m;
    }

    message ml = {m.size, m.type, malloc(m.size)};
    memcpy(ml.data, m.data, m.size);
    delete_message(&m);

    do {
        m = receive_message();
        ml.size += m.size;
        ml.data = realloc(ml.data, ml.size);
        memcpy(ml.data + ml.size - m.size, m.data, m.size);
        delete_message(&m);
    } while (m.type != M_END);

    return ml;
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

size_t send_data(message m)
{
    int64_t size_left = m.size;

    message t;
    t.data = m.data;
    do {
        t.type = m.type;
        if (size_left > MAX_DATA)
            t.size = MAX_DATA;
        else
            t.size = size_left;

        t.data = &m.data[m.size - size_left];

        send_message(t);

        size_left -= MAX_DATA;
    } while (size_left > 0);

    if (m.type == M_VIS
        || m.type == M_DATA
        || m.type == M_TXT
        || m.type == M_JPG
        || m.type == M_MP4)
        send_message((message){0, M_END, NULL});

    return 1;
}
