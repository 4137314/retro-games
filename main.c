#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "/home/oudeys/Documenti/Coding/C/ncurses-retro-games/src/ui/ui.h" // Includi l'header per le funzioni di interfaccia utente

int main() {
    setup_ui_ncurses(); // Configura ncurses

    handle_ui_menu(); // Gestisce il menu e avvia i giochi

    cleanup_ui_ncurses(); // Pulisce e termina ncurses
    return 0;
}
