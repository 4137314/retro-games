#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>
#include "games/snake/snake.h"

/*
 * Draw obstacles on the game field.
 */
void draw_snake_obstacles(SnakeGameState *game_state) {
    attron(COLOR_PAIR(3));
    for (int i = 0; i < game_state->obstacle_count; ++i) {
        mvaddch(game_state->obstacle_y[i], game_state->obstacle_x[i], '#');
    }
    attroff(COLOR_PAIR(3));
}

/*
 * Generate random obstacles for the current level.
 */
void spawn_snake_obstacles(SnakeGameState *game_state) {
    if (game_state->obstacle_count > 0) {
        free(game_state->obstacle_x);
        free(game_state->obstacle_y);
    }
    game_state->obstacle_count = 2 * game_state->level;
    game_state->obstacle_x = malloc(game_state->obstacle_count * sizeof(int));
    game_state->obstacle_y = malloc(game_state->obstacle_count * sizeof(int));
    for (int i = 0; i < game_state->obstacle_count; ++i) {
        int x, y;
        do {
            x = rand() % (game_state->width - 2) + 1;
            y = rand() % (game_state->height - 2) + 1;
        } while (x == 0 || y == 0);
        game_state->obstacle_x[i] = x;
        game_state->obstacle_y[i] = y;
    }
}

/*
 * Draw special food if the timer is active.
 */
void draw_snake_special_food(SnakeGameState *game_state) {
    if (game_state->special_food_timer > 0) {
        attron(COLOR_PAIR(2) | A_BLINK);
        mvaddch(game_state->special_food_y, game_state->special_food_x, '$');
        attroff(COLOR_PAIR(2) | A_BLINK);
    }
}

/*
 * Show an animated countdown before the game starts.
 */
void show_snake_countdown() {
    for (int i = 3; i > 0; --i) {
        erase();
        mvprintw(LINES / 2, COLS / 2 - 4, "START IN %d", i);
        refresh();
        usleep(700000);
    }
    erase();
    mvprintw(LINES / 2, COLS / 2 - 4, "GO!");
    refresh();
    usleep(500000);
    erase();
    refresh();
}

/*
 * Initialize the initial state of the snake and the game.
 */
void initialize_snake_game(Snake *snake, SnakeGameState *game_state) {
    spawn_snake_obstacles(game_state);
    game_state->special_food_timer = 0;
    snake->length = 5;  /* Initial snake length */
    snake->max_length = game_state->width * game_state->height;  /* Maximum snake length based on field size */
    snake->body_x = (int *)malloc(snake->max_length * sizeof(int));  /* Dynamic allocation for X coordinates */
    snake->body_y = (int *)malloc(snake->max_length * sizeof(int));  /* Dynamic allocation for Y coordinates */

    for (int i = 0; i < snake->length; ++i) {
    snake->body_x[i] = game_state->width / 2 - i;  /* Center X coordinates */
    snake->body_y[i] = game_state->height / 2;    /* Center Y coordinates */
    }

    snake->dx = 1;  /* Initial X direction */
    snake->dy = 0;  /* Initial Y direction */
    snake->score = 0;  /* Initial score */

    initialize_snake_food(game_state);  /* Initialize food on the field */
}

/*
 * Initialize the food state on the field.
 */
void initialize_snake_food(SnakeGameState *game_state) {
    game_state->food_count = game_state->level + 2;  /* Number of food pieces based on level */
    game_state->food_x = (int *)malloc(game_state->food_count * sizeof(int));  /* Dynamic allocation for food X */
    game_state->food_y = (int *)malloc(game_state->food_count * sizeof(int));  /* Dynamic allocation for food Y */

    for (int i = 0; i < game_state->food_count; ++i) {
    game_state->food_x[i] = -1;  /* Initialize food X to -1 (off field) */
    game_state->food_y[i] = -1;  /* Initialize food Y to -1 (off field) */
    }

    spawn_snake_food(game_state);  /* Generate food at random positions */
}

/*
 * Update the state of the snake game.
 */
