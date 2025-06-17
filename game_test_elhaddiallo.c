#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include "queue/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_dummy() { return EXIT_SUCCESS; }

bool test_game_new_empty() {
    game g = game_new_empty();
    if (!g) {
        return false;
    }
    for (uint i = 0; i < DEFAULT_SIZE; i++) {
        for (uint j = 0; j < DEFAULT_SIZE; j++) {
            if (game_get_piece_shape(g, i, j) != EMPTY || game_get_piece_orientation(g, i, j) != NORTH) {
                game_delete(g);
                return false;
            }
        }
    }
    uint i_next;
    uint j_next;
    if (game_get_ajacent_square(g, DEFAULT_SIZE - 1, DEFAULT_SIZE - 1, EAST, &i_next, &j_next)) {
        game_delete(g);
        return false;
    }
    game_delete(g);
    return true;
}

bool test_game_new() {
    // Définir les formes pour chaque case de la grille (5x5)
    shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {SEGMENT, TEE, CORNER, ENDPOINT, EMPTY, TEE, SEGMENT, EMPTY, CORNER, ENDPOINT, CORNER, EMPTY, TEE, SEGMENT, EMPTY, EMPTY, ENDPOINT, CORNER, TEE, SEGMENT, ENDPOINT, EMPTY, SEGMENT, TEE, CORNER};

    // Définir les orientations pour chaque case de la grille (5x5)
    direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH};

    // Créer un jeu avec les formes et orientations définies
    game g = game_new(shapes, orientations);
    if (!g) {
        return false; // Vérifie que le jeu a bien été créé
    }

    // Vérifier que chaque forme et orientation sont correctement définies
    for (uint i = 0; i < DEFAULT_SIZE; i++) {
        for (uint j = 0; j < DEFAULT_SIZE; j++) {
            // Vérifie que la forme et l'orientation de chaque case sont correctes
            if (game_get_piece_shape(g, i, j) != shapes[i * DEFAULT_SIZE + j] || game_get_piece_orientation(g, i, j) != orientations[i * DEFAULT_SIZE + j]) {
                game_delete(g); // Libère le jeu en cas d'erreur
                return false;   // Retourne faux si une pièce ne correspond pas
            }
        }
    }

    // Libérer la mémoire allouée pour le jeu
    game_delete(g);
    return true; // Si tout est correct, retourne vrai
}

bool test_copy() {
    game g1 = game_default();
    game g3 = game_new_empty_ext(5, 3, true);
    if (g1 == NULL) {
        return false;
    }
    game g2 = game_copy(g1);
    game g4 = game_copy(g3);
    if (g2 == NULL) {
        game_delete(g1);
        return false;
    }
    bool result = game_equal(g1, g2, true);
    bool result2 = game_equal(g3, g4, true);
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return result && result2; // Retourne true si les deux jeux sont égaux
}

bool test_game_equal(void) {
    game g1 = game_default();
    game g2 = game_default_solution();
    bool result1 = game_equal(g1, g1, false);
    bool result2 = !game_equal(g1, g2, false);
    game_delete(g1);
    game_delete(g2);
    return result1 && result2;
}

bool test_game_delete(void) {
    game g = game_default();
    if (g == NULL) {
        printf("Erreur Initialisation \n");
        return false;
    }
    game_delete(g);
    return true;
}

bool test_game_set_piece_shape(void) {
    game g = game_new_empty();
    if (!g) {
        return false;
    }
    for (uint i = 0; i < DEFAULT_SIZE; i++) {
        for (uint j = 0; j < DEFAULT_SIZE; j++) {
            game_set_piece_shape(g, i, j, SEGMENT);
            if (game_get_piece_shape(g, i, j) != SEGMENT) {
                game_delete(g);
                return false;
            }
        }
    }
    game_delete(g);
    return true;
}

bool test_game_set_piece_orientation(void) {
    uint nb_rows = 3;
    uint nb_cols = 4;
    game g = game_new_empty_ext(nb_rows, nb_cols, false);
    if (!g) {
        return false;
    }
    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            game_set_piece_orientation(g, i, j, EAST);
            if (game_get_piece_orientation(g, i, j) != EAST) {
                game_delete(g);
                return false;
            }
        }
    }
    game_delete(g);
    return true;
}

bool test_game_new_ext() {
    // Paramètres du jeu à tester
    uint nb_rows = 3;
    uint nb_cols = 4;
    shape shapes[] = {EMPTY, SEGMENT, ENDPOINT, CROSS, TEE, EMPTY, CORNER, SEGMENT, EMPTY, CROSS, EMPTY, TEE};
    direction orientations[] = {NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, NORTH};

    // Création du jeu avec les paramètres
    game g = game_new_ext(nb_rows, nb_cols, shapes, orientations, true);

    // Vérifie que le jeu est créé
    if (!g) {
        fprintf(stderr, "Erreur : jeu non créé\n");
        game_delete(g);
        return false;
    }

    // Vérifie les dimensions
    if (game_nb_rows(g) != nb_rows || game_nb_cols(g) != nb_cols) {
        fprintf(stderr, "Erreur : dimensions incorrectes\n");
        game_delete(g);
        return false;
    }

    // Vérifie le wrapping
    if (!game_is_wrapping(g)) {
        fprintf(stderr, "Erreur : wrapping incorrect\n");
        game_delete(g);
        return false;
    }

    // Vérifie les formes et orientations
    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            uint index = i * nb_cols + j;
            if (game_get_piece_shape(g, i, j) != shapes[index]) {
                fprintf(stderr, "Erreur : forme incorrecte à l'indice (%u, %u)\n", i, j);
                game_delete(g);
                return false;
            }
            if (game_get_piece_orientation(g, i, j) != orientations[index]) {
                fprintf(stderr, "Erreur : orientation incorrecte à l'indice (%u, %u)\n", i, j);
                game_delete(g);
                return false;
            }
        }
    }

    // Libérer la mémoire du jeu
    game_delete(g);

    // Si tout est correct
    return true;
}

