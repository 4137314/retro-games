```c
#include "snake.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/ui/ui.h"

void initialize_snake_game(Snake *snake, SnakeGameState *game_state) {
    snake->length = 1;
    snake->max_length = 100;
    snake->body_x = (int *)malloc(snake->max_length * sizeof(int));
    snake->body_y = (int *)malloc(snake->max_length * sizeof(int));
    snake->body_x[0] = game_state->width / 2;
    snake->body_y[0] = game_state->height / 2;
    snake->dx = 1;
    snake->dy = 0;

    spawn_snake_food(game_state);
}

void update_snake_game(Snake *snake, SnakeGameState *game_state) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body_x[i] = snake->body_x[i - 1];
        snake->body_y[i] = snake->body_y[i - 1];
    }
    snake->body_x[0] += snake->dx;
    snake->body_y[0] += snake->dy;

    if (snake->body_x[0] == game_state->food_x && snake->body_y[0] == game_state->food_y) {
        if (snake->length < snake->max_length) {
            snake->length++;
        }
        spawn_snake_food(game_state);
    }
}

void draw_snake_game(Snake *snake) {
    for (int i = 0; i < snake->length; i++) {
        mvaddch(snake->body_y[i], snake->body_x[i], '*');
    }
}

void draw_snake_food(SnakeGameState *game_state) {
    mvaddch(game_state->food_y, game_state->food_x, '#');
}

void spawn_snake_food(SnakeGameState *game_state) {
    game_state->food_x = rand() % (game_state->width - 2) + 1;
    game_state->food_y = rand() % (game_state->height - 2) + 1;
}

void play_snake_game() {
    Snake snake;
    SnakeGameState game_state;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);

    getmaxyx(stdscr, game_state.height, game_state.width);
    initialize_snake_game(&snake, &game_state);

    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                if (snake.dy == 0) {
                    snake.dx = 0;
                    snake.dy = -1;
                }
                break;
            case KEY_DOWN:
                if (snake.dy == 0) {
                    snake.dx = 0;
                    snake.dy = 1;
                }
                break;
            case KEY_LEFT:
                if (snake.dx == 0) {
                    snake.dx = -1;
                    snake.dy = 0;
                }
                break;
            case KEY_RIGHT:
                if (snake.dx == 0) {
                    snake.dx = 1;
                    snake.dy = 0;
                }
                break;
        }

        update_snake_game(&snake, &game_state);

        clear();
        draw_border(stdscr);
        draw_snake_game(&snake);
        draw_snake_food(&game_state);
        refresh();
    }

    clear();
    refresh();
    endwin();

    free(snake.body_x);
    free(snake.body_y);
}
```
```c
#ifndef SNAKE_H
#define SNAKE_H

typedef struct {
    int length;
    int max_length;
    int *body_x;
    int *body_y;
    int dx;
    int dy;
    int score; // Nuovo membro per tenere traccia del punteggio
} Snake;

typedef struct {
    int width;
    int height;
    int food_x;
    int food_y;
    int game_over; // Nuovo membro per indicare la fine del gioco
} SnakeGameState;

void initialize_snake_game(Snake *snake, SnakeGameState *game_state);
void update_snake_game(Snake *snake, SnakeGameState *game_state);
void draw_snake_game(Snake *snake);
void draw_snake_food(SnakeGameState *game_state);
void spawn_snake_food(SnakeGameState *game_state);
void play_snake_game();

#endif // SNAKE_H
```

