#ifndef TETRIS_H
#define TETRIS_H

#include <ncurses.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 2

typedef struct {
    int x, y;
    int shape[4][4];  // Tetramino
    int color;
} Tetromino;

typedef struct {
    int board[BOARD_HEIGHT][BOARD_WIDTH];
    Tetromino current;
    Tetromino next;
} TetrisGameState;

// Dichiarazioni delle funzioni del gioco Tetris
void initialize_tetris_game(TetrisGameState *game_state);
void draw_tetris_board(TetrisGameState *game_state);
void draw_tetromino(Tetromino *tetromino, int offset_x, int offset_y);
void clear_tetris_lines(TetrisGameState *game_state);
void spawn_tetromino(TetrisGameState *game_state);
void rotate_tetromino(TetrisGameState *game_state);
void move_tetromino_left(TetrisGameState *game_state);
void move_tetromino_right(TetrisGameState *game_state);
void move_tetromino_down(TetrisGameState *game_state);
void drop_tetromino(TetrisGameState *game_state);
int check_collision(TetrisGameState *game_state, Tetromino *tetromino, int offset_x, int offset_y);
void play_tetris_game();

#endif // TETRIS_H
