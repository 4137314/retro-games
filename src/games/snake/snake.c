#include "snake.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

// Inizializza lo stato iniziale del serpente e del gioco
void initialize_snake_game(Snake *snake, SnakeGameState *game_state) {
    snake->length = 5;  // Lunghezza iniziale del serpente
    snake->max_length = game_state->width * game_state->height;  // Lunghezza massima del serpente basata sulla dimensione del campo di gioco
    snake->body_x = (int *)malloc(snake->max_length * sizeof(int));  // Allocazione dinamica della memoria per le coordinate X del corpo del serpente
    snake->body_y = (int *)malloc(snake->max_length * sizeof(int));  // Allocazione dinamica della memoria per le coordinate Y del corpo del serpente

    for (int i = 0; i < snake->length; ++i) {
        snake->body_x[i] = game_state->width / 2 - i;  // Inizializza le coordinate X del corpo del serpente al centro del campo di gioco
        snake->body_y[i] = game_state->height / 2;  // Inizializza le coordinate Y del corpo del serpente al centro del campo di gioco
    }

    snake->dx = 1;  // Direzione iniziale del movimento lungo l'asse X
    snake->dy = 0;  // Direzione iniziale del movimento lungo l'asse Y
    snake->score = 0;  // Punteggio iniziale

    initialize_snake_food(game_state);  // Inizializza il cibo nel campo di gioco
}

// Inizializza lo stato del cibo nel campo di gioco
void initialize_snake_food(SnakeGameState *game_state) {
    game_state->food_count = game_state->level + 2;  // Imposta il numero di pezzi di cibo in base al livello del gioco
    game_state->food_x = (int *)malloc(game_state->food_count * sizeof(int));  // Allocazione dinamica della memoria per le coordinate X del cibo
    game_state->food_y = (int *)malloc(game_state->food_count * sizeof(int));  // Allocazione dinamica della memoria per le coordinate Y del cibo

    for (int i = 0; i < game_state->food_count; ++i) {
        game_state->food_x[i] = -1;  // Inizializza le coordinate X del cibo a -1 (fuori dal campo di gioco)
        game_state->food_y[i] = -1;  // Inizializza le coordinate Y del cibo a -1 (fuori dal campo di gioco)
    }

    spawn_snake_food(game_state);  // Genera il cibo in posizioni casuali nel campo di gioco
}

// Aggiorna lo stato del gioco del serpente
void update_snake_game(Snake *snake, SnakeGameState *game_state) {
    int new_x = snake->body_x[0] + snake->dx;  // Calcola la nuova posizione X della testa del serpente
    int new_y = snake->body_y[0] + snake->dy;  // Calcola la nuova posizione Y della testa del serpente

    // Controlla se il serpente ha colpito i bordi del campo di gioco
    if (new_x < 0 || new_x >= game_state->width || new_y < 0 || new_y >= game_state->height) {
        game_state->game_over = 1;  // Se ha colpito i bordi, imposta il flag game_over a 1
        return;
    }

    // Controlla se il serpente ha colpito se stesso
    for (int i = 0; i < snake->length; ++i) {
        if (snake->body_x[i] == new_x && snake->body_y[i] == new_y) {
            game_state->game_over = 1;  // Se ha colpito se stesso, imposta il flag game_over a 1
            return;
        }
    }

    // Aggiorna la posizione del corpo del serpente
    for (int i = snake->length; i > 0; --i) {
        snake->body_x[i] = snake->body_x[i - 1];
        snake->body_y[i] = snake->body_y[i - 1];
    }

    // Aggiorna la testa del serpente
    snake->body_x[0] = new_x;
    snake->body_y[0] = new_y;

    // Controlla se il serpente ha mangiato del cibo
    for (int i = 0; i < game_state->food_count; ++i) {
        if (new_x == game_state->food_x[i] && new_y == game_state->food_y[i]) {
            snake->length++;  // Incrementa la lunghezza del serpente
            snake->score += 10;  // Incrementa il punteggio
            game_state->food_x[i] = -1;  // Rimuove il cibo mangiato dal campo di gioco
            game_state->food_y[i] = -1;
            spawn_snake_food(game_state);  // Genera un nuovo pezzo di cibo in una posizione casuale
            break;
        }
    }
}