```c
#include <ncurses.h>
#include <stdlib.h>
#include "ui.h"

// Include i file header dei giochi
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/breakout/breakout.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/minesweeper/minesweeper.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/pacman/pacman.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/pong/pong.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/snake/snake.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/sudoku/sudoku.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/tetris/tetris.h"

// Funzione per configurare ncurses
void setup_ncurses() {
    initscr();                // Inizializza lo schermo
    cbreak();                 // Disabilita il buffering del tasto
    noecho();                 // Non mostra i caratteri digitati
    keypad(stdscr, TRUE);     // Abilita le funzioni speciali della tastiera
    curs_set(0);              // Nasconde il cursore
    start_color();            // Abilita i colori
    init_pair(1, COLOR_BLUE, COLOR_BLACK);   // Definisce il colore del bordo
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // Definisce il colore del testo
}

// Funzione per pulire up ncurses
void cleanup_ncurses() {
    endwin();                 // Termina la modalità ncurses
}

// Funzione per disegnare il bordo della finestra
void draw_border(WINDOW *win) {
    wbkgd(win, COLOR_PAIR(1)); // Imposta il colore di sfondo
    box(win, 0, 0);            // Disegna il bordo
    wrefresh(win);             // Aggiorna la finestra
}

// Funzione per disegnare il menu
void draw_menu(WINDOW *win, int highlight) {
    char *choices[] = {
        "Breakout",
        "Minesweeper",
        "Pac-Man",
        "Pong",
        "Snake",
        "Sudoku",
        "Tetris"
    };
    int n_choices = sizeof(choices) / sizeof(char *);
    int x = 2, y = 2;

    for (int i = 0; i < n_choices; ++i) {
        if (highlight == i) {
            wattron(win, A_REVERSE);
            mvwprintw(win, y, x, "%s", choices[i]);
            wattroff(win, A_REVERSE);
        } else {
            mvwprintw(win, y, x, "%s", choices[i]);
        }
        y++;
    }
    wrefresh(win);
}

// Funzione per visualizzare le informazioni
void display_info(WINDOW *win) {
    werase(win);               // Pulisce la finestra
    wbkgd(win, COLOR_PAIR(2)); // Imposta il colore di sfondo
    box(win, 0, 0);            // Disegna il bordo
    mvwprintw(win, 1, 1, "Welcome to the ncurses demo!");
    mvwprintw(win, 2, 1, "Press any key to return.");
    wrefresh(win);
}

// Funzione per gestire la logica del menu
void handle_menu() {
    int height = 15, width = 50, start_y = 5, start_x = 10;
    WINDOW *menu_win = newwin(height, width, start_y, start_x);
    keypad(menu_win, TRUE);

    int n_choices = 9;
    int choice;
    int highlight = 0;

    while (1) {
        draw_border(menu_win); // Disegna il bordo della finestra
        draw_menu(menu_win, highlight); // Disegna il menu
        choice = wgetch(menu_win);

        switch (choice) {
            case KEY_UP:
                highlight = (highlight == 0) ? n_choices - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == n_choices - 1) ? 0 : highlight + 1;
                break;
            case 10:  // Enter key
                if (highlight == 7) {  // Games
                    delwin(menu_win);
                    endwin();
                    // Call to the Games menu function
                    // play_games_menu();  // Implementa la funzione per il sottomenu di giochi
                    setup_ncurses();
                    menu_win = newwin(height, width, start_y, start_x);
                    keypad(menu_win, TRUE);
                    draw_border(menu_win);
                    draw_menu(menu_win, highlight);
                } else if (highlight == 8) {  // Dictionary
                    delwin(menu_win);
                    endwin();
                    // Call to the Dictionary menu function
                    // play_dictionary_menu();  // Implementa la funzione per il sottomenu di dizionario
                    setup_ncurses();
                    menu_win = newwin(height, width, start_y, start_x);
                    keypad(menu_win, TRUE);
                    draw_border(menu_win);
                    draw_menu(menu_win, highlight);
                } else if (highlight < 7) {  // Giochi
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
                    setup_ncurses();
                    menu_win = newwin(height, width, start_y, start_x);
                    keypad(menu_win, TRUE);
                    draw_border(menu_win);
                    draw_menu(menu_win, highlight);
                }
                break;
            case 'q':  // Tasto 'q' per uscire
                delwin(menu_win);
                cleanup_ncurses();
                exit(0);
                break;
            default:
                break;
        }
    }
}
```

