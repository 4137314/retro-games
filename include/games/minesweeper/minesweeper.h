#ifndef MINESWEEPER_H
#define MINESWEEPER_H

typedef struct {
    int width;
    int height;
    int **grid;
    int **revealed;
    int **flagged;
    int mines;
} MinesweeperGameState;

void initialize_minesweeper_game(MinesweeperGameState *game_state, int width, int height, int mines);
void draw_minesweeper_game(const MinesweeperGameState *game_state, int cursor_row, int cursor_col);
void play_minesweeper_game();

#endif // MINESWEEPER_H
