#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include <stdio.h>
#include <stdlib.h>

void print_help(void) {
    printf("Commandes disponibles :\n");
    printf("h : Afficher l'aide\n");
    printf("r : Reinitialiser la grille (shuffle)\n");
    printf("q : Quitter\n");
    printf("c i j : Tourner la pièce en (i,j) dans le sens horaire \n");
    printf("a i j : Tourner la pièce en (i,j) dans le sens anti-horaire \n");
    printf("z : Annuler le dernière coup joué \n");
    printf("y : Revenir au dernière coup annulé \n");
}

void usage(int argc, char* argv[]) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        usage(argc, argv);
        exit(EXIT_FAILURE);
    }
    game g = NULL;
    if (argc == 2) {
        char* filename = argv[1];
        g = game_load(filename);
    } else {
        g = game_default();
    }
    // les variables
    char command;
    int i, j;
    bool game_over = false;
    while (!game_over && !game_won(g)) {
        // afficher le jeu
        game_print(g);
        printf("\n");
        printf("Entrer une commande h pour l'aide\n");
        scanf(" %c", &command);
        if (command == 'h') {
            print_help();
        } else if (command == 'r') {
            game_shuffle_orientation(g);
            printf("Grille reinitialisee\n");
        } else if (command == 'q') {
            game_over = true;
            printf("Vous avez abandonne la partie\n");
        } else if (command == 'z') {
            game_undo(g);
        } else if (command == 'y') {
            game_redo(g);
        } else if (command == 'c' || command == 'a') {
            printf("Entrez les coordonnes i et j \n");
            scanf("%d %d", &i, &j);
            if (i >= 0 && i < DEFAULT_SIZE && j >= 0 && j < DEFAULT_SIZE) {
                if (command == 'c') {
                    game_play_move(g, i, j, 1);
                } else if (command == 'a') {
                    game_play_move(g, i, j, -1);
                }
            } else {
                printf("Les coordonnes sont incorrectes\n");
            }
        }
    }
    // afficher la grille
    game_print(g);
    printf("\n");
    // verifier si la partie est gagnee
    if (game_won(g)) {
        printf("Congratulations: Vous avez gagné la partie \n");
    } else if (game_over) {
        printf("SHAME!, vous avez abandonné la partie \n");
    }
    game_delete(g);
    return EXIT_SUCCESS;
}