#include "stdio.h"
#include "sys/socket.h"
#include <unistd.h>

#include "game.h"
#include "net.h"
#include "socket.h"
#include "files.h"

int main()
{
    setup_connection("lo");

#ifdef SERVER

    server_game_loop();

/*
    size_t size;
    unsigned char *buffer = file_to_buffer("arthur.txt", &size);
    send_data((message){size, M_TXT, buffer});
*/

#else

    client_game_loop();

#endif

    return 0;
}
