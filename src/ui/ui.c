#include <signal.h>
/*
 * Handle terminal resize (SIGWINCH) by setting a global flag.
 */
volatile sig_atomic_t ui_resized = 0;
void handle_winch(int sig) {
  (void)sig;
  ui_resized = 1;
}
#include "ui/ui.h"
#include <stdbool.h>

/* Colorblind mode flag.
  1 = enabled, 0 = disabled.
  This variable enables colorblind-friendly color schemes in the UI.
*/
int colorblind_mode = 0;
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "games/games.h" /* Include game headers. */
#include "games/games.h"

/*
 * Clean up ncurses and restore terminal state.
 */
void cleanup_ui_ncurses() {
  endwin();
}

/*
 * Draw the border of the given window with color pairs.
 */
void draw_ui_border(WINDOW *win) {
  wbkgd(win, COLOR_PAIR(1)); /* Blue background */
  wattron(win, COLOR_PAIR(3));
  box(win, 0, 0);            /* Yellow border */
  wattroff(win, COLOR_PAIR(3));
  wrefresh(win);
}

/*
 * Draw the main menu in the given window.
 * The highlighted item is shown with a different color.
 */
void draw_ui_menu(WINDOW *win, int highlight) {
  char *choices[] = {"Breakout", "Minesweeper", "Pac-Man", "Pong",
                     "Snake",    "Sudoku",      "Tetris", "Help", "Credits", "Exit"};
  int n_choices = sizeof(choices) / sizeof(char *);
  int win_width, win_height;
  getmaxyx(win, win_height, win_width);
  int menu_width = 0;
  for (int i = 0; i < n_choices; ++i) {
    int len = strlen(choices[i]);
    if (len > menu_width) menu_width = len;
  }
  menu_width += 6; /* Space for > < and padding */
  int x = (win_width - menu_width) / 2;
  int y = (win_height - n_choices) / 2;
  for (int i = 0; i < n_choices; ++i) {
    if (highlight == i) {
      wattron(win, colorblind_mode ? COLOR_PAIR(6) : COLOR_PAIR(4));
      wattron(win, A_BOLD | A_REVERSE);
      mvwprintw(win, y + i, x, "> %-*s <", menu_width - 4, choices[i]);
      wattroff(win, A_BOLD | A_REVERSE);
      wattroff(win, colorblind_mode ? COLOR_PAIR(6) : COLOR_PAIR(4));
    } else {
      wattron(win, COLOR_PAIR(2));
      mvwprintw(win, y + i, x, "  %-*s  ", menu_width - 4, choices[i]);
      wattroff(win, COLOR_PAIR(2));
    }
  }
  /* Footer is always centered. */
  const char *footer = "Arrows: move  Enter: select  Q: quit  C: colors";
  wattron(win, COLOR_PAIR(7));
  mvwprintw(win, win_height - 2, (win_width - (int)strlen(footer)) / 2, "%s", footer);
  wattroff(win, COLOR_PAIR(7));
  wrefresh(win);
}

/*
 * Display the program title in Matrix style.
 */
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

/*
 * Show a loading animation in the center of the screen.
 */
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

/*
 * Set up ncurses and initialize color pairs for the UI.
 */
void setup_ui_ncurses() {
  struct sigaction sa;
  sa.sa_handler = handle_winch;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGWINCH, &sa, NULL);
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  use_default_colors();
  init_pair(1, COLOR_BLUE, -1);    /* Blue border */
  init_pair(2, COLOR_GREEN, -1);   /* Green text */
  init_pair(3, COLOR_YELLOW, -1);  /* Yellow border */
  init_pair(4, COLOR_RED, -1);     /* Red highlight */
  init_pair(5, COLOR_CYAN, -1);    /* Matrix title */
  init_pair(6, COLOR_WHITE, COLOR_MAGENTA); /* Colorblind highlight */
  init_pair(7, COLOR_BLACK, COLOR_WHITE);   /* Footer */
}

/*
 * Handle the main menu logic and user input loop.
 */
void handle_ui_menu() {
  int min_width = 40, min_height = 18;
  int start_y, start_x, height, width;
  getmaxyx(stdscr, height, width);
  // Center menu for any terminal size
  start_y = (height - 18) / 2;
  if (start_y < 0) start_y = 0;
  start_x = (width - 40) / 2;
  if (start_x < 0) start_x = 0;
  while (width < min_width || height < min_height) {
    clear();
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(height/2, (width-28)/2, "Terminal too small! Resize, please.");
    attroff(COLOR_PAIR(4) | A_BOLD);
    refresh();
    int ch = getch();
    getmaxyx(stdscr, height, width);
    if (ch == 'q' || ch == 'Q') {
      cleanup_ui_ncurses();
      exit(0);
    }
  }
  clear();
  refresh();
  int n_choices = 10;
  int choice;
  int highlight = 0;
  int prev_highlight = -1;
  int redraw = 1;
  draw_ui_loading_animation();
  WINDOW *menu_win = newwin(height - 8, width - 10, start_y, start_x);
  keypad(menu_win, TRUE);
  while (1) {
    if (ui_resized) {
      ui_resized = 0;
      endwin();
      refresh();
      clearok(stdscr, TRUE);
      refresh();
      getmaxyx(stdscr, height, width);
      start_y = (height - 18) / 2;
      if (start_y < 0) start_y = 0;
      start_x = (width - 40) / 2;
      if (start_x < 0) start_x = 0;
      while (width < min_width || height < min_height) {
        clear();
        attron(COLOR_PAIR(4) | A_BOLD);
        mvprintw(height/2, (width-28)/2, "Terminal too small! Resize, please.");
        attroff(COLOR_PAIR(4) | A_BOLD);
        refresh();
        int ch = getch();
        getmaxyx(stdscr, height, width);
        start_y = (height - 18) / 2;
        if (start_y < 0) start_y = 0;
        start_x = (width - 40) / 2;
        if (start_x < 0) start_x = 0;
        if (ch == 'q' || ch == 'Q') {
          cleanup_ui_ncurses();
          exit(0);
        }
      }
      clear();
      refresh();
      delwin(menu_win);
      menu_win = newwin(height - 8, width - 10, start_y, start_x);
      keypad(menu_win, TRUE);
      redraw = 1;
    }
    if (redraw || highlight != prev_highlight) {
      erase(); /* Clear stdscr (background only) */
      draw_ui_matrix_title();
      werase(menu_win); /* Clear only the menu window */
      draw_ui_border(menu_win);
      draw_ui_menu(menu_win, highlight);
      wnoutrefresh(stdscr); /* Prepare stdscr */
      wnoutrefresh(menu_win); /* Prepare menu_win */
      doupdate(); /* Update all at once (avoids flicker/overlap) */
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
  case 10: /* Enter */
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
