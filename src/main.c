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
    message m = {24, M_DATA, data};
    for (int i = 0; i < 20; i ++)
        send_message(m);

#else

    while (1) {
        message m = receive_data();

        unsigned char *data = m.data;

        // mensagem em hexa
        for (size_t i = 0; i < m.size; i ++)
            printf("%c", data[i]);
        printf("\n");
    }

#endif

    return 0;
}
