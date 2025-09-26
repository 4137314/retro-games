
#include <ncurses.h>
#define PACMAN_COLOR 1
#define GHOST_COLOR 2
#define FOOD_COLOR 3
#define WALL_COLOR 4
#include "games/pacman/pacman.h"

// Funzione: collisione con i fantasmi
int pacman_check_ghost_collision(const Pacman *pacman, const PacmanGameState *game_state) {
    for (int i = 0; i < 4; ++i) {
        if (pacman->x == game_state->ghosts[i].x && pacman->y == game_state->ghosts[i].y)
            return 1;
    }
    return 0;
}

// Funzione: schermata di game over
void pacman_show_game_over(int score) {
    clear();
    attron(COLOR_PAIR(GHOST_COLOR) | A_BOLD);
    mvprintw(LINES / 2, COLS / 2 - 8, "GAME OVER! Score: %d", score);
    attroff(COLOR_PAIR(GHOST_COLOR) | A_BOLD);
    attron(COLOR_PAIR(FOOD_COLOR));
    mvprintw(LINES / 2 + 1, COLS / 2 - 12, "Premi 'r' per restart o 'q' per uscire");
    attroff(COLOR_PAIR(FOOD_COLOR));
    refresh();
}

void pacman_init_colors() {
    start_color();
    init_pair(PACMAN_COLOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(GHOST_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(FOOD_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(WALL_COLOR, COLOR_WHITE, COLOR_BLACK);
}

void pacman_draw_score(int score) {
    attron(A_BOLD);
    mvprintw(0, 2, "Score: %d", score);
    attroff(A_BOLD);
}
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

void pacman_initialize_game(Pacman *pacman, PacmanGameState *game_state) {
    game_state->score = 0;
    pacman->x = game_state->width / 2;
    pacman->y = game_state->height / 2;
    pacman->dx = 1;
    pacman->dy = 0;

    // Inizializzazione del cibo
    pacman_spawn_food(game_state);

    // Inizializzazione dei muri
    game_state->wall_count = 10; // Ad esempio, 10 muri
    for (int i = 0; i < game_state->wall_count; ++i) {
        game_state->walls[i][0] = rand() % (game_state->width - 2) + 1;
        game_state->walls[i][1] = rand() % (game_state->height - 2) + 1;
    }

    // Inizializzazione dei fantasmini
    for (int i = 0; i < 4; ++i) {
        game_state->ghosts[i].x = rand() % (game_state->width - 2) + 1;
        game_state->ghosts[i].y = rand() % (game_state->height - 2) + 1;
        game_state->ghosts[i].dx = (rand() % 2) * 2 - 1; // Velocità casuale: -1 o 1
        game_state->ghosts[i].dy = (rand() % 2) * 2 - 1; // Velocità casuale: -1 o 1
    }
}

void pacman_update_game(Pacman *pacman, PacmanGameState *game_state) {
    // Aggiorna la posizione di Pac-Man
    pacman->x += pacman->dx;
    pacman->y += pacman->dy;

    // Controlla il cibo
    if (pacman->x == game_state->food_x && pacman->y == game_state->food_y) {
        game_state->score += 10;
        pacman_spawn_food(game_state);
    }

    // Aggiorna i fantasmini
    for (int i = 0; i < 4; ++i) {
        game_state->ghosts[i].x += game_state->ghosts[i].dx;
        game_state->ghosts[i].y += game_state->ghosts[i].dy;

        // Rimbalza i fantasmini sui bordi dello schermo
        if (game_state->ghosts[i].x <= 0 || game_state->ghosts[i].x >= game_state->width - 1) {
            game_state->ghosts[i].dx *= -1;
        }
        if (game_state->ghosts[i].y <= 0 || game_state->ghosts[i].y >= game_state->height - 1) {
            game_state->ghosts[i].dy *= -1;
        }
    }

    // Controlla la collisione con i muri
    for (int i = 0; i < game_state->wall_count; ++i) {
        if (pacman->x == game_state->walls[i][0] && pacman->y == game_state->walls[i][1]) {
            // Ripristina la posizione di Pac-Man se colpisce un muro
            pacman->x -= pacman->dx;
            pacman->y -= pacman->dy;
        }
    }
}

void pacman_draw_game(const Pacman *pacman, const PacmanGameState *game_state) {
    // Disegna Pac-Man
    attron(COLOR_PAIR(PACMAN_COLOR) | A_BOLD);
    mvaddch(pacman->y, pacman->x, PACMAN_CHAR);
    attroff(COLOR_PAIR(PACMAN_COLOR) | A_BOLD);

    // Disegna i fantasmini
    pacman_draw_ghosts(game_state);

    // Disegna il cibo
    pacman_draw_food(game_state);

    // Disegna i muri
    pacman_draw_walls(game_state);
}

void pacman_draw_food(const PacmanGameState *game_state) {
    attron(COLOR_PAIR(FOOD_COLOR) | A_BOLD);
    mvaddch(game_state->food_y, game_state->food_x, FOOD_CHAR);
    attroff(COLOR_PAIR(FOOD_COLOR) | A_BOLD);
}

void pacman_draw_walls(const PacmanGameState *game_state) {
    for (int i = 0; i < game_state->wall_count; ++i) {
        attron(COLOR_PAIR(WALL_COLOR));
        mvaddch(game_state->walls[i][1], game_state->walls[i][0], WALL_CHAR);
        attroff(COLOR_PAIR(WALL_COLOR));
    }
}

void pacman_draw_ghosts(const PacmanGameState *game_state) {
    for (int i = 0; i < 4; ++i) {
        attron(COLOR_PAIR(GHOST_COLOR) | A_BOLD);
        mvaddch(game_state->ghosts[i].y, game_state->ghosts[i].x, GHOST_CHAR);
        attroff(COLOR_PAIR(GHOST_COLOR) | A_BOLD);
    }
int pacman_check_ghost_collision(const Pacman *pacman, const PacmanGameState *game_state) {
    for (int i = 0; i < 4; ++i) {
        if (pacman->x == game_state->ghosts[i].x && pacman->y == game_state->ghosts[i].y)
            return 1;
    }
    return 0;
}

void pacman_show_game_over(int score) {
    clear();
    attron(COLOR_PAIR(GHOST_COLOR) | A_BOLD);
    mvprintw(LINES / 2, COLS / 2 - 8, "GAME OVER! Score: %d", score);
    attroff(COLOR_PAIR(GHOST_COLOR) | A_BOLD);
    attron(COLOR_PAIR(FOOD_COLOR));
    mvprintw(LINES / 2 + 1, COLS / 2 - 12, "Premi 'r' per restart o 'q' per uscire");
    attroff(COLOR_PAIR(FOOD_COLOR));
    refresh();
}
}

void pacman_draw_border(WINDOW *win) {
    int width, height;
    getmaxyx(win, height, width);

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

void pacman_spawn_food(PacmanGameState *game_state) {
    game_state->food_x = rand() % (game_state->width - 2) + 1;
    game_state->food_y = rand() % (game_state->height - 2) + 1;
}

void play_pacman_game() {
    Pacman pacman;
    PacmanGameState game_state;
    int ch;
    int speed = 120000;

    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);

    getmaxyx(stdscr, game_state.height, game_state.width);
    pacman_init_colors();
    pacman_initialize_game(&pacman, &game_state);

restart_game:
    while (1) {
        ch = getch();
        switch (ch) {
            case KEY_UP:
                if (pacman.dy == 0) { pacman.dx = 0; pacman.dy = -1; }
                break;
            case KEY_DOWN:
                if (pacman.dy == 0) { pacman.dx = 0; pacman.dy = 1; }
                break;
            case KEY_LEFT:
                if (pacman.dx == 0) { pacman.dx = -1; pacman.dy = 0; }
                break;
            case KEY_RIGHT:
                if (pacman.dx == 0) { pacman.dx = 1; pacman.dy = 0; }
                break;
        }
        pacman_update_game(&pacman, &game_state);
        if (pacman_check_ghost_collision(&pacman, &game_state)) {
            pacman_show_game_over(game_state.score);
            while (1) {
                ch = getch();
                if (ch == 'q') { endwin(); return; }
                if (ch == 'r') {
                    pacman_initialize_game(&pacman, &game_state);
                    clear();
                    goto restart_game;
                }
            }
        }
        erase();
        pacman_draw_border(stdscr);
        pacman_draw_game(&pacman, &game_state);
        pacman_draw_score(game_state.score);
        refresh();
        usleep(speed);
    }
}
