#include "game.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "net.h"

int is_movement_type(char type) {
    if (type == M_UP
        || type == M_DOWN
        || type == M_RIGHT
        || type == M_LEFT)
        return 1;

    return 0;
}

char get_pos(int i, int j, game g) {
    if (i < 0 || i >= WIDTH || j < 0 || j >= HEIGHT)
        return '#';

    else
        return g.board[i][j];
}

direction turn_left(direction dir)
{
    switch (dir) {
        case UP:    return LEFT;
        case DOWN:  return RIGHT;
        case LEFT:  return DOWN;
        case RIGHT: return UP;
    }

    return UP;
}

direction turn_right(direction dir)
{
    switch (dir) {
        case UP:    return RIGHT;
        case DOWN:  return LEFT;
        case LEFT:  return UP;
        case RIGHT: return DOWN;
    }

    return UP;
}

direction turn_back(direction dir)
{
    switch (dir) {
        case UP:    return DOWN;
        case DOWN:  return UP;
        case LEFT:  return RIGHT;
        case RIGHT: return LEFT;
    }

    return UP;
}

point next_position(point p, direction dir)
{
    switch (dir) {
        case UP:
            p.x--;
            break;

        case DOWN:
            p.x++;
            break;

        case LEFT:
            p.y--;
            break;

        case RIGHT:
            p.y++;
            break;
    }

    return p;
}

int can_move(game *g, point p)
{
    if (p.x < 0 || p.x >= WIDTH)
        return 0;

    if (p.y < 0 || p.y >= HEIGHT)
        return 0;

    if (g->board[p.x][p.y] != '0')
        return 0;

    return 1;
}

int move_ghost(game *g, ghost *ghost, direction dir)
{
    point next = next_position(ghost->pos, dir);

    if (!can_move(g, next))
        return 0;

    g->board[ghost->pos.x][ghost->pos.y] = '0';

    ghost->pos = next;

    ghost->dir = dir;

    g->board[ghost->pos.x][ghost->pos.y] = ghost->color;

    return 1;
}

// Vermelho – regra da mão esquerda
void red_movement(game *g, ghost *ghost)
{
    if (move_ghost(g, ghost, turn_left(ghost->dir)))
        return;

    if (move_ghost(g, ghost, ghost->dir))
        return;

    if (move_ghost(g, ghost, turn_right(ghost->dir)))
        return;

    move_ghost(g, ghost, turn_back(ghost->dir));
}

// Azul – regra da mão direita
void blue_movement(game *g, ghost *ghost)
{
    if (move_ghost(g, ghost, turn_right(ghost->dir)))
        return;

    if (move_ghost(g, ghost, ghost->dir))
        return;

    if (move_ghost(g, ghost, turn_left(ghost->dir)))
        return;

    move_ghost(g, ghost, turn_back(ghost->dir));
}

// Verde – alterna direita e esquerda 
void green_movement(game *g, ghost *ghost)
{
    static int toggle = 0;

    if (toggle == 0)
        red_movement(g, ghost);
    else
        blue_movement(g, ghost);

    toggle = !toggle;
}

// Amarelo – aleatório
void yellow_movement(game *g, ghost *ghost)
{
    direction dir;

    // tenta 4 vezes, se não conseguir mover, fica parado
    for (int i = 0; i < 4; i++) {
        dir = rand() % 4;
        if (move_ghost(g, ghost, dir))
            return;
    }
}

