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
    unsigned char *buffer = file_to_message("foto.jpg", &size);
    send_data((message){size, M_JPG, buffer});
    

#else

    while (1) {
        message m = receive_data();

        unsigned char *data = m.data;

        message_to_file(m);
        //xdg-open foto.jpg
        //fork(); //execv

        // mensagem em normal
        for (size_t i = 0; i < m.size; i ++)
            printf("%c", data[i]);
        printf("\n");
    }

#endif

    return 0;
}
