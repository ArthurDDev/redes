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
