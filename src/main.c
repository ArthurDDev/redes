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

    char data[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    message m = {100, M_DATA, data};
    
    size_t size;
    unsigned char *buffer = file_to_buffer("arthur.txt", &size);
    message r = {size, M_TXT, buffer};

    for (size_t i = 0; i < r.size; i ++)
        printf("%c", buffer[i]);
    printf("\n");

    send_data(r);
    send_data(m);

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
