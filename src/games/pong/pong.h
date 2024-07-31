// pong.h
#ifndef PONG_H
#define PONG_H

#define PADDLE_WIDTH 1
#define PADDLE_HEIGHT 5
#define BALL_SIZE 1

typedef struct {
    int x, y;
    int dx, dy;
    int width, height;
} Paddle;

typedef struct {
    int x, y;
    int dx, dy;
} Ball;

typedef struct {
    int width, height;
    Paddle left_paddle, right_paddle;
    Ball ball;
} PongGameState;

void initialize_pong_game(PongGameState *game_state);
void update_pong_game(PongGameState *game_state);
void draw_pong_game(const PongGameState *game_state);
void play_pong_game();

#endif // PONG_H
