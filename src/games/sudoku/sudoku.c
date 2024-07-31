#include "sudoku.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

// Funzioni di utilità per generare e risolvere la griglia di Sudoku
int sudoku_is_valid(int grid[GRID_SIZE][GRID_SIZE], int row, int col, int num);
int solve_sudoku(int grid[GRID_SIZE][GRID_SIZE]);
void generate_sudoku(SudokuGameState *game_state);

void initialize_sudoku_game(SudokuGameState *game_state) {
    generate_sudoku(game_state);
}
// Funzione per disegnare la griglia di Sudoku con bordi e separatori
void draw_grid(int start_row, int start_col) {
    attron(COLOR_PAIR(1));
    for (int i = 0; i <= GRID_SIZE; i++) {
        mvhline(start_row + i * 2, start_col, '-', GRID_SIZE * 2 + 1);
        mvvline(start_row, start_col + i * 2, '|', GRID_SIZE * 2 + 1);
    }
    mvaddch(start_row, start_col + GRID_SIZE * 2, '+');
    mvaddch(start_row + GRID_SIZE * 2, start_col + GRID_SIZE * 2, '+');
    attroff(COLOR_PAIR(1));
}

void draw_sudoku_game(const SudokuGameState *game_state, int cursor_row, int cursor_col) {
    int start_row = 2;
    int start_col = 2;

    draw_grid(start_row, start_col);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (i == cursor_row && j == cursor_col) {
                attron(A_REVERSE);  // Evidenzia la casella selezionata
            }
            if (game_state->grid[i][j] != 0) {
                mvprintw(start_row + i * 2, start_col + j * 2 + 1, "%d", game_state->grid[i][j]);
            } else {
                mvprintw(start_row + i * 2, start_col + j * 2 + 1, ".");
            }
            attroff(A_REVERSE);
        }
    }
}

void draw_status(const SudokuGameState *game_state, int time_elapsed) {
    mvprintw(0, 0, "Score: %d", game_state->score);
    mvprintw(1, 0, "Best Score: %d", game_state->best_score);
    mvprintw(2, 0, "Time Elapsed: %02d:%02d", time_elapsed / 60, time_elapsed % 60);
}

void play_sudoku_game() {
    SudokuGameState game_state;
    int ch;
    int row = 0, col = 0;
    int start_time, current_time, time_elapsed;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);

    // Inizializza colori
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK); // Bordo della griglia
    init_pair(2, COLOR_WHITE, COLOR_BLACK); // Testo
    init_pair(3, COLOR_RED, COLOR_BLACK);   // Evidenziazione

    initialize_sudoku_game(&game_state);
    start_time = time(NULL);

    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                row = (row == 0) ? GRID_SIZE - 1 : row - 1;
                break;
            case KEY_DOWN:
                row = (row == GRID_SIZE - 1) ? 0 : row + 1;
                break;
            case KEY_LEFT:
                col = (col == 0) ? GRID_SIZE - 1 : col - 1;
                break;
            case KEY_RIGHT:
                col = (col == GRID_SIZE - 1) ? 0 : col + 1;
                break;
            default:
                if (ch >= '1' && ch <= '9') {
                    int num = ch - '0';
                    if (sudoku_is_valid(game_state.grid, row, col, num)) {
                        game_state.grid[row][col] = num;
                    }
                }
                break;
        }

        current_time = time(NULL);
        time_elapsed = (int)(current_time - start_time);

        clear();
        draw_sudoku_game(&game_state, row, col);
        draw_status(&game_state, time_elapsed);
        move(row * 2 + 2, col * 2 + 1);
        refresh();
    }

    endwin();
}

// Funzioni di utilità per generare e risolvere la griglia di Sudoku
int sudoku_is_valid(int grid[GRID_SIZE][GRID_SIZE], int row, int col, int num) {
    for (int x = 0; x < GRID_SIZE; x++) {
        if (grid[row][x] == num || grid[x][col] == num ||
            grid[row - row % 3 + x / 3][col - col % 3 + x % 3] == num) {
            return 0;
        }
    }
    return 1;
}

int solve_sudoku(int grid[GRID_SIZE][GRID_SIZE]) {
    int row, col;
    int empty = 0;

    for (row = 0; row < GRID_SIZE; row++) {
        for (col = 0; col < GRID_SIZE; col++) {
            if (grid[row][col] == 0) {
                empty = 1;
                break;
            }
        }
        if (empty) {
            break;
        }
    }

    if (!empty) {
        return 1;
    }

    for (int num = 1; num <= 9; num++) {
        if (sudoku_is_valid(grid, row, col, num)) {
            grid[row][col] = num;
            if (solve_sudoku(grid)) {
                return 1;
            }
            grid[row][col] = 0;
        }
    }
    return 0;
}

void generate_sudoku(SudokuGameState *game_state) {
    int grid[GRID_SIZE][GRID_SIZE] = {0};

    srand(time(NULL));

    // Riempire la diagonale di 3x3 box
    for (int i = 0; i < GRID_SIZE; i += 3) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                int num;
                do {
                    num = rand() % GRID_SIZE + 1;
                } while (!sudoku_is_valid(grid, i + j, i + k, num));
                grid[i + j][i + k] = num;
            }
        }
    }

    // Risolvere la griglia
    solve_sudoku(grid);

    // Rimuovere alcuni numeri per creare il puzzle
    for (int i = 0; i < 40; i++) {
        int row = rand() % GRID_SIZE;
        int col = rand() % GRID_SIZE;
        grid[row][col] = 0;
    }

    // Copiare la griglia nel game_state
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            game_state->grid[i][j] = grid[i][j];
            game_state->solution[i][j] = grid[i][j];
        }
    }
}