#include "tetris.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

static const int shapes[7][4][4] = {
    {{1, 1, 1, 1}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}, // I
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // T
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // O
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // L
    {{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // J
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // S
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}  // Z
};

void initialize_tetris_game(TetrisGameState *game_state) {
    memset(game_state->board, 0, sizeof(game_state->board));
    spawn_tetromino(game_state);
}

void draw_tetris_board(TetrisGameState *game_state) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game_state->board[y][x]) {
                mvaddch(y * BLOCK_SIZE, x * BLOCK_SIZE, '#');
            }
        }
    }
}

void draw_tetromino(Tetromino *tetromino, int offset_x, int offset_y) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetromino->shape[y][x]) {
                mvaddch((offset_y + y) * BLOCK_SIZE, (offset_x + x) * BLOCK_SIZE, '#');
            }
        }
    }
}

void clear_tetris_lines(TetrisGameState *game_state) {
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        int full_line = 1;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!game_state->board[y][x]) {
                full_line = 0;
                break;
            }
        }
        if (full_line) {
            for (int k = y; k > 0; k--) {
                memcpy(game_state->board[k], game_state->board[k - 1], BOARD_WIDTH * sizeof(int));
            }
            memset(game_state->board[0], 0, BOARD_WIDTH * sizeof(int));
            y++;
        }
    }
}

void spawn_tetromino(TetrisGameState *game_state) {
    int shape_index = rand() % 7;
    memcpy(game_state->current.shape, shapes[shape_index], sizeof(game_state->current.shape));
    game_state->current.x = BOARD_WIDTH / 2 - 2;
    game_state->current.y = 0;
    game_state->current.color = 1;
}

void rotate_tetromino(TetrisGameState *game_state) {
    Tetromino temp = game_state->current;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            game_state->current.shape[x][3 - y] = temp.shape[y][x];
        }
    }
    if (check_collision(game_state, &game_state->current, game_state->current.x, game_state->current.y)) {
        memcpy(&game_state->current, &temp, sizeof(Tetromino));
    }
}

void move_tetromino_left(TetrisGameState *game_state) {
    if (!check_collision(game_state, &game_state->current, game_state->current.x - 1, game_state->current.y)) {
        game_state->current.x--;
    }
}

void move_tetromino_right(TetrisGameState *game_state) {
    if (!check_collision(game_state, &game_state->current, game_state->current.x + 1, game_state->current.y)) {
        game_state->current.x++;
    }
}

void move_tetromino_down(TetrisGameState *game_state) {
    if (!check_collision(game_state, &game_state->current, game_state->current.x, game_state->current.y + 1)) {
        game_state->current.y++;
    } else {
        // Add tetromino to board
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (game_state->current.shape[y][x]) {
                    game_state->board[game_state->current.y + y][game_state->current.x + x] = 1;
                }
            }
        }
        clear_tetris_lines(game_state);
        spawn_tetromino(game_state);
    }
}

void drop_tetromino(TetrisGameState *game_state) {
    while (!check_collision(game_state, &game_state->current, game_state->current.x, game_state->current.y + 1)) {
        game_state->current.y++;
    }
    // Add tetromino to board
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (game_state->current.shape[y][x]) {
                game_state->board[game_state->current.y + y][game_state->current.x + x] = 1;
            }
        }
    }
    clear_tetris_lines(game_state);
    spawn_tetromino(game_state);
}

int check_collision(TetrisGameState *game_state, Tetromino *tetromino, int offset_x, int offset_y) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetromino->shape[y][x]) {
                int board_x = offset_x + x;
                int board_y = offset_y + y;
                if (board_x < 0 || board_x >= BOARD_WIDTH || board_y >= BOARD_HEIGHT || game_state->board[board_y][board_x]) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void play_tetris_game() {
    TetrisGameState game_state;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);

    srand(time(NULL));
    initialize_tetris_game(&game_state);

    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_LEFT:
                move_tetromino_left(&game_state);
                break;
            case KEY_RIGHT:
                move_tetromino_right(&game_state);
                break;
            case KEY_DOWN:
                move_tetromino_down(&game_state);
                break;
            case ' ':
                drop_tetromino(&game_state);
                break;
            case KEY_UP:
                rotate_tetromino(&game_state);
                break;
        }

        clear();
        draw_tetris_board(&game_state);
        draw_tetromino(&game_state.current, game_state.current.x, game_state.current.y);
        refresh();
    }

    clear();
    refresh();
    endwin();
}