void update_snake_game(Snake *snake, SnakeGameState *game_state) {
    /* Collision with obstacles */
    for (int i = 0; i < game_state->obstacle_count; ++i) {
        if (snake->body_x[0] == game_state->obstacle_x[i] && snake->body_y[0] == game_state->obstacle_y[i]) {
            game_state->game_over = 1;
            return;
        }
    }

    /* Special food */
    if (game_state->special_food_timer > 0 && snake->body_x[0] == game_state->special_food_x && snake->body_y[0] == game_state->special_food_y) {
        snake->score += 50;
        game_state->special_food_timer = 0;
    }
    /* Special food timer */
    if (game_state->special_food_timer > 0) game_state->special_food_timer--;
    int new_x = snake->body_x[0] + snake->dx;  /* New X position for snake head */
    int new_y = snake->body_y[0] + snake->dy;  /* New Y position for snake head */

    /* Check if snake hit the field borders */
    if (new_x < 0 || new_x >= game_state->width || new_y < 0 || new_y >= game_state->height) {
    game_state->game_over = 1;  /* Set game_over if hit border */
        return;
    }

    /* Check if snake hit itself */
    for (int i = 0; i < snake->length; ++i) {
        if (snake->body_x[i] == new_x && snake->body_y[i] == new_y) {
            game_state->game_over = 1;  /* Set game_over if hit itself */
            return;
        }
    }

    /* Update snake body position */
    for (int i = snake->length; i > 0; --i) {
        snake->body_x[i] = snake->body_x[i - 1];
        snake->body_y[i] = snake->body_y[i - 1];
    }

    /* Update snake head */
    snake->body_x[0] = new_x;
    snake->body_y[0] = new_y;

    /* Check if snake ate food */
    for (int i = 0; i < game_state->food_count; ++i) {
        if (new_x == game_state->food_x[i] && new_y == game_state->food_y[i]) {
            snake->length++;  /* Increase snake length */
            snake->score += 10;  /* Increase score */
            game_state->food_x[i] = -1;  /* Remove eaten food from field */
            game_state->food_y[i] = -1;
            spawn_snake_food(game_state);  /* Generate new food at random position */
            break;
        }
    }
}

/*
 * Draw the snake on the screen.
 */
void draw_snake_game(Snake *snake) {
    for (int i = 0; i < snake->length; ++i) {
        if (i == 0) {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvaddch(snake->body_y[i], snake->body_x[i], 'O'); /* Green head */
            attroff(COLOR_PAIR(1) | A_BOLD);
        } else {
            attron(COLOR_PAIR(3));
            mvaddch(snake->body_y[i], snake->body_x[i], 'o'); /* Yellow body */
            attroff(COLOR_PAIR(3));
        }
    }
}

/*
 * Draw the food on the screen.
 */
void draw_snake_food(SnakeGameState *game_state) {
    for (int i = 0; i < game_state->food_count; ++i) {
        if (game_state->food_x[i] != -1 && game_state->food_y[i] != -1) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvaddch(game_state->food_y[i], game_state->food_x[i], '@');  /* Red food */
            attroff(COLOR_PAIR(2) | A_BOLD);
        }
    }
}

/*
 * Generate food at random positions on the field.
 */
void spawn_snake_food(SnakeGameState *game_state) {
    for (int i = 0; i < game_state->food_count; ++i) {
        if (game_state->food_x[i] == -1 && game_state->food_y[i] == -1) {
            int x, y;
            do {
                x = rand() % game_state->width;  /* Random X position */
                y = rand() % game_state->height;  /* Random Y position */
            } while (x == 0 || y == 0);  /* Avoid placing food on borders */

            game_state->food_x[i] = x;
            game_state->food_y[i] = y;
        }
    }
}

/*
 * Draw the snake's score on the screen.
 */
void draw_snake_score(Snake *snake, SnakeGameState *game_state) {
    attron(A_BOLD);
    mvprintw(0, 2, "Score: %d", snake->score);  /* Print score */
    mvprintw(0, 20, "Level: %d", game_state->level);  /* Print level */
    attroff(A_BOLD);
}

/*
 * Show the start screen for the game.
 */
void show_snake_start_screen() {
    clear();  /* Clear the screen */
    mvprintw(LINES / 2, COLS / 2 - 10, "Press 'c' to start the game");  /* Show start message */
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 'q' to quit");  /* Show quit message */
    refresh();  /* Refresh the screen */
}

/*
 * Show the game over screen.
 */
void show_snake_game_over_screen(int score) {
    clear();
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(LINES / 2, COLS / 2 - 10, "GAME OVER! Score: %d", score);
    attroff(COLOR_PAIR(2) | A_BOLD);
    attron(COLOR_PAIR(3));
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Premi 'r' per restart o 'q' per uscire");
    attroff(COLOR_PAIR(3));
    refresh();
}

/*
 * Initialize color pairs for the Snake game.
 */
void initialize_snake_colors() {
    start_color();  /* Enable color support in ncurses */
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  /* Green for snake */
    init_pair(2, COLOR_RED, COLOR_BLACK);    /* Red for food */
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); /* Yellow for other elements */
}

/*
 * Draw the border of the game field.
 */
void draw_snake_border(WINDOW *win) {
    int width, height;
    getmaxyx(win, height, width);  /* Get window size */

    /* Draw top and bottom borders */
    for (int x = 0; x < width; ++x) {
        mvwaddch(win, 0, x, '-');
        mvwaddch(win, height - 1, x, '-');
    }

    /* Draw side borders */
    for (int y = 0; y < height; ++y) {
        mvwaddch(win, y, 0, '|');
        mvwaddch(win, y, width - 1, '|');
    }

    /* Draw corners */
    mvwaddch(win, 0, 0, '+');
    mvwaddch(win, 0, width - 1, '+');
    mvwaddch(win, height - 1, 0, '+');
    mvwaddch(win, height - 1, width - 1, '+');
}

/*
 * Draw the menu for the Snake game.
 */