```c
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ui.h"

// Include i file header dei giochi
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/breakout/breakout.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/minesweeper/minesweeper.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/pacman/pacman.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/pong/pong.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/snake/snake.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/sudoku/sudoku.h"
#include "/Users/oudeys/Desktop/Programmazione/C/projects/ncurses/src/games/tetris/tetris.h"

// Funzione per configurare ncurses
void setup_ncurses() {
    initscr();                // Inizializza lo schermo
    cbreak();                 // Disabilita il buffering del tasto
    noecho();                 // Non mostra i caratteri digitati
    keypad(stdscr, TRUE);     // Abilita le funzioni speciali della tastiera
    curs_set(0);              // Nasconde il cursore
    start_color();            // Abilita i colori
    init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Definisce il colore del bordo
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Definisce il colore del testo
}

// Funzione per pulire up ncurses
void cleanup_ncurses() {
    endwin();                 // Termina la modalità ncurses
}

// Funzione per disegnare il bordo della finestra
void draw_border(WINDOW *win) {
    wbkgd(win, COLOR_PAIR(1)); // Imposta il colore di sfondo
    box(win, 0, 0);            // Disegna il bordo
    wrefresh(win);             // Aggiorna la finestra
}

// Funzione per disegnare il menu
void draw_menu(WINDOW *win, int highlight) {
    char *choices[] = {
        "Breakout",
        "Minesweeper",
        "Pac-Man",
        "Pong",
        "Snake",
        "Sudoku",
        "Tetris"
    };
    int n_choices = sizeof(choices) / sizeof(char *);
    int x = 2, y = 2;

    for (int i = 0; i < n_choices; ++i) {
        if (highlight == i) {
            wattron(win, A_REVERSE);
            mvwprintw(win, y, x, "%s", choices[i]);
            wattroff(win, A_REVERSE);
        } else {
            mvwprintw(win, y, x, "%s", choices[i]);
        }
        y++;
    }
    wrefresh(win);
}

// Funzione per visualizzare il titolo in stile Matrix
void draw_matrix_title() {
    char *title[] = {
        " _____  _____ _______ _______  _______ ",
        "|  __ \\|  __ \\__   __|__   __||__   __|",
        "| |__) | |  | | | |     | |     | |   ",
        "|  _  /| |  | | | |     | |     | |   ",
        "| | \\ \\| |__| | | |     | |     | |   ",
        "|_|  \\_\\_____/  |_|     |_|     |_|   "
    };

    int row;
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int start_x = (max_x - 55) / 2; // Centra il titolo
    int start_y = 1; // Spazio dal bordo superiore

    for (row = 0; row < 6; ++row) {
        mvprintw(start_y + row, start_x, "%s", title[row]);
    }
    refresh();
}

// Funzione per gestire la logica del menu
void handle_menu() {
    int start_y, start_x, height, width;
    getmaxyx(stdscr, height, width); // Ottieni le dimensioni del terminale
    start_y = 8; // Spazio per il titolo
    start_x = 5;

    WINDOW *menu_win = newwin(height - 8, width - 10, start_y, start_x); // Riduci altezza e larghezza per il menu
    keypad(menu_win, TRUE);

    int n_choices = 7; // Numero di scelte nel menu
    int choice;
    int highlight = 0;

    while (1) {
        clear(); // Pulisce lo schermo
        draw_matrix_title(); // Disegna il titolo in stile Matrix
        draw_border(menu_win); // Disegna il bordo della finestra del menu
        draw_menu(menu_win, highlight); // Disegna il menu
        choice = wgetch(menu_win);

        switch (choice) {
            case KEY_UP:
                highlight = (highlight == 0) ? n_choices - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == n_choices - 1) ? 0 : highlight + 1;
                break;
            case 10:  // Enter key
                if (highlight < n_choices) {  // Giochi
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
                    setup_ncurses();
                    menu_win = newwin(height - 8, width - 10, start_y, start_x);
                    keypad(menu_win, TRUE);
                    draw_border(menu_win);
                    draw_menu(menu_win, highlight);
                }
                break;
            case 'q':  // Tasto 'q' per uscire
                delwin(menu_win);
                cleanup_ncurses();
                exit(0);
                break;
            default:
                break;
        }
    }
}

```

# Snake

