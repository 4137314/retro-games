#include "ui.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Include i file header dei giochi
#include "include/games/games.h"

// Funzione per pulire up ncurses
void cleanup_ui_ncurses() {
  endwin(); // Termina la modalità ncurses
}

// Funzione per disegnare il bordo della finestra
void draw_ui_border(WINDOW *win) {
  wbkgd(win, COLOR_PAIR(1)); // Imposta il colore di sfondo
  box(win, 0, 0);            // Disegna il bordo
  wrefresh(win);             // Aggiorna la finestra
}

// Funzione per disegnare il menu
void draw_ui_menu(WINDOW *win, int highlight) {
  char *choices[] = {"Breakout", "Minesweeper", "Pac-Man", "Pong",
                     "Snake",    "Sudoku",      "Tetris"};
  int n_choices = sizeof(choices) / sizeof(char *);
  int x = 2, y = 2;

  for (int i = 0; i < n_choices; ++i) {
    if (highlight == i) {
      wattron(win, A_REVERSE);
      mvwprintw(win, y, x, "%s", choices[i]);
      wattroff(win, A_REVERSE);
    } else {
      wattron(win,
              COLOR_PAIR(2)); // Utilizza la coppia di colori 2 (testo verde)
      mvwprintw(win, y, x, "%s", choices[i]);
      wattroff(win, COLOR_PAIR(2));
    }
    y++;
  }
  wrefresh(win);
}

// Funzione per visualizzare il titolo in stile Matrix
void draw_ui_matrix_title() {
  char *title[] = {
      " ____      _                ____",
      "|  _ \\ ___| |_ _ __ ___    / ___| __ _ _ __ ___   ___  ___",
      "| |_) / _ \\ __| '__/ _ \\  | |  _ / _` | '_ ` _ \\ / _ \\/ __|",
      "|  _ <  __/ |_| | | (_) | | |_| | (_| | | | | | |  __/\\__ \\",
      "|_| \\_\\___|\\__|_|  \\___/   \\____|\\__,_|_| |_| |_|\\___||___/",
      "",
  };

  int row;
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x); // Usa variabili modificabili
  int start_x = (max_x - 55) / 2; // Centra il titolo
  int start_y = 1;                // Spazio dal bordo superiore

  for (row = 0; row < 6; ++row) {
    mvprintw(start_y + row, start_x, "%s", title[row]);
  }
  refresh();
}

// Funzione per configurare ncurses
void setup_ui_ncurses() {
  initscr();            // Inizializza lo schermo
  cbreak();             // Disabilita il buffering del tasto
  noecho();             // Non mostra i caratteri digitati
  keypad(stdscr, TRUE); // Abilita le funzioni speciali della tastiera
  curs_set(0);          // Nasconde il cursore
  start_color();        // Abilita i colori
  init_pair(1, COLOR_BLUE, COLOR_BLACK);  // Definisce il colore del bordo
  init_pair(2, COLOR_GREEN, COLOR_BLACK); // Definisce il colore del testo
}

// Funzione per gestire la logica del menu
void handle_ui_menu() {
  int start_y, start_x, height, width;
  getmaxyx(stdscr, height, width); // Ottieni le dimensioni del terminale
  start_y = 8;                     // Spazio per il titolo
  start_x = 5;

  WINDOW *menu_win = newwin(height - 8, width - 10, start_y,
                            start_x); // Riduci altezza e larghezza per il menu
  keypad(menu_win, TRUE);

  int n_choices = 7; // Numero di scelte nel menu
  int choice;
  int highlight = 0;

  while (1) {
    clear();                  // Pulisce lo schermo
    draw_ui_matrix_title();   // Disegna il titolo in stile Matrix
    draw_ui_border(menu_win); // Disegna il bordo della finestra del menu
    draw_ui_menu(menu_win, highlight); // Disegna il menu
    choice = wgetch(menu_win);

    switch (choice) {
    case KEY_UP:
      highlight = (highlight == 0) ? n_choices - 1 : highlight - 1;
      break;
    case KEY_DOWN:
      highlight = (highlight == n_choices - 1) ? 0 : highlight + 1;
      break;
    case 10:                       // Enter key
      if (highlight < n_choices) { // Giochi
        delwin(menu_win);
        endwin();
        // Call to the corresponding game function
        switch (highlight) {
        case 0:
          play_breakout_game();
          break;
        case 1:
          play_minesweeper_game();
          break;
        case 2:
          play_pacman_game();
          break;
        case 3:
          play_pong_game();
          break;
        case 4:
          play_snake_game();
          break;
        case 5:
          play_sudoku_game();
          break;
        case 6:
          play_tetris_game();
          break;
        }
        setup_ui_ncurses();
        menu_win = newwin(height - 8, width - 10, start_y, start_x);
        keypad(menu_win, TRUE);
        draw_ui_border(menu_win);
        draw_ui_menu(menu_win, highlight);
      }
      break;
    case 'q': // Tasto 'q' per uscire
      delwin(menu_win);
      cleanup_ui_ncurses();
      exit(0);
      break;
    default:
      break;
    }
  }
}
