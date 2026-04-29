#include "stdio.h"
#include "sys/socket.h"

#include "net.h"
#include "socket.h"

int main()
{
    setup_connection("lo");

#ifdef SERVER

    char data[] = "eusouumamensagembemlonga";
    message m = {20, M_DATA, data};
    void *buffer;
    int tam = create_message(m, &buffer);
    int response;

    response = send(CON.socket, buffer, tam, 0);

    if (response == -1){
        // kkkkkkk olha o comentário do cara
        printf("Deu merda\n");
    }

    unsigned char *x = buffer;
    for (int i = 0; i < 24; i ++)
        printf("%x ", x[i]);
    printf("\n");

#else

    while (1) {
        char buffer[25];
        if (recv(CON.socket, &buffer, 24, 0) == -1)
            printf("Erro\n");
        else {
            for (int i = 0; i < 24; i ++)
                printf("%x ", buffer[i]);
            printf("\n");
            break;
        }
    }

#endif

    return 0;
}
