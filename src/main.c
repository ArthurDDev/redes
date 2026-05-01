#include "stdio.h"
#include "sys/socket.h"
#include <unistd.h>


#include "net.h"
#include "socket.h"

int main()
{
    setup_connection("lo");

#ifdef SERVER

    unsigned char data[] = "eusouumamensagembemlonga";
    message m = {20, M_DATA, data};
    send_message(m);
    sleep(1);
    send_message(m);

#else

    while (1) {
       message m = receive_data();

        printf("Mensagem:\nTamanho: %ld | Tipo: %d | Dados:\n", m.size, m.type);
        unsigned char *data = m.data;

        // mensagem em hexa
        for (size_t i = 0; i < m.size; i ++)
            printf("%x ", data[i]);
        printf("\n");

        // mensagem normal
        for (size_t i = 0; i < m.size; i ++)
            printf("%c", data[i]);
        printf("\n");
    }

#endif

    return 0;
}
