#include "breakout.h"
#include <ncurses.h>
#include <stdlib.h>

#define PADDLE_SPEED 2 // Velocità del paddle (numero di unità per movimento)

void initialize_breakout_game(BreakoutGameState *game_state, int width, int height) {
    game_state->width = width;
    game_state->height = height;
    game_state->paddle_x = width / 2 - 5; // Centro il paddle
    game_state->ball_x = width / 2;
    game_state->ball_y = height - 4;
    game_state->ball_dir_x = 1;
    game_state->ball_dir_y = -1;
    game_state->score = 0;
    game_state->level = 1;
    game_state->lives = 3; // Inizializza le vite

    // Calcola il numero di righe di mattoni in base all'altezza dello schermo
    int num_rows = height / 4; // Ad esempio, una riga di mattoni ogni 4 righe
    game_state->bricks = (int **)malloc(num_rows * sizeof(int *));
    for (int i = 0; i < num_rows; i++) {
        game_state->bricks[i] = (int *)malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) {
            game_state->bricks[i][j] = 1; // Inizializza tutti i mattoni
        }
    }
}

void draw_breakout_game(const BreakoutGameState *game_state) {
    clear();

    int start_y = (LINES - game_state->height) / 2;
    int start_x = (COLS - game_state->width) / 2;

    // Disegna il bordo del campo di gioco
    for (int i = 0; i < game_state->width; i++) {
        mvprintw(start_y, start_x + i, "-");
        mvprintw(start_y + game_state->height - 1, start_x + i, "-");
    }
    for (int i = 0; i < game_state->height; i++) {
        mvprintw(start_y + i, start_x, "|");
        mvprintw(start_y + i, start_x + game_state->width - 1, "|");
    }

    // Disegna i mattoni
    for (int i = 0; i < (game_state->height / 4); i++) {
        for (int j = 0; j < game_state->width; j++) {
            if (game_state->bricks[i][j]) {
                mvprintw(start_y + i + 1, start_x + j, "#");
            }
        }
    }

    // Disegna il paddle
    mvprintw(start_y + game_state->height - 2, start_x + game_state->paddle_x, "=====");

    // Disegna la palla
    mvprintw(start_y + game_state->ball_y, start_x + game_state->ball_x, "O");

    // Disegna il punteggio, il livello e le vite
    mvprintw(start_y - 1, start_x, "Score: %d", game_state->score);
    mvprintw(start_y - 1, start_x + 20, "Level: %d", game_state->level);
    mvprintw(start_y - 1, start_x + 40, "Lives: %d", game_state->lives);

    refresh();
}


void update_breakout_ball_position(BreakoutGameState *game_state) {
    int new_x = game_state->ball_x + game_state->ball_dir_x;
    int new_y = game_state->ball_y + game_state->ball_dir_y;

    // Controllo delle collisioni con i muri
    if (new_x <= 1 || new_x >= game_state->width - 2) {
        game_state->ball_dir_x *= -1;
    }
    if (new_y <= 1) {
        game_state->ball_dir_y *= -1;
    }

    // Controllo delle collisioni con il paddle
    if (new_y == game_state->height - 3 && new_x >= game_state->paddle_x && new_x < game_state->paddle_x + 5) {
        game_state->ball_dir_y *= -1;
    }

    // Controllo delle collisioni con i mattoni
    if (new_y < (game_state->height / 4) && game_state->bricks[new_y][new_x]) {
        game_state->bricks[new_y][new_x] = 0;
        game_state->ball_dir_y *= -1;
        game_state->score += 10;
    }

    // Controllo se la palla è persa
    if (new_y >= game_state->height - 1) {
        game_state->lives--;
        if (game_state->lives == 0) {
            show_breakout_game_over_screen(game_state->score);
        } else {
            // Riposiziona la palla
            game_state->ball_x = game_state->width / 2;
            game_state->ball_y = game_state->height - 4;
            game_state->ball_dir_x = 1;
            game_state->ball_dir_y = -1;
        }
    }

    game_state->ball_x += game_state->ball_dir_x;
    game_state->ball_y += game_state->ball_dir_y;
}

void show_breakout_start_screen() {
    clear();
    mvprintw(LINES / 2 - 1, COLS / 2 - 10, "Welcome to Breakout Game!");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press any key to start");
    refresh();
    getch();
}

void show_breakout_game_over_screen(int score) {
    clear();
    mvprintw(LINES / 2 - 1, COLS / 2 - 10, "Game Over!");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Your score: %d", score);
    mvprintw(LINES / 2 + 3, COLS / 2 - 10, "Press 'q' to quit or any other key to restart");
    refresh();
    int ch = getch();
    if (ch == 'q') {
        endwin();
        exit(0);
    } else {
        play_breakout_game(); // Riavvia il gioco
    }
}

void play_breakout_game() {
    BreakoutGameState game_state;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100); // Imposta timeout per getch()

    // Rileva le dimensioni dello schermo
    int width = COLS - 1; // Larghezza dello schermo
    int height = LINES;   // Altezza dello schermo

    show_breakout_start_screen();
    initialize_breakout_game(&game_state, width, height);

    while (1) {
        ch = getch();
        if (ch == 'q') {
            break;
        }
        switch (ch) {
            case KEY_LEFT:
                game_state.paddle_x = (game_state.paddle_x > PADDLE_SPEED) ? game_state.paddle_x - PADDLE_SPEED : 0;
                break;
            case KEY_RIGHT:
                game_state.paddle_x = (game_state.paddle_x < width - 6 - PADDLE_SPEED) ? game_state.paddle_x + PADDLE_SPEED : width - 6;
                break;
            default:
                break;
        }

        update_breakout_ball_position(&game_state);
        draw_breakout_game(&game_state);
    }

    clear();
    refresh();
    endwin();

    for (int i = 0; i < (height / 4); i++) {
        free(game_state.bricks[i]);
    }
    free(game_state.bricks);
}
