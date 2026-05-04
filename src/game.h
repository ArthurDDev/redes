#ifndef __GAME__
#define __GAME__

#include <stddef.h>

struct point {
    int x, y;
};

typedef struct point point;

struct game {
    char board[64][64];
    unsigned char light_level;
    point player_pos;
};

typedef struct game game;


// SERVER

void server_game_loop();

game make_game(const char *map);

void server_process(game g);

void send_board(game g);

point get_element(char el);

// CLIENTE
void client_game_loop();

void render_board(char *board, size_t size);

void move_player(unsigned char dir);


#endif