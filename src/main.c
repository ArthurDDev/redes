#include "stdio.h"
#include "sys/socket.h"
#include <unistd.h>


#include "net.h"
#include "socket.h"

int main()
{
    setup_connection("lo");

#ifdef SERVER

    char data[] = "eusouumamensagembemlonga";
    message m = {20, M_DATA, data};
    send_message(m);
    sleep(1);
    send_message(m);

#else

    while (1) {
       message m = recieve_data();

        printf("Mensagem:\nTamanho: %ld | Tipo: %d, Dados:\n", m.size, m.type);
        unsigned char *data = m.data;
        for (int i = 0; i < m.size; i ++)
            printf("%x ", data[i]);
        printf("\n");
    }

#endif


    return 0;
}
