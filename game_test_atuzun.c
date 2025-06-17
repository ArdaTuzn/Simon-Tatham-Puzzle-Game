#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_dummy(void) { return EXIT_SUCCESS; }

bool test_game_get_piece_shape() {
    shape shapes[] = {SEGMENT, TEE, CORNER, ENDPOINT, EMPTY, TEE, SEGMENT, EMPTY, CORNER, ENDPOINT, CORNER, EMPTY, TEE, SEGMENT, EMPTY, EMPTY, ENDPOINT, CORNER, TEE, SEGMENT, ENDPOINT, EMPTY, SEGMENT, TEE, CORNER};
    game g = game_new(shapes, NULL);
    if (!g) {
        return false;
    }
    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            if (game_get_piece_shape(g, i, j) != shapes[i * game_nb_cols(g) + j]) {
                game_delete(g);
                return false;
            }
        }
    }
    game_delete(g);
    return true;
}

bool test_game_get_piece_orientation(void) {
    // Jeu avec des orientations spécifiques
    direction orientations[] = {EAST, WEST, NORTH, EMPTY, SOUTH, EMPTY, EAST, NORTH, WEST, SOUTH, WEST, EAST, SOUTH, EMPTY, NORTH, NORTH, EAST, WEST, SOUTH, EMPTY};
    uint nb_rows = 4;
    uint nb_cols = 5;

    // Créer un jeu
    game g = game_new_ext(nb_rows, nb_cols, NULL, orientations, false);
    if (!g) {
        fprintf(stderr, "Erreur : échec de la création du jeu\n");
        return false;
    }

    // Vérifier chaque orientation
    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            direction expected = orientations[i * nb_cols + j];
            direction actual = game_get_piece_orientation(g, i, j);
            if (actual != expected) {
                fprintf(stderr,
                        "Erreur : orientation incorrecte à (%u, %u). "
                        "Attendu: %d, Obtenu: %d\n",
                        i, j, expected, actual);
                game_delete(g);
                return false;
            }
        }
    }

    // Supprimer le jeu
    game_delete(g);
    return true;
}

bool test_game_play_move(void) {
    direction orientations[] = {EAST, WEST, EAST, WEST, SOUTH, EAST, WEST, EAST, WEST, NORTH, EAST, WEST, EAST, WEST, SOUTH, EAST, WEST, EAST, WEST, NORTH, EAST, EAST, EAST, EAST, WEST};
    shape shapes[] = {ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, SEGMENT, SEGMENT, SEGMENT, ENDPOINT};
    game g = game_new(shapes, orientations);
    if (!g) {
        return false;
    }
    game_play_move(g, 0, 0, 1);
    if (game_get_piece_orientation(g, 0, 0) != SOUTH) {
        game_delete(g);
        return false;
    }
    game_play_move(g, 0, 0, -1);
    if (game_get_piece_orientation(g, 0, 0) != EAST) {
        game_delete(g);
        return false;
    }
    game_play_move(g, 0, 0, 3);
    if (game_get_piece_orientation(g, 0, 0) != NORTH) {
        game_delete(g);
        return false;
    }
    game_play_move(g, 0, 0, 4);
    if (game_get_piece_orientation(g, 0, 0) != NORTH) {
        game_delete(g);
        return false;
    }
    game_play_move(g, 0, 0, 5);
    if (game_get_piece_orientation(g, 0, 0) != EAST) {
        game_delete(g);
        return false;
    }
    game_delete(g);
    return true;
}

bool test_game_won(void) {
    game g1 = game_default_solution();
    game g2 = game_default();
    game g3 = game_new_empty();
    game g4 = game_new(NULL, NULL);

    direction orientations[] = {EAST, WEST, EAST, WEST, SOUTH, EAST, WEST, EAST, WEST, NORTH, EAST, WEST, EAST, WEST, SOUTH, EAST, WEST, EAST, WEST, NORTH, EAST, EAST, EAST, EAST, WEST};
    shape shapes[] = {ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, SEGMENT, SEGMENT, SEGMENT, ENDPOINT};
    direction orientations2[] = {EAST, WEST};
    shape shapes2[] = {ENDPOINT, ENDPOINT};
    game g5 = game_new(shapes, orientations);
    game g6 = game_new_empty_ext(5, 3, true);
    game g7 = game_new_ext(1, 2, shapes2, orientations2, true);
    if (!g1 || !g2 || !g3 || !g4 || !g5 || !g6 || !g7) {
        return false;
    }

    if (!game_won(g1)) {
        game_delete(g1);
        game_delete(g2);
        game_delete(g3);
        game_delete(g4);
        game_delete(g5);
        game_delete(g6);
        game_delete(g7);
        return false;
    }
    if (game_won(g2)) {
        game_delete(g1);
        game_delete(g2);
        game_delete(g3);
        game_delete(g4);
        game_delete(g5);
        game_delete(g6);
        game_delete(g7);
        return false;
    }
    if (!game_won(g3)) {
        game_delete(g1);
        game_delete(g2);
        game_delete(g3);
        game_delete(g4);
        game_delete(g5);
        game_delete(g6);
        game_delete(g7);
        return false;
    }
    if (!game_won(g4)) {
        game_delete(g1);
        game_delete(g2);
        game_delete(g3);
        game_delete(g4);
        game_delete(g5);
        game_delete(g6);
        game_delete(g7);
        return false;
    }
    if (game_won(g5)) {
        game_delete(g1);
        game_delete(g2);
        game_delete(g3);
        game_delete(g4);
        game_delete(g5);
        game_delete(g6);
        game_delete(g7);
        return false;
    }
    if (!game_won(g6)) {
        game_delete(g1);
        game_delete(g2);
        game_delete(g3);
        game_delete(g4);
        game_delete(g5);
        game_delete(g6);
        game_delete(g7);
        return false;
    }
    if (!game_won(g7)) {
        game_delete(g1);
        game_delete(g2);
        game_delete(g3);
        game_delete(g4);
        game_delete(g5);
        game_delete(g6);
        game_delete(g7);
        return false;
    }
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(g7);
    return true;
}

