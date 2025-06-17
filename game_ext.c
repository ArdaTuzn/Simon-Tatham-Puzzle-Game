#include "game_ext.h"
#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
#include "queue/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

game game_new_ext(uint nb_rows, uint nb_cols, shape* shapes, direction* orientations, bool wrapping) {
    game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
    uint size = (nb_rows * nb_cols);
    if (shapes != NULL) {
        for (uint i = 0; i < size; i++) {
            if (shapes[i] != EMPTY && shapes[i] != ENDPOINT && shapes[i] != SEGMENT && shapes[i] != CORNER && shapes[i] != TEE && shapes[i] != CROSS) {
                fprintf(stderr, "La taille du tableau shapes est invalide\n");
                exit(EXIT_FAILURE);
            }
            g->s[i] = shapes[i];
        }
    }
    if (orientations != NULL) {
        for (uint i = 0; i < size; i++) {
            if (orientations[i] != NORTH && orientations[i] != EAST && orientations[i] != SOUTH && orientations[i] != WEST) {
                fprintf(stderr, "La taille du tableau orientations est invalide\n");
                exit(EXIT_FAILURE);
            }
            g->d[i] = orientations[i];
        }
    }
    return g;
}

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping) {
    // Valide les parsamètres d'entrée
    if (nb_rows <= 0 || nb_cols <= 0) {
        return NULL;
    }

    // Alloue la mémoire pour la structure du jeu
    game g = malloc(sizeof(game_s));
    if (!g) {
        fprintf(stderr, "Erreur : échec de l'allocation mémoire dans game_new_empty_ext\n");
        exit(EXIT_FAILURE);
    }

    uint size = (nb_rows * nb_cols);

    // Initialise les champs du jeu
    g->nb_rows = nb_rows;
    g->nb_columns = nb_cols;
    g->wrapping = wrapping;
    g->undo_stack = queue_new();
    g->redo_stack = queue_new();

    // Alloue la mémoire pour les formes et orientations
    g->s = calloc(size, sizeof(shape));
    g->d = calloc(size, sizeof(direction));

    if (!g->s || !g->d) {
        free(g->s);
        free(g->d);
        queue_free(g->undo_stack);
        queue_free(g->redo_stack);
        free(g);
        fprintf(stderr, "Erreur : échec de l'allocation mémoire pour les formes ou orientations\n");
        exit(EXIT_FAILURE);
    }

    // Initialise la grille avec les valeurs par défaut
    for (uint i = 0; i < size; i++) {
        g->s[i] = EMPTY;
        g->d[i] = NORTH;
    }
    return g;
}

uint game_nb_rows(cgame g) {
    if (!g) {
        fprintf(stderr, "Invalid jeu\n");
        exit(EXIT_FAILURE);
    }
    return g->nb_rows;
}

uint game_nb_cols(cgame g) {
    if (!g) {
        fprintf(stderr, "Invalid jeu\n");
        exit(EXIT_FAILURE);
    }
    return g->nb_columns;
}

bool game_is_wrapping(cgame g) {
    if (!g) {
        fprintf(stderr, "Invalid jeu\n");
        exit(EXIT_FAILURE);
    }
    return (g->wrapping);
}

void game_undo(game g) {
    if (!g) {
        fprintf(stderr, "Invalid jeu\n");
        exit(EXIT_FAILURE);
    }
    if (!queue_is_empty(g->undo_stack)) {
        // Le type de data est tableau d'entiers
        int* last_move = queue_pop_tail(g->undo_stack);
        // On enfile le redo_stack
        queue_push_tail(g->redo_stack, last_move);

        int i = last_move[0];
        int j = last_move[1];
        int nb_quarter_turns = last_move[2];

        int reverse_rotations = (nb_quarter_turns % 4 + 4) % 4;
        for (int k = 0; k < reverse_rotations; k++) {
            if (g->d[i * game_nb_cols(g) + j] == NORTH) {
                g->d[i * game_nb_cols(g) + j] = WEST;
            } else {
                g->d[i * game_nb_cols(g) + j] -= 1;
            }
        }
    } else {
        printf("Pas de mouvement pour undo.\n");
    }
}

void game_redo(game g) {
    if (!g) {
        fprintf(stderr, "Invalid jeu\n");
        exit(EXIT_FAILURE);
    }
    if (!queue_is_empty(g->redo_stack)) {
        // Le type de data est tableau d'entiers
        int* last_move = queue_pop_tail(g->redo_stack);
        // On enfile le undo_stack
        queue_push_tail(g->undo_stack, last_move);

        int i = last_move[0];
        int j = last_move[1];
        int nb_quarter_turns = last_move[2];

        int rotations = (nb_quarter_turns % 4 + 4) % 4;
        for (int k = 0; k < rotations; k++) {
            if (g->d[i * game_nb_cols(g) + j] == WEST) {
                g->d[i * game_nb_cols(g) + j] = NORTH;
            } else {
                g->d[i * game_nb_cols(g) + j] += 1;
            }
        }
    } else {
        printf("Pas de mouvement pour redo.\n");
    }
}