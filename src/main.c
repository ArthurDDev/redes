#include "stdio.h"
#include "sys/socket.h"
#include <unistd.h>

#include "net.h"
#include "socket.h"
#include "files.h"

int main()
{
    setup_connection("lo");

#ifdef SERVER

    size_t size;
    unsigned char *buffer = file_to_buffer("arthur.txt", &size);
    send_data((message){size, M_TXT, buffer});
    

#else

    while (1) {
        message m = receive_data();

        unsigned char *data = m.data;

        buffer_to_file(m.data, m.size);
        
        // mensagem em normal
        for (size_t i = 0; i < m.size; i ++)
            printf("%c", data[i]);
        printf("\n");
    }

#endif

    return 0;
}
