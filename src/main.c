#include "stdio.h"
#include "sys/socket.h"

#include "net.h"
#include "socket.h"

int main()
{
    setup_connection("lo");

#ifdef SERVER

    char data[] = "mensagemahdalsdhalsdjhlj";
    if (send(CON.socket, data, 20, 0) == -1){
        printf("Deu merda\n");
    }

#else

    while (1) {
        char buffer[20];
        if (recv(CON.socket, &buffer, 20, 0) == -1)
            printf("Erro\n");
        else {
            printf(buffer);
            break;
        }
    }

#endif


    return 0;
}
