#include "minesweeper.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

void initialize_minesweeper_game(MinesweeperGameState *game_state, int width, int height, int mines) {
    game_state->width = width;
    game_state->height = height;
    game_state->mines = mines;

    game_state->grid = (int **)malloc(height * sizeof(int *));
    game_state->revealed = (int **)malloc(height * sizeof(int *));
    game_state->flagged = (int **)malloc(height * sizeof(int *));
    for (int i = 0; i < height; i++) {
        game_state->grid[i] = (int *)malloc(width * sizeof(int));
        game_state->revealed[i] = (int *)malloc(width * sizeof(int));
        game_state->flagged[i] = (int *)malloc(width * sizeof(int));
    }

    // Initialize grid with zeros
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            game_state->grid[i][j] = 0;
            game_state->revealed[i][j] = 0;
            game_state->flagged[i][j] = 0;
        }
    }

    // Place mines
    srand(time(NULL));
    for (int i = 0; i < mines; i++) {
        int row, col;
        do {
            row = rand() % height;
            col = rand() % width;
        } while (game_state->grid[row][col] == -1);
        game_state->grid[row][col] = -1;

        // Update numbers around the mine
        for (int r = row - 1; r <= row + 1; r++) {
            for (int c = col - 1; c <= col + 1; c++) {
                if (r >= 0 && r < height && c >= 0 && c < width && game_state->grid[r][c] != -1) {
                    game_state->grid[r][c]++;
                }
            }
        }
    }
}

void draw_minesweeper_game(const MinesweeperGameState *game_state, int cursor_row, int cursor_col) {
    for (int i = 0; i < game_state->height; i++) {
        for (int j = 0; j < game_state->width; j++) {
            if (i == cursor_row && j == cursor_col) {
                attron(A_REVERSE);
            }
            if (game_state->flagged[i][j]) {
                mvprintw(i + 1, j * 2 + 1, "F");
            } else if (!game_state->revealed[i][j]) {
                mvprintw(i + 1, j * 2 + 1, "#");
            } else if (game_state->grid[i][j] == -1) {
                mvprintw(i + 1, j * 2 + 1, "*");
            } else {
                mvprintw(i + 1, j * 2 + 1, "%d", game_state->grid[i][j]);
            }
            attroff(A_REVERSE);
        }
    }
}

void reveal_cell(MinesweeperGameState *game_state, int row, int col) {
    if (row < 0 || row >= game_state->height || col < 0 || col >= game_state->width || game_state->revealed[row][col]) {
        return;
    }
    game_state->revealed[row][col] = 1;
    if (game_state->grid[row][col] == 0) {
        // Reveal surrounding cells if the current cell is empty
        for (int r = row - 1; r <= row + 1; r++) {
            for (int c = col - 1; c <= col + 1; c++) {
                if (r != row || c != col) {
                    reveal_cell(game_state, r, c);
                }
            }
        }
    }
}

void play_minesweeper_game() {
    MinesweeperGameState game_state;
    int ch;
    int row = 0, col = 0;
    int width = 10, height = 10, mines = 10;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    initialize_minesweeper_game(&game_state, width, height, mines);

    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                row = (row == 0) ? height - 1 : row - 1;
                break;
            case KEY_DOWN:
                row = (row == height - 1) ? 0 : row + 1;
                break;
            case KEY_LEFT:
                col = (col == 0) ? width - 1 : col - 1;
                break;
            case KEY_RIGHT:
                col = (col == width - 1) ? 0 : col + 1;
                break;
            case ' ':
                if (!game_state.flagged[row][col]) {
                    reveal_cell(&game_state, row, col);
                }
                break;
            case 'f':
                game_state.flagged[row][col] = !game_state.flagged[row][col];
                break;
            default:
                break;
        }

        clear();
        draw_minesweeper_game(&game_state, row, col);
        mvprintw(0, 0, "Use arrow keys to move, ' ' to reveal, 'f' to flag, 'q' to quit.");
        move(row + 1, col * 2 + 1);
        refresh();
    }

    endwin();

    for (int i = 0; i < height; i++) {
        free(game_state.grid[i]);
        free(game_state.revealed[i]);
        free(game_state.flagged[i]);
    }
    free(game_state.grid);
    free(game_state.revealed);
    free(game_state.flagged);
}