bool test_game_new_empty_ext() {
    // Test avec des dimensions valides et wrapping activé
    uint nb_rows = 3;
    uint nb_cols = 4;
    bool wrapping = true;

    game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);

    // Vérifie que le jeu est créé
    if (!g) {
        fprintf(stderr, "Erreur : jeu non créé\n");
        return false;
    }

    // Vérifie les dimensions
    if (game_nb_rows(g) != nb_rows || game_nb_cols(g) != nb_cols) {
        fprintf(stderr, "Erreur : dimensions incorrectes\n");
        game_delete(g);
        return false;
    }

    // Vérifie le wrapping
    if (!game_is_wrapping(g)) {
        fprintf(stderr, "Erreur : wrapping incorrect\n");
        game_delete(g);
        return false;
    }

    // Vérifie que toutes les cases sont vides
    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            if (game_get_piece_shape(g, i, j) != EMPTY) {
                fprintf(stderr, "Erreur : case non vide à (%u, %u)\n", i, j);
                game_delete(g);
                return false;
            }
            if (game_get_piece_orientation(g, i, j) != NORTH) {
                fprintf(stderr, "Erreur : orientation incorrecte à (%u, %u)\n", i, j);
                game_delete(g);
                return false;
            }
        }
    }

    // Libère la mémoire
    game_delete(g);
    return true;
}

/* bool test_game_save(void) {
   game g = game_default();
   if (g == NULL) {
       game_delete(g);
       return false ;
   }
   const char *filename = "default.sol" ;
   game_save(g, (char *)filename) ;
   FILE * file = fopen(filename, "w");
   if (file == NULL){
       fprintf(stderr, "Erreur : Le fichier de sauvegarde n'a pas été créé\n");
       game_delete(g);
       remove(filename);
       return false;
   }
   fclose(file);
   remove(filename);
   game_delete(g);
   return true;
} */

bool test_game_save(void) {
    const char* filename = "test_game_save.txt";
    game g1 = game_default(); // Use the default game
    if (!g1) {
        fprintf(stderr, "Erreur: Impossible de créer le jeu par défaut.\n");
        return false;
    }

    game_save(g1, (char*)filename); // Save the game

    // Load the saved game
    game g2 = game_load((char*)filename);
    if (!g2) {
        perror("Erreur: Impossible de charger le jeu");
        game_delete(g1);
        remove(filename); // Clean up the file if we can't open it
        return false;
    }

    // Check game equals
    bool result = game_equal(g1, g2, false);
    if (!result) {
        fprintf(stderr, "Erreur: Les jeux ne sont pas égaux.\n");
    }

    game_delete(g1);
    game_delete(g2);
    remove(filename); // Clean up the temporary file

    return result; // All tests passed
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <test_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "dummy") == 0) {
        return test_dummy();
    } else if (strcmp(argv[1], "game_new_empty") == 0) {
        if (test_game_new_empty()) {
            printf("Test game_new_empty: OK\n");
            return EXIT_SUCCESS;
        } else {
            printf("Test game_new_empty: Erreur\n");
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "game_new") == 0) {
        if (test_game_new()) {
            printf("Test game_new: OK\n");
            return EXIT_SUCCESS;
        } else {
            printf("Test game_new: Erreur\n");
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "game_copy") == 0) {
        if (test_copy()) {
            return EXIT_SUCCESS;
        } else {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "game_equal") == 0) {
        if (test_game_equal()) {
            return EXIT_SUCCESS;
        } else {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "game_delete") == 0) {
        if (test_game_delete()) {
            return EXIT_SUCCESS;
        } else {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "game_set_piece_shape") == 0) {
        if (test_game_set_piece_shape()) {
            return EXIT_SUCCESS;
        } else {
            return EXIT_FAILURE;
        }
    }
    if (strcmp(argv[1], "game_set_piece_orientation") == 0) {
        if (test_game_set_piece_orientation()) {
            return EXIT_SUCCESS;
        } else {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "game_new_ext") == 0) {
        if (test_game_new_ext()) {
            return EXIT_SUCCESS;
        } else {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "game_new_empty_ext") == 0) {
        if (test_game_new_empty_ext()) {
            return EXIT_SUCCESS;
        } else {
            return EXIT_FAILURE;
        }
    } else if (strcmp(argv[1], "game_save") == 0) {
        if (test_game_save()) {
            printf("Test game_save: OK\n");
            return EXIT_SUCCESS;
        } else {
            printf("Test game_save: Erreur\n");
            return EXIT_FAILURE;
        }

    } else {
        fprintf(stderr, "Invalid argument: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
}
