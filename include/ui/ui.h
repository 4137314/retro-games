#ifndef UI_H
#define UI_H

#include <ncurses.h>

// Funzioni per l'interfaccia utente
void setup_ui_ncurses();          // Imposta ncurses
void handle_ui_menu(); // Gestisce la logica del menu
void cleanup_ui_ncurses();       // Pulizia ncurses
void draw_ui_border(WINDOW *win); // Disegna il bordo della finestra
void draw_ui_menu(WINDOW *win, int highlight); // Disegna il menu
void draw_ui_matrix_title(); // Titolo Matrix
void draw_ui_loading_animation(); // Animazione di caricamento
extern int colorblind_mode;

// Dichiarazioni delle funzioni di gioco
void play_breakout_game();
void play_minesweeper_game();
void play_pacman_game();
void play_pong_game();
void play_snake_game();
void play_sudoku_game();
void play_tetris_game();

#endif // UI_H
