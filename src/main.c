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

    const char *map = "";

    if (argc == 3)
        map = argv[2];
    
    server_game_loop(map);

#else
    
    client_game_loop();

#endif

    return 0;
}
