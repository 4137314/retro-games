# --- Configurazione ---
CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -Iinclude -Iinclude/games -Iinclude/ui
LDFLAGS := 
# Aggiungiamo la libreria ncurses qui
LDLIBS  := -lncurses
TARGET  := retro-games

# --- Individuazione automatica dei sorgenti ---
SRCS := $(shell find src -name "*.c")
OBJS := $(SRCS:.c=.o)

# --- Regole Principali ---

.PHONY: all clean help

all: $(TARGET)

$(TARGET): $(OBJS)
	# Nota l'aggiunta di $(LDLIBS) alla fine del comando di link
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LDLIBS)
	@echo "Build completata: $(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Pulizia completata."
