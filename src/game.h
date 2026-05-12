#ifndef __GAME__
#define __GAME__

#include <stddef.h>

#define WIDTH 40
#define HEIGHT 40

struct point {
    int x, y;
};
typedef struct point point;

enum direction {
    UP, 
    DOWN, 
    LEFT, 
    RIGHT
};
typedef enum direction direction;

struct ghost{
    char color;
    point pos;
    direction dir;
};
typedef struct ghost ghost;


struct game {
    char board[WIDTH][HEIGHT];
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
void render_board(unsigned char *board, size_t size);
void move_player(unsigned char dir);

// GHOSTS
direction turn_left(direction dir);
direction turn_right(direction dir);
direction turn_back(direction dir);
point next_position(point p, direction dir);
int can_move(game *g, point p);
int move_ghost(game *g, ghost *gh, direction dir);

#endif