#include "games/chess/chess.h"
#include <ncurses.h>

void initialize_chess_game(ChessGameState *game_state) {
    // Stub: inizializza la board a zero
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            game_state->board[i][j] = 0;
    game_state->turn = 0;
    game_state->selected_x = 0;
    game_state->selected_y = 0;
    game_state->game_over = 0;
}

void draw_chess_game(const ChessGameState *game_state) {
    // Stub: stampa la board vuota
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            mvprintw(i + 2, j * 2 + 2, ".");
        }
    }
    mvprintw(0, 0, "Chess (stub): premi q per uscire");
}

void play_chess_game() {
    ChessGameState game_state;
    initialize_chess_game(&game_state);
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    int ch;
    while ((ch = getch()) != 'q') {
        clear();
        draw_chess_game(&game_state);
        refresh();
    }
    endwin();
}
