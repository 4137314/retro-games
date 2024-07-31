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
