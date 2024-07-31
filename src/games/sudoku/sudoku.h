#ifndef SUDOKU_H
#define SUDOKU_H

#define GRID_SIZE 9

typedef struct {
    int grid[GRID_SIZE][GRID_SIZE];
    int solution[GRID_SIZE][GRID_SIZE];
    int score;
    int best_score;
} SudokuGameState;

void initialize_sudoku_game(SudokuGameState *game_state);
void draw_sudoku_game(const SudokuGameState *game_state, int cursor_row, int cursor_col);
void play_sudoku_game();

#endif // SUDOKU_H