void server_game_loop()
{
    game g = make_game("");
    send_board(g);

    ghost red = {'R', {3, 4}, RIGHT};
    ghost green = {'G', {4, 4}, RIGHT};
    ghost blue = {'B', {5, 4}, RIGHT};
    ghost yellow = {'Y', {6, 4}, RIGHT};

    message m;
    int movement_count = 1;
    while (1) {
        // Fog of war
        if (movement_count % 5 == 0 && g.light_level < WIDTH)
            g.light_level ++;
        movement_count ++;

        printf("%d | ", movement_count);

        // Receber input
        do {
            m = receive_data();
        } while (!is_movement_type(m.type));

        red_movement(&g, &red);
        green_movement(&g, &green);
        blue_movement(&g, &blue);
        yellow_movement(&g, &yellow);
            
        point next_pos = g.player_pos;
        switch(m.type) {
            case M_UP:
                printf("MOVENTO PARA CIMA\n");
                next_pos.y --;
                break;
            case M_DOWN:
                printf("MOVENTO PARA BAIXO\n");
                next_pos.y ++;
                break;
            case M_RIGHT:
                printf("MOVENTO PARA DIREITA\n");
                next_pos.x ++;
                break;
            case M_LEFT:
                printf("MOVENTO PARA ESQUERDA\n");
                next_pos.x --;
                break;      
        }

        char c = get_pos(next_pos.x, next_pos.y, g);
        if (c == '#') {
            next_pos = g.player_pos;
            c = get_pos(next_pos.x, next_pos.y, g);
        }

        switch(c) {
            case 'R':
                printf("JOGADOR PERDEU\n");
                break;
            case 'G':
                printf("JOGADOR PERDEU\n");
                break;
            case 'B':
                printf("JOGADOR PERDEU\n");
                break;
            case 'Y':
                printf("JOGADOR PERDEU\n");
                break;
            
            case '1':
                printf("JOGADOR COLETOU 1\n");
                break;
            case '2':
                printf("JOGADOR COLETOU 2\n");
                break;
            case '3':
                printf("JOGADOR COLETOU 3\n");
                break;
            case '4':
                printf("JOGADOR COLETOU 4\n");
                break;
            case '5':
                printf("JOGADOR COLETOU 5\n");
                break;
            case '6':
                printf("JOGADOR COLETOU 6\n");
                break;

        }
        
        if (get_pos(g.player_pos.x, g.player_pos.y, g) == 'P')
            g.board[g.player_pos.x][g.player_pos.y] = '0';
        g.player_pos = next_pos;
        g.board[g.player_pos.x][g.player_pos.y] = 'P';

        send_board(g);
    }

}

void send_board(game g)
{
    //printf("S1\n");

    int width = g.light_level * 2 + 1;
    int area = width * width;
    unsigned char *buffer = malloc(area);


    for (int i = 0; i < width; i ++)
        for (int j = 0; j < width; j ++) {
            buffer[j * width + i] = get_pos(i + g.player_pos.x - g.light_level, j + g.player_pos.y - g.light_level, g);
        }

    message m = {area, M_VIS, buffer};
    send_data(m);
    delete_message(&m);
}

void client_game_loop()
{
    while (1) {
        message m;
        do {
            m = receive_data();
        } while (m.type != M_VIS);

        render_board(m.data, m.size);

        char move;
        int valid;
            printf("Digite o movimento:  \n");
        do {
            scanf("%c", &move);

            valid = 0;
            switch(move) {
                case 'w':
                    send_message((message){0, M_UP, NULL});
                    valid = 1;
                    break;
                
                case 's':
                    send_message((message){0, M_DOWN, NULL});
                    valid = 1;
                    break;
            
                case 'a':
                    send_message((message){0, M_LEFT, NULL});
                    valid = 1;
                    break;
        
                case 'd':
                    send_message((message){0, M_RIGHT, NULL});
                    valid = 1;
                    break;
            }
        } while (valid == 0);
    }
}

void render_board(unsigned char *board, size_t size)
{
    if (!board) {
        fprintf(stderr, "Tabuleiro inválido\n");
        return;
    }

    size_t width = sqrt(size);

    if (size % width != 0) {
        fprintf(stderr, "O tabuleiro não é quadrado\n");
        return;
    }

    for (size_t i = 0; i < size; i += width) {
        for (size_t j = 0; j < width; j ++) {
		if (board[i+j] == '0')
			printf(" ");
		else
            printf("%c", board[i + j]);
	}
        printf("\n");
    }
}

game make_game(const char *map)
{
    game g;
    const char *ufpr_board[] = {
        "0000#00000000000000000000000000000000000",
        "0000#00000000000000000000000000000000000",
        "00P0#00000000000000000000000000000000000",
        "0#00R0000#000000000000000000000000000000",
        "0#00G0000#000000000000000000000000000000",
        "0#00B0000#000000000000000000000000000000",
        "0#00Y0000#000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0001234560000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
        "0000000000000000000000000000000000000000",
    };

    if (strcmp(map, "") == 0)
        for (int i = 0; i < WIDTH; i ++)
            for (int j = 0; j < WIDTH; j ++)
                g.board[j][i] = ufpr_board[i][j];

    g.light_level = 1;
    g.player_pos = (point){2, 2};

    return g;
}
