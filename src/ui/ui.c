#include "ui/ui.h"
#include <stdbool.h>

// Modalità per daltonici (colorblind)
int colorblind_mode = 0;
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Include i file header dei giochi
#include "games/games.h"

// Funzione per pulire up ncurses
void cleanup_ui_ncurses() {
  endwin(); // Termina la modalità ncurses
}

// Funzione per disegnare il bordo della finestra
void draw_ui_border(WINDOW *win) {
  wbkgd(win, COLOR_PAIR(1)); // Sfondo blu
  wattron(win, COLOR_PAIR(3));
  box(win, 0, 0);            // Bordo giallo
  wattroff(win, COLOR_PAIR(3));
  wrefresh(win);
}

// Funzione per disegnare il menu
void draw_ui_menu(WINDOW *win, int highlight) {
  char *choices[] = {"Breakout", "Minesweeper", "Pac-Man", "Pong",
                     "Snake",    "Sudoku",      "Tetris", "Help", "Credits", "Exit"};
  int n_choices = sizeof(choices) / sizeof(char *);
  int x = 4, y = 2;
  for (int i = 0; i < n_choices; ++i) {
    if (highlight == i) {
      wattron(win, colorblind_mode ? COLOR_PAIR(6) : COLOR_PAIR(4));
      wattron(win, A_BOLD | A_REVERSE);
      mvwprintw(win, y, x, "> %s <", choices[i]);
      wattroff(win, A_BOLD | A_REVERSE);
      wattroff(win, colorblind_mode ? COLOR_PAIR(6) : COLOR_PAIR(4));
    } else {
      wattron(win, COLOR_PAIR(2));
      mvwprintw(win, y, x, "  %s", choices[i]);
      wattroff(win, COLOR_PAIR(2));
    }
    y++;
  }
  // Footer
  wattron(win, COLOR_PAIR(7));
  mvwprintw(win, getmaxy(win) - 2, 2, "Frecce: muovi  Invio: seleziona  Q: esci  C: colori");
  wattroff(win, COLOR_PAIR(7));
  wrefresh(win);
}

// Funzione per visualizzare il titolo in stile Matrix
void draw_ui_matrix_title() {
  char *title[] = {
      " ____      _                ____",
      "|  _ \\ ___| |_ _ __ ___    / ___| __ _ _ __ ___   ___  ___",
      "| |_) / _ \\ __| '__/ _ \\  | |  _ / _` | '_ ` _ \\ / _ \\ / __|",
    "|  _ <  __/ |_| | | (_) | | |_| | (_| | | | | | |  __/\\__ \\",
      "|_| \\_\\___|\\__|_|  \\___/   \\____|\\__,_|_| |_| |_|\\___||___/",
      "",
  };
  int row;
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);
  int start_x = (max_x - 65) / 2;
  int start_y = 1;
  attron(COLOR_PAIR(5));
  for (row = 0; row < 6; ++row) {
    mvprintw(start_y + row, start_x, "%s", title[row]);
  }
  attroff(COLOR_PAIR(5));
  refresh();
}

// Animazione di caricamento
void draw_ui_loading_animation() {
  const char *frames[] = {"|", "/", "-", "\\"};
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);
  int y = max_y / 2, x = max_x / 2;
  for (int i = 0; i < 12; ++i) {
    attron(COLOR_PAIR(4));
    mvprintw(y, x, "Loading... %s", frames[i % 4]);
    attroff(COLOR_PAIR(4));
    refresh();
    usleep(90000);
  }
  clear();
  refresh();
}

// Funzione per configurare ncurses
void setup_ui_ncurses() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  use_default_colors();
  init_pair(1, COLOR_BLUE, -1);    // Bordo blu
  init_pair(2, COLOR_GREEN, -1);   // Testo verde
  init_pair(3, COLOR_YELLOW, -1);  // Bordo giallo
  init_pair(4, COLOR_RED, -1);     // Highlight rosso
  init_pair(5, COLOR_CYAN, -1);    // Titolo Matrix
  init_pair(6, COLOR_WHITE, COLOR_MAGENTA); // Colorblind highlight
  init_pair(7, COLOR_BLACK, COLOR_WHITE);   // Footer
}

// Funzione per gestire la logica del menu
void handle_ui_menu() {
  int start_y, start_x, height, width;
  getmaxyx(stdscr, height, width);
  start_y = 8;
  start_x = 5;
  int n_choices = 10;
  int choice;
  int highlight = 0;
  int prev_highlight = -1;
  int redraw = 1;
  draw_ui_loading_animation();
  WINDOW *menu_win = newwin(height - 8, width - 10, start_y, start_x);
  keypad(menu_win, TRUE);
  while (1) {
    if (redraw || highlight != prev_highlight) {
      erase(); // Pulisce stdscr (solo sfondo)
      draw_ui_matrix_title();
      werase(menu_win); // Pulisce solo la finestra menu
      draw_ui_border(menu_win);
      draw_ui_menu(menu_win, highlight);
      wnoutrefresh(stdscr); // Prepara stdscr
      wnoutrefresh(menu_win); // Prepara menu_win
      doupdate(); // Aggiorna tutto insieme (evita flicker/sovrapposizioni)
      prev_highlight = highlight;
      redraw = 0;
    }
    choice = wgetch(menu_win);
    switch (choice) {
    case KEY_UP:
      highlight = (highlight == 0) ? n_choices - 1 : highlight - 1;
      break;
    case KEY_DOWN:
      highlight = (highlight == n_choices - 1) ? 0 : highlight + 1;
      break;
    case 'c':
    case 'C':
      colorblind_mode = !colorblind_mode;
      redraw = 1;
      break;
    case 10: // Enter
      delwin(menu_win);
      endwin();
      if (highlight == 0) play_breakout_game();
      else if (highlight == 1) play_minesweeper_game();
      else if (highlight == 2) play_pacman_game();
      else if (highlight == 3) play_pong_game();
      else if (highlight == 4) play_snake_game();
      else if (highlight == 5) play_sudoku_game();
      else if (highlight == 6) play_tetris_game();
      else if (highlight == 7) {
        setup_ui_ncurses();
        clear();
        attron(COLOR_PAIR(2));
        mvprintw(3, 5, "Usa le frecce per muoverti nel menu. Invio per selezionare. Q per uscire. C per cambiare colori.");
        mvprintw(5, 5, "Ogni gioco ha controlli specifici, consultare la documentazione.");
        attroff(COLOR_PAIR(2));
        getch();
      } else if (highlight == 8) {
        setup_ui_ncurses();
        clear();
        attron(COLOR_PAIR(5));
        mvprintw(3, 5, "RETRO-GAMES - (C) 2025 4137314");
        mvprintw(5, 5, "https://github.com/4137314/retro-games");
        mvprintw(7, 5, "GNU GPL v3 - Powered by ncurses");
        attroff(COLOR_PAIR(5));
        getch();
      } else if (highlight == 9) {
        cleanup_ui_ncurses();
        exit(0);
      }
      setup_ui_ncurses();
      menu_win = newwin(height - 8, width - 10, start_y, start_x);
      keypad(menu_win, TRUE);
      draw_ui_border(menu_win);
      draw_ui_menu(menu_win, highlight);
      redraw = 1;
      break;
    case 'q':
    case 'Q':
      delwin(menu_win);
      cleanup_ui_ncurses();
      exit(0);
      break;
    default:
      break;
    }
  }
}