// Disegna il serpente sullo schermo
void draw_snake_game(Snake *snake) {
    for (int i = 0; i < snake->length; ++i) {
        mvaddch(snake->body_y[i], snake->body_x[i], i == 0 ? 'O' : 'o');  // Disegna la testa del serpente come 'O' e il corpo come 'o'
    }
}

// Disegna il cibo sullo schermo
void draw_snake_food(SnakeGameState *game_state) {
    for (int i = 0; i < game_state->food_count; ++i) {
        if (game_state->food_x[i] != -1 && game_state->food_y[i] != -1) {
            mvaddch(game_state->food_y[i], game_state->food_x[i], '*');  // Disegna il cibo come '*'
        }
    }
}

// Genera cibo in posizioni casuali nel campo di gioco
void spawn_snake_food(SnakeGameState *game_state) {
    for (int i = 0; i < game_state->food_count; ++i) {
        if (game_state->food_x[i] == -1 && game_state->food_y[i] == -1) {
            int x, y;
            do {
                x = rand() % game_state->width;  // Genera una posizione X casuale
                y = rand() % game_state->height;  // Genera una posizione Y casuale
            } while (x == 0 || y == 0);  // Evita di posizionare il cibo sui bordi

            game_state->food_x[i] = x;
            game_state->food_y[i] = y;
        }
    }
}

// Disegna il punteggio del serpente sullo schermo
void draw_snake_score(Snake *snake, SnakeGameState *game_state) {
    mvprintw(0, 0, "Score: %d", snake->score);  // Stampa il punteggio
    mvprintw(1, 0, "Level: %d", game_state->level);  // Stampa il livello del gioco
}

// Mostra la schermata di avvio del gioco
void show_snake_start_screen() {
    clear();  // Pulisce lo schermo
    mvprintw(LINES / 2, COLS / 2 - 10, "Press 'c' to start the game");  // Mostra il messaggio di avvio
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 'q' to quit");  // Mostra il messaggio per uscire
    refresh();  // Aggiorna lo schermo
}

// Mostra la schermata di game over
void show_snake_game_over_screen(int score) {
    clear();  // Pulisce lo schermo
    mvprintw(LINES / 2, COLS / 2 - 10, "Game Over! Your score: %d", score);  // Mostra il punteggio finale
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 'q' to quit");  // Mostra il messaggio per uscire
    refresh();  // Aggiorna lo schermo
}

// Inizializza i colori per il gioco Snake
void initialize_snake_colors() {
    start_color();  // Abilita il supporto ai colori in ncurses
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Colore verde per il serpente
    init_pair(2, COLOR_RED, COLOR_BLACK);  // Colore rosso per il cibo
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);  // Colore giallo per altri elementi
}

// Disegna il bordo del campo di gioco
void draw_snake_border(WINDOW *win) {
    int width, height;
    getmaxyx(win, height, width);  // Ottiene le dimensioni della finestra

    // Disegna il bordo superiore e inferiore
    for (int x = 0; x < width; ++x) {
        mvwaddch(win, 0, x, '-');
        mvwaddch(win, height - 1, x, '-');
    }

    // Disegna i bordi laterali
    for (int y = 0; y < height; ++y) {
        mvwaddch(win, y, 0, '|');
        mvwaddch(win, y, width - 1, '|');
    }

    // Disegna gli angoli
    mvwaddch(win, 0, 0, '+');
    mvwaddch(win, 0, width - 1, '+');
    mvwaddch(win, height - 1, 0, '+');
    mvwaddch(win, height - 1, width - 1, '+');
}

