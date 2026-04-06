#include "stdio.h"

int main()
{

#ifdef SERVER
    printf("Eu sou o servidor\n");
#else
    printf("Eu sou o cliente\n");
#endif

    return 0;
}
