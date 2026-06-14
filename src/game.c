#include "game.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "net.h"
#include "files.h"

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

    if (next.x == g->player_pos.x && next.y == g->player_pos.y){
        lose_server();
    }

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
    if (move_ghost(g, ghost, ghost->dir))
        return;

    if (move_ghost(g, ghost, turn_left(ghost->dir)))
        return;

    if (move_ghost(g, ghost, turn_right(ghost->dir)))
        return;

    move_ghost(g, ghost, turn_back(ghost->dir));
}

// Azul – regra da mão direita
void blue_movement(game *g, ghost *ghost)
{

    if (move_ghost(g, ghost, ghost->dir))
        return;

    if (move_ghost(g, ghost, turn_right(ghost->dir)))
        return;

    if (move_ghost(g, ghost, turn_left(ghost->dir)))
        return;

    move_ghost(g, ghost, turn_back(ghost->dir));
}

// Verde – alterna direita e esquerda
void green_movement(game *g, ghost *ghost)
{
    static int toggle = 0;

    if (move_ghost(g, ghost, ghost->dir))
        return;

    if (toggle == 0) {
        if (move_ghost(g, ghost, turn_left(ghost->dir))) {
            toggle = 1;
            return;
        }

        if (move_ghost(g, ghost, turn_right(ghost->dir))) {
            toggle = 1;
            return;
        }
    }
    else {
        if (move_ghost(g, ghost, turn_right(ghost->dir))) {
            toggle = 0;
            return;
        }

        if (move_ghost(g, ghost, turn_left(ghost->dir))) {
            toggle = 0;
            return;
        }
    }

    move_ghost(g, ghost, turn_back(ghost->dir));
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

void lose_server()
{
	size_t siz;
	unsigned char *data = file_to_message("perdeu.txt", &siz);
	send_data((message){siz, M_TXT, data});
	free(data);

	send_data((message){0, M_LOSE, NULL});
	exit(0);
}

void server_game_loop(const char *map)
{
    game g = make_game(map);
    send_board(g, 1);

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

        red_movement(&g, &g.red);
        green_movement(&g, &g.green);
        blue_movement(&g, &g.blue);
        yellow_movement(&g, &g.yellow);

        point next_pos = g.player_pos;
        switch(m.type) {
            case M_UP:
                printf("MOVIMENTO PARA CIMA\n");
                next_pos.y --;
                break;
            case M_DOWN:
                printf("MOVIMENTO PARA BAIXO\n");
                next_pos.y ++;
                break;
            case M_RIGHT:
                printf("MOVIMENTO PARA DIREITA\n");
                next_pos.x ++;
                break;
            case M_LEFT:
                printf("MOVIMENTO PARA ESQUERDA\n");
                next_pos.x --;
                break;
        }

        char c = get_pos(next_pos.x, next_pos.y, g);
        if (c == '#') {
            next_pos = g.player_pos;
            c = get_pos(next_pos.x, next_pos.y, g);
        }

	size_t siz;
    unsigned char *data;

	switch(c) {
            case 'R':
                lose_server();
                break;
            case 'G':
                lose_server();
                break;
            case 'B':
                lose_server();
                break;
            case 'Y':
                lose_server();
                break;

            case '1':
                data = file_to_message("1.txt", &siz);
		send_data((message){siz, M_TXT, data});
                break;
            case '2':
                data = file_to_message("2.txt", &siz);
		send_data((message){siz, M_TXT, data});
                break;
            case '3':
                data = file_to_message("3.jpg", &siz);
		send_data((message){siz, M_JPG, data});
                break;
            case '4':
                data = file_to_message("4.jpg", &siz);
		send_data((message){siz, M_JPG, data});
                break;
            case '5':
                data = file_to_message("5.mp4", &siz);
		send_data((message){siz, M_MP4, data});
                break;
            case '6':
                data = file_to_message("6.mp4", &siz);
		send_data((message){siz, M_MP4, data});
                break;

        }

        if (get_pos(g.player_pos.x, g.player_pos.y, g) == 'P')
            g.board[g.player_pos.x][g.player_pos.y] = '0';
        g.player_pos = next_pos;
        g.board[g.player_pos.x][g.player_pos.y] = 'P';

        send_board(g, 0);
    }

}

void send_board(game g, char first)
{
    int width = g.light_level * 2 + 1;
    int area = width * width;
    unsigned char *buffer = malloc(area);


    for (int i = 0; i < width; i ++)
        for (int j = 0; j < width; j ++) {
            buffer[j * width + i] = get_pos(i + g.player_pos.x - g.light_level, j + g.player_pos.y - g.light_level, g);
        }

    message m = {area, M_VIS, buffer};
    if (first)
	    m.type = M_INIT;
    send_data(m);
    delete_message(&m);
}

void client_game_loop()
{
    while (1) {
        message m;
        do {
            m = receive_data();
	    if (m.type == M_MP4 || m.type == M_TXT || m.type == M_JPG) {
		    message_to_file(m);
	    }
	    if (m.type == M_LOSE)
		exit(0);
        } while (m.type != M_VIS && m.type != M_INIT);

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
          else if (board[i+j] == '#')
	        printf("#");
		  else
            printf("%c", board[i + j]);
	}
        printf("\n");
    }
}

