#include "stdio.h"
#include "sys/socket.h"
#include <unistd.h>
#include <time.h>

#include "game.h"
#include "net.h"
#include "socket.h"
#include "files.h"

int main(int argc, char **argv)
{
    if (argc <= 1){
        fprintf(stderr, "Fornecer a interface de rede como argumento\n");
        exit(1);
    }

    setup_connection(argv[1]);

    srand(time(NULL));

#ifdef SERVER

    server_game_loop();

    // size_t s;
    // unsigned char *buffer = file_to_message("foto.jpg", &s);
    // send_data((message){s, M_JPG, buffer});

#else
    
    client_game_loop();

    // message m = receive_data();
    // message_to_file(m);

#endif

    return 0;
}
