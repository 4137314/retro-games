# Progetto ncurses

Questo progetto dimostra l'uso di `ncurses` per creare un'interfaccia utente a terminale in C.

## Struttura del Progetto

- **src/**: Contiene il codice sorgente del programma.
- **bin/**: Directory per i file eseguibili compilati.
- **Makefile**: Script di compilazione.
- **README.md**: Documentazione generale del progetto.

## Compilazione

Per compilare il progetto, eseguire:

```sh
make
```

# [ui.c](src/ui/ui.c)

- setup_ui_ncurses
	- Inizializza schermo
	- Disabilita buffering testo
	- Non mostra caratteri digitati
	- Abilita funzioni speciali tastiera
	- Nasconde cursore
	- Abilita colori
	- Definisce colore bordo
	- Definisce colore testo

- handle_ui_menu
	- Ottiene dimensione terminale
	- Definisce spazio per titolo
	- Definisce spazio per menu
	- Pulisce schermo
	- Disegna titolo
	- Disegna bordo finestra menu
	- Disegna menu con scelta multipla

- cleanup_ui_ncurses
	- Termina la modalità ncurses

- draw_ui_border
	- Disegna il bordo della finestra

- draw_ui_menu
	- Disegna il menu

- display_ui_info

# [breakout.c](src/games/breakout/breakout.c)

- initialize_breakout_game

- draw_breakout_game

- update_breakout_ball_position

- show_brakout_start_screen

- show_breakout_game_over_screen

- play_breakout_game


# [minesweeper.c](src/games/minesweeper/minesweeper.c)

- initialize_minesweeper_game

- draw_minesweeper_game

- reveal_cell

- play_minesweeper_game

# [pacman.c](src/games/pacman/pacman.c)

- pacman_initialize_game

- pacman_update_game

- pacman_drwa_game

- pacman_draw_food

- pacman_draw_walls

- pacman_draw_ghosts

- pacman_draw_border

- pacman_spawn_food

- play_pacman_game

# [pong.c](src/games/pong/pong.c)

- initialize_pong_game

- update_pong_game

- drwa_pong_game

- play_pong_game

# [snake.c](src/games/snake/snake.c)

- initialize_snake_game

- initialize_snake_food

- update_snake_game

- draw_snake_game

- draw_snake_food

- spawn_snake_food

- draw_snake_score

- show_snake_start_screen

- show_snake_game_over_screen

- initialize_snake_colors

- draw_snake_border

- draw_snake_menu

- play_snake_game

# [sudoku.c](src/games/sudoku/sudoku.c)

- initialize_sudoku_game

- draw_sudoku_grid

- draw_sudoku_game

- draw_sudoku_status

- play_sudoku_game

- sudoku_is_valid

- solve_sudoku

- generate_sudoku

# [tetris.c](src/games/tetris/tetris.c)

- initialize_tetris_game

- draw_tetris_board

- drwa_tetromino

- clear_sudoku_lines

- spawn_tetromino

- rotate_tetromino

- rotate_tetromino

- move_tetromino_left

- move_tetromino_right

- move_tetromino_down

- drop_tetromino

- check_collision

- play_tetris_game

# `Makefile`

Questo makefile di solito funziona $\Rightarrow$ usare questo!

```makefile
# Nome del target eseguibile
TARGET = bin/main

# Directory per i file oggetto
OBJ_DIR = obj

# Directory per i file sorgente
SRC_DIR = src

# Trova i file sorgente e crea le variabili per i file oggetto
SRC = $(wildcard $(SRC_DIR)/games/*/*.c) $(SRC_DIR)/ui/ui.c main.c
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Target binario
$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	gcc -o $@ $(OBJ) `pkg-config --libs ncurses`

# Regole di compilazione per i file oggetto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	gcc -Wall -I$(SRC_DIR) `pkg-config --cflags ncurses` -c -o $@ $<

# Compilazione di main.o separatamente
$(OBJ_DIR)/main.o: main.c
	@mkdir -p $(dir $@)
	gcc -Wall -I$(SRC_DIR) `pkg-config --cflags ncurses` -c -o $@ $<

# Regola di pulizia
clean:
	rm -f $(TARGET)
	rm -rf $(OBJ_DIR)

.PHONY: clean
```