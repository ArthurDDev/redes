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
    if (i < 0 || i >= WIDTH || j < 0 || j >= WIDTH)
        return '#';

    else
        return g.board[i][j];
}

// Vermelho – regra da mão esquerda
void red_movement(int i, int j, game g)
{
    g.board[i][j] = '0';

    if (get_pos(i, j - 1, g) == '0')
        g.board[i][j - 1] = 'R';
    else if (get_pos(i - 1, j, g) == '0')
        g.board[i - 1][j] = 'R';
    else
        g.board[i][j] = 'R';
}

// Verde – alterna direita e esquerda 
void green_movement(int i, int j, game g)
{
    g.board[i][j] = '0';
    if (get_pos(i + 1, j, g) != '#')
        g.board[i + 1][j] = 'G';
    else if (get_pos(i - 1, j, g) != '#')
        g.board[i - 1][j] = 'G';
    else
        g.board[i][j] = 'G';
}

// Azul – regra da mão direita
void blue_movement(int i, int j, game g)
{
    g.board[i][j] = '0';
    if (get_pos(i + 1, j, g) != '#')
        g.board[i + 1][j] = 'B';
    else if (get_pos(i - 1, j, g) != '#')
        g.board[i - 1][j] = 'B';
    else
        g.board[i][j] = 'B';
}

// Amarelo – aleatório
void yellow_movement(int i, int j, game g)
{
    g.board[i][j] = '0';
    if (get_pos(i + 1, j, g) != '#')
        g.board[i + 1][j] = 'Y';
    else if (get_pos(i - 1, j, g) != '#')
        g.board[i - 1][j] = 'Y';
    else
        g.board[i][j] = 'Y';
}

void server_game_loop()
{
    game g = make_game("");
    send_board(g);

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

        for (int i = 0; i < WIDTH; i ++)
            for (int j = 0; j < WIDTH; j ++)
                switch(g.board[i][j]) {
                    case 'R':
                        red_movement(i, j, g);
                        break;

                    case 'G':
                        green_movement(i, j, g);
                        break;

                    case 'B':
                        blue_movement(i, j, g);
                        break;

                    case 'Y':
                        yellow_movement(i, j, g);
                        break;
                }

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
        for (size_t j = 0; j < width; j ++)
            printf("%c", board[i + j]);
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