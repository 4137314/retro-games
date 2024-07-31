#ifndef SNAKE_H
#define SNAKE_H

#include <ncurses.h> // Per riconoscere il tipo WINDOW

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
    int *food_x; // Cambiato a puntatore per gestire più pezzi di cibo
    int *food_y; // Cambiato a puntatore per gestire più pezzi di cibo
    int food_count; // Nuovo membro per tenere traccia del numero di pezzi di cibo
    int game_over; // Nuovo membro per indicare la fine del gioco
    int level;     // Nuovo membro per i livelli del gioco
} SnakeGameState;

void initialize_snake_game(Snake *snake, SnakeGameState *game_state);
void update_snake_game(Snake *snake, SnakeGameState *game_state);
void draw_snake_game(Snake *snake);
void draw_snake_food(SnakeGameState *game_state);
void spawn_snake_food(SnakeGameState *game_state);
void initialize_snake_food(SnakeGameState *game_state); // Nuova dichiarazione
void draw_snake_score(Snake *snake, SnakeGameState *game_state);
void show_snake_start_screen();
void show_snake_game_over_screen(int score);
void initialize_snake_colors();
void play_snake_game();
void draw_snake_menu(WINDOW *menu_win, int highlight);
void draw_snake_border(WINDOW *win);

#endif // SNAKE_H
