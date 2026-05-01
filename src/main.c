#include "stdio.h"
#include "sys/socket.h"
#include <unistd.h>

#include "net.h"
#include "socket.h"

int main()
{
    setup_connection("lo");

#ifdef SERVER

    char data[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    message m = {100, M_DATA, data};
    send_data(m);

#else

    while (1) {
        message m = receive_data();

        unsigned char *data = m.data;

        // mensagem em normal
        for (size_t i = 0; i < m.size; i ++)
            printf("%c", data[i]);
        printf("\n");
    }

#endif

    return 0;
}
