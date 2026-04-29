#include "net.h"
#include "socket.h"

struct connection CON = {0, 0};

void setup_connection(char* interface)
{
    CON.socket = cria_raw_socket(interface);
    CON.seq = 0;
}