```c
#include "snake.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

void initialize_snake_game(Snake *snake, SnakeGameState *game_state) {
    snake->length = 1;
    snake->max_length = 100;
    snake->body_x = (int *)malloc(snake->max_length * sizeof(int));
    snake->body_y = (int *)malloc(snake->max_length * sizeof(int));
    snake->body_x[0] = game_state->width / 2;
    snake->body_y[0] = game_state->height / 2;
    snake->dx = 1;
    snake->dy = 0;
    snake->score = 0;

    spawn_snake_food(game_state);
}

void update_snake_game(Snake *snake, SnakeGameState *game_state) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body_x[i] = snake->body_x[i - 1];
        snake->body_y[i] = snake->body_y[i - 1];
    }
    snake->body_x[0] += snake->dx;
    snake->body_y[0] += snake->dy;

    if (snake->body_x[0] == game_state->food_x && snake->body_y[0] == game_state->food_y) {
        if (snake->length < snake->max_length) {
            snake->length++;
        }
        snake->score++;
        spawn_snake_food(game_state);
        if (snake->score % 5 == 0) {
            game_state->level++;
            timeout(100 - (game_state->level * 10)); // Aumenta la velocità del gioco con ogni livello
        }
    }

    if (snake->body_x[0] <= 0 || snake->body_x[0] >= game_state->width - 1 ||
        snake->body_y[0] <= 0 || snake->body_y[0] >= game_state->height - 1) {
        game_state->game_over = 1;
    }

    for (int i = 1; i < snake->length; i++) {
        if (snake->body_x[0] == snake->body_x[i] && snake->body_y[0] == snake->body_y[i]) {
            game_state->game_over = 1;
        }
    }
}

void draw_snake_game(Snake *snake) {
    attron(COLOR_PAIR(1));
    mvaddch(snake->body_y[0], snake->body_x[0], 'O');  // Testa del serpente
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    for (int i = 1; i < snake->length; i++) {
        mvaddch(snake->body_y[i], snake->body_x[i], 'o');  // Corpo del serpente
    }
    attroff(COLOR_PAIR(2));
}

void draw_snake_food(SnakeGameState *game_state) {
    attron(COLOR_PAIR(3));
    mvaddch(game_state->food_y, game_state->food_x, '#');
    attroff(COLOR_PAIR(3));
}

void spawn_snake_food(SnakeGameState *game_state) {
    game_state->food_x = rand() % (game_state->width - 2) + 1;
    game_state->food_y = rand() % (game_state->height - 2) + 1;
}

void draw_score(Snake *snake, SnakeGameState *game_state) {
    attron(COLOR_PAIR(4));
    mvprintw(0, 2, "Score: %d", snake->score);
    mvprintw(0, 20, "Level: %d", game_state->level);
    attroff(COLOR_PAIR(4));
}

void show_start_screen() {
    clear();
    attron(COLOR_PAIR(5));
    mvprintw(10, 10, "Welcome to Snake Game!");
    mvprintw(12, 10, "Press any key to start");
    attroff(COLOR_PAIR(5));
    refresh();
    getch();
}

void show_game_over_screen(int score) {
    clear();
    attron(COLOR_PAIR(6));
    mvprintw(10, 10, "Game Over!");
    mvprintw(12, 10, "Your score: %d", score);
    mvprintw(14, 10, "Press 'q' to exit or any other key to restart");
    attroff(COLOR_PAIR(6));
    refresh();
}

void initialize_colors() {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Testa del serpente
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // Corpo del serpente
    init_pair(3, COLOR_RED, COLOR_BLACK);    // Cibo
    init_pair(4, COLOR_WHITE, COLOR_BLACK);  // Punteggio
    init_pair(5, COLOR_CYAN, COLOR_BLACK);   // Schermata iniziale
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // Schermata di fine gioco
}

void play_snake_game() {
    Snake snake;
    SnakeGameState game_state;
    int ch;

    srand(time(NULL));

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100); // Imposta il timeout per la lettura dei tasti

    initialize_colors();

    do {
        getmaxyx(stdscr, game_state.height, game_state.width);
        game_state.game_over = 0;
        game_state.level = 1;

        show_start_screen();

        initialize_snake_game(&snake, &game_state);

        while (!game_state.game_over) {
            ch = getch();  // Legge il tasto premuto

            switch (ch) {
                case KEY_UP:
                    if (snake.dy == 0) {
                        snake.dx = 0;
                        snake.dy = -1;
                    }
                    break;
                case KEY_DOWN:
                    if (snake.dy == 0) {
                        snake.dx = 0;
                        snake.dy = 1;
                    }
                    break;
                case KEY_LEFT:
                    if (snake.dx == 0) {
                        snake.dx = -1;
                        snake.dy = 0;
                    }
                    break;
                case KEY_RIGHT:
                    if (snake.dx == 0) {
                        snake.dx = 1;
                        snake.dy = 0;
                    }
                    break;
                case 'q':
                    game_state.game_over = 1; // Esci dal gioco
                    break;
            }

            if (game_state.game_over) {
                break; // Esci dal ciclo di gioco se game_over è impostato
            }

            update_snake_game(&snake, &game_state);

            clear();
            draw_snake_game(&snake);
            draw_snake_food(&game_state);
            draw_score(&snake, &game_state);
            refresh();
        }

        show_game_over_screen(snake.score);

        // Ripristina lo stato di ncurses
        endwin();

        // Riinizializza ncurses per il prossimo ciclo di gioco
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);
        timeout(100);

    } while (getch() != 'q'); // Solo un getch() per controllare l'uscita

    clear();
    refresh();
    endwin();

    free(snake.body_x);
    free(snake.body_y);
}
```