game make_game(const char *map)
{
    game g;
    g.light_level = 1;

    const char *ufpr_board[] = {
        "########################################",
        "#0000000000000000000000000000000000000#",
        "#00#########00#######################0#",
        "#0000000000000000000000000000000000000#",
        "#00######00#############00#####000##00#",
        "#0000000000000000000000000000000000000#",
        "#00#########00#######################0#",
        "#0000000000000000000000000000000000000#",
        "#0000000000000000000000000000000000000#",
        "#00#########00#######################0#",
        "#0000000000000000000000000000000000000#",
        "#0000#0#0000#####000#####000####000000#",
        "#0000#0#0000#0000000#000#000#000#00000#",
        "#0000#0#0000#####000#000#000#000#00000#",
        "#0000#0#0000#0000000#000#000#000#00000#",
        "#0000#0#0000#0000000#####000####000000#",
        "#0000#0#0000#0000000#0000000#000#00000#",
        "#0000#0#0000#0000000#0000000#000#00000#",
        "#0000#0#0000#0000000#0000000#000#00000#",
        "#0000###0000#0000000#0000000#000#00000#",
        "#0000000000000000000000000000000000000#",
        "#00######00#############00#####000##00#",
        "#0000000000000000000000000000000000000#",
        "#00#########00#######################0#",
        "#0000000000000000000000000000000000000#",
        "#00######00#############00#####000##00#",
        "#0000000000000000000000000000000000000#",
        "#00#########00#######################0#",
        "#0000000000000000000000000000000000000#",
        "#00#########00#######################0#",
        "#0000000000000000000000000000000000000#",
        "#00######00#############00#####000##00#",
        "#0000000000000000000000000000000000000#",
        "#00#########00#######################0#",
        "#0000000000000000000000000000000000000#",
        "#00######00#############00#####000##00#",
        "#0000000000000000000000000000000000000#",
        "#00#########00#######################0#",
        "#0000000000000000000000000000000000000#",
        "########################################"
    };

    if (!map || strcmp(map, "") == 0) {
        printf("Usando mapa padrão\n");
        for (int i = 0; i < WIDTH; i ++)
            for (int j = 0; j < WIDTH; j ++)
                g.board[j][i] = ufpr_board[i][j];

        for (int i = 1; i <= 6; i++){
            point p = valid_point(&g);
            int x = p.x;
            int y = p.y;
            g.board[x][y] = i + '0';
        }

        g.player_pos = valid_point(&g);
        g.red = (ghost){'R', valid_point(&g), RIGHT};
        g.green = (ghost){'G', valid_point(&g), DOWN};
        g.blue = (ghost){'B', valid_point(&g), RIGHT};
        g.yellow = (ghost){'Y', valid_point(&g), LEFT};
        g.board[g.player_pos.x][g.player_pos.y] = 'P';
        g.board[g.red.pos.x][g.red.pos.y] = 'R';
        g.board[g.green.pos.x][g.green.pos.y] = 'G';
        g.board[g.blue.pos.x][g.blue.pos.y] = 'B';
        g.board[g.yellow.pos.x][g.yellow.pos.y] = 'Y';
    }
    else {
        FILE *csv = fopen(map, "r");

        if (!csv) {
            perror("Erro abrindo mapa");

            // volta pro UFPR
            for (int i = 0; i < HEIGHT; i++)
                for (int j = 0; j < WIDTH; j++)
                    g.board[j][i] = ufpr_board[i][j];
        }
        else {
            // inicializa o tabuleiro vazio
            memset(g.board,'0',sizeof(g.board));

            char line[256];

            for (int i = 0; i < HEIGHT; i++) {

                if (!fgets(line, sizeof(line), csv))
                    break;

                int col = 0;

                // pega cada elemento separado por vírgula
                char *tok = strtok(line,",\n");

                while(tok && col < WIDTH){

                    char c = tok[0];

                    g.board[col][i] = c;

                    switch(c){
                        case 'P':
                            g.player_pos = (point){col,i};
                            break;
                        case 'R':
                            g.red = (ghost){'R', {col, i}, RIGHT};
                            break;
                        case 'G':
                            g.green = (ghost){'G', {col, i}, RIGHT};
                            break;
                        case 'B':
                            g.blue = (ghost){'B', {col, i}, RIGHT};
                            break;
                        case 'Y':
                            g.yellow = (ghost){'Y', {col, i}, RIGHT};
                            break;
                    }

                    tok = strtok(NULL,",\n");
                    col++;
                }
            }

            fclose(csv);
        }
    }

    return g;
}

long aleat (long min, long max)
{
  return ((rand() % (max - min + 1)) + min);
}

point valid_point(game *g)
{
    while (1) {
        point p = {aleat(0, WIDTH-1), aleat(0, HEIGHT-1)};

        if (g->board[p.x][p.y] == '0')
            return p;
    }
}