// Disegna il menu del gioco Snake
void draw_snake_menu(WINDOW *menu_win, int highlight) {
    char *choices[] = { "Level 1", "Level 2", "Level 3", "Quit" };  // Opzioni del menu
    int n_choices = sizeof(choices) / sizeof(char *);  // Numero di opzioni del menu
    box(menu_win, 0, 0);  // Disegna il bordo della finestra del menu
    for (int i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE);  // Evidenzia la scelta corrente
        }
        mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);  // Stampa l'opzione del menu
        wattroff(menu_win, A_REVERSE);  // Disattiva l'evidenziazione
    }
    wrefresh(menu_win);  // Aggiorna la finestra del menu
}

// Esegue il gioco Snake
void play_snake_game() {
    Snake snake;
    SnakeGameState game_state;
    int ch;
    int highlight = 1;
    int level = 1;

    srand(time(NULL));  // Inizializza il generatore di numeri casuali
    initscr();  // Inizializza la modalità ncurses
    cbreak();  // Disabilita il buffering della riga
    noecho();  // Disabilita l'eco dell'input dell'utente
    keypad(stdscr, TRUE);  // Abilita la lettura dei tasti speciali
    curs_set(0);  // Nasconde il cursore
    timeout(100);  // Imposta il timeout per getch()

    // Crea la finestra del menu
    int menu_height = 10;
    int menu_width = 20;
    int start_y = (LINES - menu_height) / 2;
    int start_x = (COLS - menu_width) / 2;
    WINDOW *menu_win = newwin(menu_height, menu_width, start_y, start_x);

    draw_snake_menu(menu_win, highlight);  // Disegna il menu
    while ((ch = getch()) != 'q') {  // Continua a leggere i tasti finché non viene premuto 'q'
        switch (ch) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = 4;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == 4)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: // Enter key
                if (highlight >= 1 && highlight <= 3) {
                    level = highlight;  // Imposta il livello di difficoltà
                    game_state.level = level;
                    game_state.game_over = 0;
                    game_state.width = COLS; // Imposta la larghezza del campo di gioco
                    game_state.height = LINES; // Imposta l'altezza del campo di gioco
                    initialize_snake_game(&snake, &game_state);  // Inizializza il gioco del serpente
                    initialize_snake_colors();  // Inizializza i colori del gioco
                    delwin(menu_win);  // Elimina la finestra del menu
                    clear();  // Pulisce lo schermo
                    goto start_game;  // Salta alla fase di gioco
                } else if (highlight == 4) {
                    delwin(menu_win);  // Elimina la finestra del menu
                    endwin();  // Termina la modalità ncurses
                    return;
                }
                break;
        }

        draw_snake_menu(menu_win, highlight);  // Ridisegna il menu con la nuova evidenziazione
    }

start_game:
    while (!game_state.game_over && (ch = getch()) != 'q') {  // Continua il gioco finché non è game over o viene premuto 'q'
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

        update_snake_game(&snake, &game_state);  // Aggiorna lo stato del gioco
        clear();  // Pulisce lo schermo
        draw_snake_border(stdscr);  // Disegna il bordo del campo di gioco
        draw_snake_game(&snake);  // Disegna il serpente
        draw_snake_food(&game_state);  // Disegna il cibo
        draw_snake_score(&snake, &game_state);  // Disegna il punteggio
        refresh();  // Aggiorna lo schermo
        usleep(1 / game_state.level);  // Regola la velocità del gioco in base al livello
    }

    show_snake_game_over_screen(snake.score);  // Mostra la schermata di game over

    while ((ch = getch()) != 'q') {
        // Attende che l'utente prema 'q' per uscire
    }

    endwin();  // Termina la modalità ncurses
    free(snake.body_x);  // Libera la memoria allocata per le coordinate X del serpente
    free(snake.body_y);  // Libera la memoria allocata per le coordinate Y del serpente
    free(game_state.food_x);  // Libera la memoria allocata per le coordinate X del cibo
    free(game_state.food_y);  // Libera la memoria allocata per le coordinate Y del cibo
}