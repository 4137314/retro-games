#ifndef SNAKE_H
#define SNAKE_H

#include <ncurses.h> /* For WINDOW type recognition */

typedef struct {
    int length;
    int max_length;
    int *body_x;
    int *body_y;
    int dx;
    int dy;
    int score; /* Member to keep track of the score */
} Snake;

typedef struct {
    int width;
    int height;
    int *food_x;
    int *food_y;
    int food_count;
    int game_over;
    int level;
    int *obstacle_x;
    int *obstacle_y;
    int obstacle_count;
    int special_food_x;
    int special_food_y;
    int special_food_timer;
} SnakeGameState;

void initialize_snake_game(Snake *snake, SnakeGameState *game_state);
void update_snake_game(Snake *snake, SnakeGameState *game_state);
void draw_snake_game(Snake *snake);
void draw_snake_food(SnakeGameState *game_state);
void spawn_snake_food(SnakeGameState *game_state);
/*
 * Initialize the food state for the Snake game.
 */
void initialize_snake_food(SnakeGameState *game_state);
void draw_snake_score(Snake *snake, SnakeGameState *game_state);
void show_snake_start_screen();
void show_snake_game_over_screen(int score);
void initialize_snake_colors();
void play_snake_game();
void draw_snake_menu(WINDOW *menu_win, int highlight);
void draw_snake_border(WINDOW *win);
void draw_snake_obstacles(SnakeGameState *game_state);
void spawn_snake_obstacles(SnakeGameState *game_state);
void draw_snake_special_food(SnakeGameState *game_state);
void show_snake_countdown();

#endif /* SNAKE_H */