bool test_game_reset_orientation(void) {
    direction orientations[] = {SOUTH, EAST, SOUTH, SOUTH, SOUTH, SOUTH, EAST, SOUTH, SOUTH, SOUTH, SOUTH, EAST, WEST, SOUTH, EAST, SOUTH, EAST, WEST, SOUTH, SOUTH};
    game g = game_new(NULL, orientations);
    if (!g) {
        return false;
    }
    game_reset_orientation(g);
    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            if (game_get_piece_orientation(g, i, j) != NORTH) {
                game_delete(g);
                return false;
            }
        }
    }
    game_delete(g);
    return true;
}

bool test_game_shuffle_orientation(void) {
    direction orientations[] = {EAST, WEST, NORTH, EMPTY, SOUTH, EMPTY, EAST, NORTH, WEST, SOUTH, WEST, EAST, SOUTH, EMPTY, NORTH, NORTH, EAST, WEST, SOUTH, EMPTY};
    game g = game_new(NULL, orientations);
    if (!g) {
        return false;
    }
    for (int attempt = 0; attempt < 3; attempt++) {
        game_shuffle_orientation(g);
        for (uint i = 0; i < game_nb_rows(g); i++) {
            for (uint j = 0; j < DEFAULT_SIZE; j++) {
                if (game_get_piece_orientation(g, i, j) != orientations[i * game_nb_cols(g) + j]) {
                    game_delete(g);
                    return true;
                }
            }
        }
    }
    game_delete(g);
    return false;
}

bool test_game_print(void) {
    game g = game_default();
    if (!g) {
        return false;
    }
    game_print(g);
    game_delete(g);
    return true;
}

bool test_game_undo(void) {
    game g = game_default();
    game g_default = game_default();
    game_play_move(g, 0, 0, 2);
    game_undo(g);
    game_undo(g);
    bool result = game_equal(g, g_default, false);
    game_delete(g);
    game_delete(g_default);
    return result;
}

bool test_game_redo(void) {
    game g = game_default();
    game g_default = game_default();
    game_play_move(g, 0, 0, 2);
    game_undo(g);
    game_redo(g);
    game_redo(g);
    // On vérifie si la fontion ne fait rien si aucun game_undo n'a pas été fait
    bool result1 = game_get_piece_orientation(g, 0, 0) == EAST;
    game_undo(g);
    game_play_move(g, 1, 1, 4);
    // Cette game_redo ne peut pas 'redo' (0,0) annulé par la fonction game_undo car on a joué un nouveau mouveent
    // avec game_play_move
    game_redo(g);
    bool result2 = game_equal(g, g_default, false);
    game_delete(g);
    game_delete(g_default);
    return result1 && result2;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "dummy") == 0) {
        return test_dummy();
    }

    fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
    bool ok = false;

    if (strcmp("game_get_piece_shape", argv[1]) == 0) {
        ok = test_game_get_piece_shape();
    } else if (strcmp("game_get_piece_orientation", argv[1]) == 0) {
        ok = test_game_get_piece_orientation();
    } else if (strcmp("game_play_move", argv[1]) == 0) {
        ok = test_game_play_move();
    } else if (strcmp("game_won", argv[1]) == 0) {
        ok = test_game_won();
    } else if (strcmp("game_reset_orientation", argv[1]) == 0) {
        ok = test_game_reset_orientation();
    } else if (strcmp("game_shuffle_orientation", argv[1]) == 0) {
        ok = test_game_shuffle_orientation();
    } else if (strcmp("game_print", argv[1]) == 0) {
        ok = test_game_print();
    } else if (strcmp("game_undo", argv[1]) == 0) {
        ok = test_game_undo();
    } else if (strcmp("game_redo", argv[1]) == 0) {
        ok = test_game_redo();
    } else {
        fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (ok) {
        fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
        return EXIT_FAILURE;
    }
}
