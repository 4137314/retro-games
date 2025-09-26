bin_PROGRAMS = retro-games
retro_games_SOURCES = \
	src/retro-games.c \
	src/ui/ui.c \
	src/games/breakout/breakout.c \
	src/games/minesweeper/minesweeper.c \
	src/games/pacman/pacman.c \
	src/games/pong/pong.c \
	src/games/snake/snake.c \
	src/games/sudoku/sudoku.c \
	src/games/tetris/tetris.c \
	src/games/chess/chess.c

AM_CPPFLAGS = -I$(top_srcdir)/include
AM_LDFLAGS =
LDADD = -lncurses

EXTRA_DIST = README LICENSE configure.ac Makefile.am
