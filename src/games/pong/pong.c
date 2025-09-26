// pong.c
#include "games/pong/pong.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

void initialize_pong_game(PongGameState *game_state) {
    int width = COLS;
    int height = LINES;

    // Imposta le dimensioni del gioco
    game_state->width = width;
    game_state->height = height;

    game_state->left_paddle.x = 1;
    game_state->left_paddle.y = height / 2 - PADDLE_HEIGHT / 2;
    game_state->left_paddle.dx = 0;
    game_state->left_paddle.dy = 0;
    game_state->left_paddle.width = PADDLE_WIDTH;
    game_state->left_paddle.height = PADDLE_HEIGHT;

    game_state->right_paddle.x = width - 2;
    game_state->right_paddle.y = height / 2 - PADDLE_HEIGHT / 2;
    game_state->right_paddle.dx = 0;
    game_state->right_paddle.dy = 0;
    game_state->right_paddle.width = PADDLE_WIDTH;
    game_state->right_paddle.height = PADDLE_HEIGHT;

    game_state->ball.x = width / 2;
    game_state->ball.y = height / 2;
    game_state->ball.dx = (rand() % 2 == 0) ? 1 : -1;
    game_state->ball.dy = (rand() % 2 == 0) ? 1 : -1;

    // Imposta una velocità iniziale visibile per la pallina
    game_state->ball.dx *= 0.5;  // Velocità iniziale della pallina
    game_state->ball.dy *= 0.5;
}

void update_pong_game(PongGameState *game_state) {
    // Muove i paddle
    game_state->left_paddle.y += game_state->left_paddle.dy;
    game_state->right_paddle.y += game_state->right_paddle.dy;

    // Limiti per i paddle
    if (game_state->left_paddle.y < 1) game_state->left_paddle.y = 1;
    if (game_state->left_paddle.y > game_state->height - game_state->left_paddle.height - 1) game_state->left_paddle.y = game_state->height - game_state->left_paddle.height - 1;
    if (game_state->right_paddle.y < 1) game_state->right_paddle.y = 1;
    if (game_state->right_paddle.y > game_state->height - game_state->right_paddle.height - 1) game_state->right_paddle.y = game_state->height - game_state->right_paddle.height - 1;

    // Muove la pallina
    game_state->ball.x += game_state->ball.dx;
    game_state->ball.y += game_state->ball.dy;

    // Rimbalza la pallina dai bordi superiore e inferiore
    if (game_state->ball.y <= 1 || game_state->ball.y >= game_state->height - BALL_SIZE - 1) {
        game_state->ball.dy *= -1;
    }

    // Rimbalza la pallina dai paddle
    if ((game_state->ball.x == game_state->left_paddle.x + game_state->left_paddle.width && game_state->ball.y >= game_state->left_paddle.y && game_state->ball.y < game_state->left_paddle.y + game_state->left_paddle.height) ||
        (game_state->ball.x == game_state->right_paddle.x - BALL_SIZE && game_state->ball.y >= game_state->right_paddle.y && game_state->ball.y < game_state->right_paddle.y + game_state->right_paddle.height)) {
        game_state->ball.dx *= -1;
        game_state->ball.dx *= 1.05;  // Aumenta leggermente la velocità della pallina
        game_state->ball.dy *= 1.05;  // Aumenta leggermente la velocità della pallina
    }

    // Pallina fuori dai confini (lato sinistro)
    if (game_state->ball.x < 1) {
        game_state->ball.x = game_state->width / 2;
        game_state->ball.y = game_state->height / 2;
        game_state->ball.dx = (rand() % 2 == 0) ? 1 : -1;  // Ripristina la direzione della pallina
        game_state->ball.dy = (rand() % 2 == 0) ? 1 : -1;  // Ripristina la direzione della pallina
        game_state->ball.dx *= 0.5;  // Ripristina la velocità della pallina
        game_state->ball.dy *= 0.5;
    }

    // Pallina fuori dai confini (lato destro)
    if (game_state->ball.x >= game_state->width - BALL_SIZE - 1) {
        game_state->ball.x = game_state->width / 2;
        game_state->ball.y = game_state->height / 2;
        game_state->ball.dx = (rand() % 2 == 0) ? 1 : -1;  // Ripristina la direzione della pallina
        game_state->ball.dy = (rand() % 2 == 0) ? 1 : -1;  // Ripristina la direzione della pallina
        game_state->ball.dx *= 0.5;  // Ripristina la velocità della pallina
        game_state->ball.dy *= 0.5;
    }

    // AI per il paddle destro
    if (game_state->ball.dx > 0) {  // Solo muovi il paddle AI se la pallina si sta avvicinando
        if (game_state->ball.y < game_state->right_paddle.y + (game_state->right_paddle.height / 2)) {
            game_state->right_paddle.dy = -1;
        } else if (game_state->ball.y > game_state->right_paddle.y + (game_state->right_paddle.height / 2)) {
            game_state->right_paddle.dy = 1;
        } else {
            game_state->right_paddle.dy = 0;
        }
    } else {
        game_state->right_paddle.dy = 0;  // Il paddle AI si ferma quando la pallina si allontana
    }
}

void draw_pong_game(const PongGameState *game_state) {
    clear(); // Pulisce lo schermo prima di ridisegnare

    // Disegna i bordi
    for (int x = 0; x < game_state->width; ++x) {
        mvaddch(0, x, '-');
        mvaddch(game_state->height - 1, x, '-');
    }
    for (int y = 0; y < game_state->height; ++y) {
        mvaddch(y, 0, '|');
        mvaddch(y, game_state->width - 1, '|');
    }

    // Disegna i paddle
    for (int i = 0; i < game_state->left_paddle.height; ++i) {
        mvaddch(game_state->left_paddle.y + i, game_state->left_paddle.x, '|');
    }
    for (int i = 0; i < game_state->right_paddle.height; ++i) {
        mvaddch(game_state->right_paddle.y + i, game_state->right_paddle.x, '|');
    }

    // Disegna la pallina
    mvaddch(game_state->ball.y, game_state->ball.x, 'O');

    refresh(); // Rende visibile il contenuto aggiornato
}

void play_pong_game() {
    PongGameState game_state;
    int ch;

    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(30);  // Imposta il timeout per getch() in millisecondi

    initialize_pong_game(&game_state);

    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                game_state.right_paddle.dy = -1;
                break;
            case KEY_DOWN:
                game_state.right_paddle.dy = 1;
                break;
            case 'w':
                game_state.left_paddle.dy = -1;
                break;
            case 's':
                game_state.left_paddle.dy = 1;
                break;
            default:
                game_state.left_paddle.dy = 0;
                game_state.right_paddle.dy = 0;
                break;
        }

        update_pong_game(&game_state);
        draw_pong_game(&game_state);
    }

    endwin(); // Termina la modalità ncurses
}
