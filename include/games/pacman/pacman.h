#ifndef PACMAN_H
#define PACMAN_H

#define PACMAN_CHAR 'C'
#define GHOST_CHAR 'G'
#define FOOD_CHAR '*'
#define WALL_CHAR '#'

typedef struct {
    int x, y;
    int dx, dy;
} Pacman;

typedef struct {
    int x, y;
    int dx, dy;
} Ghost;

typedef struct {
    int width, height;
    int food_x, food_y;
    int wall_count;
    int walls[100][2];  // Assumiamo al massimo 100 muri
    Ghost ghosts[4];   // Assumiamo al massimo 4 fantasmini
    int score;         // Punteggio
} PacmanGameState;

void pacman_initialize_game(Pacman *pacman, PacmanGameState *game_state);
void pacman_update_game(Pacman *pacman, PacmanGameState *game_state);
void pacman_draw_game(const Pacman *pacman, const PacmanGameState *game_state);
void pacman_draw_food(const PacmanGameState *game_state);
void pacman_draw_walls(const PacmanGameState *game_state);
void pacman_draw_ghosts(const PacmanGameState *game_state);
void pacman_spawn_food(PacmanGameState *game_state);
void play_pacman_game();

// UI/logic helpers
void pacman_init_colors();
void pacman_draw_score(int score);
int pacman_check_ghost_collision(const Pacman *pacman, const PacmanGameState *game_state);
void pacman_show_game_over(int score);

#endif // PACMAN_H
