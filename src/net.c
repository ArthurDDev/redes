#include "net.h"
#include "socket.h"
#include "message.h"
#include "logs.h"

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
}

int validate_header(unsigned char *buffer)
{
    if (buffer[0] != 0b01111110)
        return 1;

    if (CON.seq != seq_from_buffer(buffer))
        return 1;

    return 0;
}

char increment_seq()
{
    CON.seq = (CON.seq + 1) % MAX_SEQ;

    return CON.seq;
}

char next_seq()
{
    return CON.seq;
}

char seq_from_buffer(unsigned char *buffer)
{
    return (buffer[1] << 3 | buffer[2] >> 5) & 0b00111111;
}

void send_ack()
{
    unsigned char *buffer = malloc(20);
    create_frame((message){0, M_ACK, NULL}, &buffer);

    if (send(CON.socket, buffer, 20, 0) == -1) {
        fprintf(stderr, "Erro ao enviar ACK\n");
        return;
    }
}

void send_nack()
{
    unsigned char *buffer = malloc(20);
    create_frame((message){0, M_NACK, NULL}, &buffer);

    printf("ENVIANDO NACK\n");

    if (send(CON.socket, buffer, 20, 0) == -1) {
        fprintf(stderr, "Erro ao enviar NACK\n");
        return;
    }
}

char recieve_ack(char seq)
{
    message m;
    unsigned char buffer[64];

    time_t start_time, current_time;
    time(&start_time);
    while (1) {
        time(&current_time);
        if (current_time - start_time > TIMEOUT) {
            flog("", LOG_TIMEOUT);
            break;
        }
        if (recv(CON.socket, buffer, 64, 0) == -1) {
            continue;
        }

        if (validate_header(buffer))
            continue;

        m = decode_message(buffer);
        if (seq_from_buffer(buffer) != seq) {
		delete_message(&m);
            continue;
        }

        if (m.type == M_ACK) {
            flog("", LOG_ACK);
            delete_message(&m);
            return 1;
        }

        if (m.type == M_NACK) {
            flog("", LOG_NACK);
            delete_message(&m);
            return 0;
        }
    }
    
    return 0;
}

char send_message(message m)
{
    unsigned char *buffer;
    size_t siz = create_frame(m, &buffer);
    if (siz < MIN_SIZE) {
        buffer = realloc(buffer, MIN_SIZE);
        siz = MIN_SIZE;
    }

    siz = format_buffer(&buffer, siz);

    int timeouts = 0;
    do {
        timeouts ++;

        if (timeouts > MAX_TIMEOUT) {
            fprintf(stderr, "Timeout\n");
            exit(1);
        }

        if (send(CON.socket, buffer, siz, 0) == -1) {
            fprintf(stderr, "Erro ao enviar mensagem\n");
            buffer = delete_frame(buffer);
            return 1;
        }

        if (m.type == M_ACK || m.type == M_NACK) {
            buffer = delete_frame(buffer);
            return 0;
        }
    } while (!recieve_ack(CON.seq));

    increment_seq();

    return 0;

}

message receive_message()
{
    message m;

    unsigned char *buffer = malloc(64);
    if (buffer == NULL) {
        fprintf(stderr, "Erro ao alocar memoria\n");
        exit(1);
    }

    while (1) {

        if (recv(CON.socket, buffer, 64, 0) == -1) {
            continue;
        }

        restore_buffer(&buffer, 64);

        if (validate_header(buffer))
            continue;

        m = decode_message(buffer);
        if (!validate_frame(buffer, m.size + 4)) {
            send_nack();
            continue;
        }

        if (m.type == M_ACK || m.type == M_NACK)
            continue;
        CON.seq = seq_from_buffer(buffer);

        break;
    }

    buffer = delete_frame(buffer);

    if (m.type != M_ACK && m.type != M_NACK)
        send_ack();

    increment_seq();

    return m;
}

void send_file_data(message m)
{
    int64_t size_left = m.size;

    message t;

    t.data = malloc(sizeof(size_t) + 1);
    t.type = m.type;
    size_left -= 1;
    t.data[0] = m.data[0];
    for (size_t i = 1; i <= sizeof(size_t); i++)
        t.data[i] = size_left >> (8 * (sizeof(size_t) - i));

    t.size = 1 + sizeof(size_t);
    send_message(t);
    free(t.data);

    //int last_percentage = 0;

    do {
        t.type = M_DATA;
        if (size_left > MAX_DATA)
            t.size = MAX_DATA;
        else
            t.size = size_left;

        t.data = &m.data[m.size - size_left];

        send_message(t);

        /*
        int current_percentage = (m.size - size_left) * 100 / m.size;
        if (current_percentage > last_percentage) {
            char msg[50];
            snprintf(msg, sizeof(msg), "Enviando arquivo: %d%%", current_percentage);
            flog(msg, LOG_PROGRESS);
            last_percentage = current_percentage;
        }
        */

        size_left -= MAX_DATA;
    } while (size_left > 0);

    send_message((message){0, M_END, NULL});
    //flog("Enviando arquivo: 100%%", LOG_PROGRESS);
}

void send_any_data(message m)
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

    if (m.type == M_VIS)
        send_message((message){0, M_END, NULL});

}

size_t send_data(message m)
{
    log_state = LOP_DATA;

    if (is_file(m)) {
        flog("ARQUIVO", LOG_PROGRESS);
        send_file_data(m);
    }
    else {
        send_any_data(m);
        flog("DADOS", LOG_PROGRESS);
    }
    
    log_state = LOP_ALL;
    flog("", LOG_SUCCESS);

    return 1;
}


message receive_data()
{
    log_state = LOP_DATA;

    message m = receive_message();

    if (!(m.type == M_VIS
        || m.type == M_DATA
        || m.type == M_TXT
        || m.type == M_JPG
        || m.type == M_MP4 )) {

            log_state = LOP_ALL;
            return m;
    }

    message ml;
    if (is_file(m)) {
        ml = (message){1, m.type, malloc(1)};
        ml.data[0] = m.data[0];
    }
    else {
        ml = (message){m.size, m.type, malloc(m.size)};
        memcpy(ml.data, m.data, m.size);
    }
    delete_message(&m);

    do {
        m = receive_message();
        ml.size += m.size;
        ml.data = realloc(ml.data, ml.size);
        memcpy(ml.data + ml.size - m.size, m.data, m.size);
        delete_message(&m);
    } while (m.type != M_END);

    log_state = LOP_ALL;

    return ml;
}
