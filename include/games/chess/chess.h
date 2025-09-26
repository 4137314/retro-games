#ifndef CHESS_H
#define CHESS_H

typedef struct {
    int board[8][8];
    int turn; // 0 = bianco, 1 = nero
    int selected_x, selected_y;
    int game_over;
} ChessGameState;

void initialize_chess_game(ChessGameState *game_state);
void draw_chess_game(const ChessGameState *game_state);
void play_chess_game();

#endif // CHESS_H
