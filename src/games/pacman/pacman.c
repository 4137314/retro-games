#include "games/pacman/pacman.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

void pacman_initialize_game(Pacman *pacman, PacmanGameState *game_state) {
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
    mvaddch(pacman->y, pacman->x, PACMAN_CHAR);

    // Disegna i fantasmini
    pacman_draw_ghosts(game_state);

    // Disegna il cibo
    pacman_draw_food(game_state);

    // Disegna i muri
    pacman_draw_walls(game_state);
}

void pacman_draw_food(const PacmanGameState *game_state) {
    mvaddch(game_state->food_y, game_state->food_x, FOOD_CHAR);
}

void pacman_draw_walls(const PacmanGameState *game_state) {
    for (int i = 0; i < game_state->wall_count; ++i) {
        mvaddch(game_state->walls[i][1], game_state->walls[i][0], WALL_CHAR);
    }
}

void pacman_draw_ghosts(const PacmanGameState *game_state) {
    for (int i = 0; i < 4; ++i) {
        mvaddch(game_state->ghosts[i].y, game_state->ghosts[i].x, GHOST_CHAR);
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

    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);

    getmaxyx(stdscr, game_state.height, game_state.width);
    pacman_initialize_game(&pacman, &game_state);

    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                if (pacman.dy == 0) {
                    pacman.dx = 0;
                    pacman.dy = -1;
                }
                break;
            case KEY_DOWN:
                if (pacman.dy == 0) {
                    pacman.dx = 0;
                    pacman.dy = 1;
                }
                break;
            case KEY_LEFT:
                if (pacman.dx == 0) {
                    pacman.dx = -1;
                    pacman.dy = 0;
                }
                break;
            case KEY_RIGHT:
                if (pacman.dx == 0) {
                    pacman.dx = 1;
                    pacman.dy = 0;
                }
                break;
        }

        pacman_update_game(&pacman, &game_state);

        clear();
        pacman_draw_border(stdscr);
        pacman_draw_game(&pacman, &game_state);
        refresh();
    }

    clear();
    refresh();
    endwin();
}