void draw_snake_menu(WINDOW *menu_win, int highlight) {
    char *choices[] = { "Level 1", "Level 2", "Level 3", "Quit" };  /* Menu options */
    int n_choices = sizeof(choices) / sizeof(char *);  /* Number of menu options */
    box(menu_win, 0, 0);  /* Draw menu window border */
    for (int i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE);  /* Highlight current choice */
        }
    mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);  /* Print menu option */
    wattroff(menu_win, A_REVERSE);  /* Disable highlight */
    }
    wrefresh(menu_win);  /* Refresh menu window */
}

/*
 * Main function to run the Snake game.
 */
void play_snake_game() {
    static int high_score = 0;
    Snake snake;
    SnakeGameState game_state;
    int ch;
    int highlight = 1;
    int level = 1;

    srand(time(NULL));  /* Initialize random number generator */
    initscr();  /* Initialize ncurses mode */
    cbreak();  /* Disable line buffering */
    noecho();  /* Disable user input echo */
    keypad(stdscr, TRUE);  /* Enable special keys */
    curs_set(0);  /* Hide cursor */
    timeout(100);  /* Set getch() timeout */

    /* Create the menu window */
    int menu_height = 10;
    int menu_width = 20;
    int start_y = (LINES - menu_height) / 2;
    int start_x = (COLS - menu_width) / 2;
    WINDOW *menu_win = newwin(menu_height, menu_width, start_y, start_x);

    draw_snake_menu(menu_win, highlight);  /* Draw the menu */
    while ((ch = getch()) != 'q') {  /* Read keys until 'q' is pressed */
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
                    level = highlight;  /* Set difficulty level */
                    game_state.level = level;
                    game_state.game_over = 0;
                    game_state.width = COLS; /* Set field width */
                    game_state.height = LINES; /* Set field height */
                    initialize_snake_game(&snake, &game_state);  /* Initialize snake game */
                    initialize_snake_colors();  /* Initialize colors */
                    delwin(menu_win);  /* Delete menu window */
                    clear();  /* Clear screen */
                    goto start_game;  /* Go to game phase */
                } else if (highlight == 4) {
                    delwin(menu_win);  /* Delete menu window */
                    endwin();  /* End ncurses mode */
                    return;
                }
                break;
        }

    draw_snake_menu(menu_win, highlight);  /* Redraw menu with new highlight */
    }

start_game:
    int speed = 120000 - (game_state.level * 20000);
    if (speed < 40000) speed = 40000;
    int just_ate = 0;
    show_snake_countdown();
    int slowdown = 0;
    while (!game_state.game_over && (ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                if (snake.dy == 0) { snake.dx = 0; snake.dy = -1; }
                break;
            case KEY_DOWN:
                if (snake.dy == 0) { snake.dx = 0; snake.dy = 1; }
                break;
            case KEY_LEFT:
                if (snake.dx == 0) { snake.dx = -1; snake.dy = 0; }
                break;
            case KEY_RIGHT:
                if (snake.dx == 0) { snake.dx = 1; snake.dy = 0; }
                break;
        }
        int old_score = snake.score;
        update_snake_game(&snake, &game_state);
        just_ate = (snake.score > old_score);
    /* Special food every 5 food eaten */
        if (snake.length % 5 == 0 && game_state.special_food_timer == 0) {
            game_state.special_food_x = rand() % (game_state.width - 2) + 1;
            game_state.special_food_y = rand() % (game_state.height - 2) + 1;
            game_state.special_food_timer = 40 + rand() % 30;
        }
        erase();
        draw_snake_border(stdscr);
        draw_snake_obstacles(&game_state);
        draw_snake_game(&snake);
        draw_snake_food(&game_state);
        draw_snake_special_food(&game_state);
        draw_snake_score(&snake, &game_state);
        mvprintw(0, 40, "High Score: %d", (snake.score > high_score) ? snake.score : high_score);
        mvprintw(1, 2, "Lunghezza: %d", snake.length);
        refresh();
        if (just_ate) {
            beep(); flash();
            if (game_state.special_food_timer > 0 && snake.body_x[0] == game_state.special_food_x && snake.body_y[0] == game_state.special_food_y) {
                slowdown = 10;
            }
        }
        if (slowdown > 0) {
            usleep(speed * 2);
            slowdown--;
        } else {
            usleep(speed);
        }
    }
    if (snake.score > high_score) high_score = snake.score;

restart_game:
    for (int i = 0; i < 6; ++i) {
        attron(COLOR_PAIR(2) | A_BOLD);
        draw_snake_border(stdscr);
        attroff(COLOR_PAIR(2) | A_BOLD);
        refresh();
        usleep(100000);
        erase();
        refresh();
        usleep(60000);
    }
    beep();
    show_snake_game_over_screen(snake.score);
    mvprintw(2, 2, "High Score: %d", high_score);
    refresh();
    while (1) {
        ch = getch();
        if (ch == 'q') break;
        if (ch == 'r') {
            free(snake.body_x);
            free(snake.body_y);
            free(game_state.food_x);
            free(game_state.food_y);
            clear();
            goto start_game;
        }
    }
    endwin();
    free(snake.body_x);
    free(snake.body_y);
    free(game_state.food_x);
    free(game_state.food_y);
}