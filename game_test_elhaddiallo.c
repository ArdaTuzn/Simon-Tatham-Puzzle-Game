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
    // Define the shapes for each square of the grid (5x5)
    shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {SEGMENT, TEE, CORNER, ENDPOINT, EMPTY, TEE, SEGMENT, EMPTY, CORNER, ENDPOINT, CORNER, EMPTY, TEE, SEGMENT, EMPTY, EMPTY, ENDPOINT, CORNER, TEE, SEGMENT, ENDPOINT, EMPTY, SEGMENT, TEE, CORNER};

    // Define the orientations for each square of the grid (5x5)
    direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH};

    // Create a game with the defined shapes and orientations
    game g = game_new(shapes, orientations);
    if (!g) {
        return false; // Check that the game was created
    }

    // Check that each shape and orientation is correctly defined
    for (uint i = 0; i < DEFAULT_SIZE; i++) {
        for (uint j = 0; j < DEFAULT_SIZE; j++) {
            if (game_get_piece_shape(g, i, j) != shapes[i * DEFAULT_SIZE + j] || game_get_piece_orientation(g, i, j) != orientations[i * DEFAULT_SIZE + j]) {
                game_delete(g); // Free memory on error
                return false;
            }
        }
    }

    game_delete(g); // Free memory
    return true;
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
    return result && result2; // Return true if both games are equal
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
        printf("Error Initializing\n");
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
    // Parameters for the game
    uint nb_rows = 3;
    uint nb_cols = 4;
    shape shapes[] = {EMPTY, SEGMENT, ENDPOINT, CROSS, TEE, EMPTY, CORNER, SEGMENT, EMPTY, CROSS, EMPTY, TEE};
    direction orientations[] = {NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, WEST, NORTH, EAST, SOUTH, NORTH};

    // Create the game with parameters
    game g = game_new_ext(nb_rows, nb_cols, shapes, orientations, true);

    if (!g) {
        fprintf(stderr, "Error: game not created\n");
        return false;
    }

    // Check dimensions
    if (game_nb_rows(g) != nb_rows || game_nb_cols(g) != nb_cols) {
        fprintf(stderr, "Error: incorrect dimensions\n");
        game_delete(g);
        return false;
    }

    // Check wrapping
    if (!game_is_wrapping(g)) {
        fprintf(stderr, "Error: incorrect wrapping\n");
        game_delete(g);
        return false;
    }

    // Check shapes and orientations
    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            uint index = i * nb_cols + j;
            if (game_get_piece_shape(g, i, j) != shapes[index]) {
                fprintf(stderr, "Error: incorrect shape at index (%u, %u)\n", i, j);
                game_delete(g);
                return false;
            }
            if (game_get_piece_orientation(g, i, j) != orientations[index]) {
                fprintf(stderr, "Error: incorrect orientation at index (%u, %u)\n", i, j);
                game_delete(g);
                return false;
            }
        }
    }

    game_delete(g);
    return true;
}

bool test_game_new_empty_ext() {
    // Test with valid dimensions and wrapping enabled
    uint nb_rows = 3;
    uint nb_cols = 4;
    bool wrapping = true;

    game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);

    if (!g) {
        fprintf(stderr, "Error: game not created\n");
        return false;
    }

    if (game_nb_rows(g) != nb_rows || game_nb_cols(g) != nb_cols) {
        fprintf(stderr, "Error: incorrect dimensions\n");
        game_delete(g);
        return false;
    }

    if (!game_is_wrapping(g)) {
        fprintf(stderr, "Error: incorrect wrapping\n");
        game_delete(g);
        return false;
    }

    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            if (game_get_piece_shape(g, i, j) != EMPTY) {
                fprintf(stderr, "Error: cell not empty at (%u, %u)\n", i, j);
                game_delete(g);
                return false;
            }
            if (game_get_piece_orientation(g, i, j) != NORTH) {
                fprintf(stderr, "Error: incorrect orientation at (%u, %u)\n", i, j);
                game_delete(g);
                return false;
            }
        }
    }

    game_delete(g);
    return true;
}

bool test_game_save(void) {
    const char* filename = "test_game_save.txt";
    game g1 = game_default(); // Use the default game
    if (!g1) {
        fprintf(stderr, "Error: Unable to create default game.\n");
        return false;
    }

    game_save(g1, (char*)filename); // Save the game

    game g2 = game_load((char*)filename);
    if (!g2) {
        perror("Error: Unable to load game");
        game_delete(g1);
        remove(filename); // Clean up
        return false;
    }

    bool result = game_equal(g1, g2, false);
    if (!result) {
        fprintf(stderr, "Error: Games are not equal.\n");
    }

    game_delete(g1);
    game_delete(g2);
    remove(filename);

    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <test_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "dummy") == 0) return test_dummy();
    else if (strcmp(argv[1], "game_new_empty") == 0) return test_game_new_empty() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_new") == 0) return test_game_new() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_copy") == 0) return test_copy() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_equal") == 0) return test_game_equal() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_delete") == 0) return test_game_delete() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_set_piece_shape") == 0) return test_game_set_piece_shape() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_set_piece_orientation") == 0) return test_game_set_piece_orientation() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_new_ext") == 0) return test_game_new_ext() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_new_empty_ext") == 0) return test_game_new_empty_ext() ? EXIT_SUCCESS : EXIT_FAILURE;
    else if (strcmp(argv[1], "game_save") == 0) return test_game_save() ? EXIT_SUCCESS : EXIT_FAILURE;
    else {
        fprintf(stderr, "Invalid argument: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
}
