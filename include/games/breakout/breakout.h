#ifndef BREAKOUT_H
#define BREAKOUT_H

typedef struct {
    int width;
    int height;
    int paddle_x;
    int ball_x;
    int ball_y;
    int ball_dir_x;
    int ball_dir_y;
    int **bricks;
    int score;
    int level;
    int lives; // Numero di vite
} BreakoutGameState;

void initialize_breakout_game(BreakoutGameState *game_state, int width, int height);
void draw_breakout_game(const BreakoutGameState *game_state);
void update_breakout_ball_position(BreakoutGameState *game_state);
void play_breakout_game();
void show_breakout_start_screen();
void show_breakout_game_over_screen(int score);

#endif // BREAKOUT_H
