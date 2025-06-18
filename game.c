#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Creates an empty game with default values.
 * Returns a pointer to the game structure, or NULL in case of error.
 */
game game_new_empty(void) {
    // allocate memory for the game structure
    game g = malloc(sizeof(game_s));
    if (!g) {
        fprintf(stderr, "Error creating the game\n");
        return NULL;
    }

    // total size of the grid
    uint size = DEFAULT_SIZE * DEFAULT_SIZE;
    g->nb_columns = DEFAULT_SIZE;
    g->nb_rows = DEFAULT_SIZE;
    g->wrapping = false;
    g->undo_stack = queue_new();
    g->redo_stack = queue_new();

    // allocate and initialize the shape and direction arrays
    g->s = calloc(size, sizeof(shape));
    g->d = calloc(size, sizeof(direction));
    // check allocations
    if (!g->s || !g->d || !g->undo_stack || !g->redo_stack) {
        game_delete(g);
        return NULL;
    }
    return g; // return the created game
}

/**
 * Creates a new game based on provided shapes and orientations.
 * If arrays are NULL, the cells are initialized with default values.
 */
game game_new(shape* shapes, direction* orientations) {
    // Create an empty game using game_new_empty
    game g = game_new_empty();
    if (!g) {
        fprintf(stderr, "Error creating the game\n");
        exit(1);
    }

    // Calculate the grid size
    uint size = DEFAULT_SIZE * DEFAULT_SIZE;
    // Initialize shapes and orientations for each cell
    for (uint i = 0; i < size; i++) {
        g->s[i] = (shapes) ? shapes[i] : EMPTY;
        g->d[i] = (orientations) ? orientations[i] : NORTH;
    }
    return g;
}

/**
 * Copies an existing game.
 * Returns a new identical game instance or NULL in case of error.
 */
game game_copy(cgame g) {
    if (g == NULL) {
        return NULL;
    }
    uint size = game_nb_rows(g) * game_nb_cols(g);
    game new_game = game_new_empty();
    if (new_game == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }
    // Copy properties and arrays
    memcpy(new_game->s, g->s, size * sizeof(shape));
    memcpy(new_game->d, g->d, size * sizeof(direction));
    new_game->nb_rows = game_nb_rows(g);
    new_game->nb_columns = game_nb_cols(g);
    new_game->wrapping = game_is_wrapping(g);
    return new_game;
}

/**
 * Compares two games to check if they are identical.
 * If ignore_orientation is true, only shapes are compared.
 */
bool game_equal(cgame g1, cgame g2, bool ignore_orientation) {
    // Check if both games are valid
    if (g1 == NULL || g2 == NULL) {
        return false;
    }

    // Get game dimensions
    uint rows1 = game_nb_rows(g1);
    uint cols1 = game_nb_cols(g1);
    uint rows2 = game_nb_rows(g2);
    uint cols2 = game_nb_cols(g2);

    // Check if dimensions match
    if (rows1 != rows2 || cols1 != cols2) {
        return false;
    }

    // Calculate total size
    uint size = rows1 * cols1;

    // Compare shapes with memcmp
    if (memcmp(g1->s, g2->s, size * sizeof(shape)) != 0) {
        return false;
    }

    // If orientation is not ignored, compare orientations
    if (!ignore_orientation) {
        if (memcmp(g1->d, g2->d, size * sizeof(direction)) != 0) {
            return false;
        }
    }

    // Compare wrapping option
    if (game_is_wrapping(g1) != game_is_wrapping(g2)) {
        return false;
    }

    return true; // Games are identical
}

/**
 * Frees resources associated with a game.
 */
void game_delete(game g) {
    if (g == NULL) {
        return;
    }
    free(g->s);
    free(g->d);
    queue_free_full(g->undo_stack, free);
    queue_free_full(g->redo_stack, free);
    free(g);
}

/**
 * Sets the shape of a piece in the game.
 */
void game_set_piece_shape(game g, uint i, uint j, shape s) {
    if (!g || i >= g->nb_rows || j >= g->nb_columns) {
        fprintf(stderr, "Error in parameters or invalid indices.\n");
        exit(EXIT_FAILURE);
    }
    // Check if shape is valid
    if (s != EMPTY && s != ENDPOINT && s != SEGMENT && s != CORNER && s != TEE && s != CROSS) {
        fprintf(stderr, "Invalid shape\n");
        exit(1);
    }
    g->s[i * game_nb_cols(g) + j] = s;
}

/**
 * Sets the orientation of a piece in the game.
 */
void game_set_piece_orientation(game g, uint i, uint j, direction o) {
    if (!g || i >= g->nb_rows || j >= g->nb_columns) {
        fprintf(stderr, "Error in parameters or invalid indices.\n");
        exit(EXIT_FAILURE);
    }
    // Check if direction is valid
    if (o != NORTH && o != EAST && o != SOUTH && o != WEST) {
        fprintf(stderr, "Invalid orientation\n");
    }
    g->d[i * game_nb_cols(g) + j] = o;
}

/**
 * Retrieves the shape of a piece.
 */
shape game_get_piece_shape(cgame g, uint i, uint j) {
    if (!g || i >= g->nb_rows || j >= g->nb_columns) {
        fprintf(stderr, "Invalid indices or game.\n");
        exit(EXIT_FAILURE);
    }
    return g->s[i * game_nb_cols(g) + j];
}

/**
 * Retrieves the orientation of a piece.
 */
direction game_get_piece_orientation(cgame g, uint i, uint j) {
    if (!g) {
        fprintf(stderr, "Error in parameters\n");
        exit(1);
    }
    if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
        fprintf(stderr, "Error in indices\n");
        exit(1);
    }
    return g->d[i * game_nb_cols(g) + j];
}

/**
 * Plays a move by rotating a piece a given number of quarter turns.
 */
void game_play_move(game g, uint i, uint j, int nb_quarter_turns) {
    if (!g) {
        fprintf(stderr, "Error in parameters\n");
        exit(1);
    }
    if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
        fprintf(stderr, "Error in indices\n");
        exit(1);
    }
    int* move = malloc(3 * sizeof(int));
    if (!move) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    move[0] = i;
    move[1] = j;
    move[2] = nb_quarter_turns;
    queue_push_tail(g->undo_stack, move);
    while (!queue_is_empty(g->redo_stack)) {
        free(queue_pop_head(g->redo_stack));
    }
    int move2 = (nb_quarter_turns % 4 + 4) % 4;
    for (uint k = 0; k < move2; k++) {
        if (g->d[i * game_nb_cols(g) + j] == WEST) {
            g->d[i * game_nb_cols(g) + j] = NORTH;
        } else {
            g->d[i * game_nb_cols(g) + j] += 1;
        }
    }
}

/**
 * Checks whether the game is won.
 */
bool game_won(cgame g) {
    if (!g) {
        fprintf(stderr, "Error: invalid game pointer.\n");
        exit(EXIT_FAILURE);
    }
    return game_is_well_paired(g) && game_is_connected(g);
}

/**
 * Resets the orientation of all pieces in the game to NORTH.
 */
void game_reset_orientation(game g) {
    if (!g && !g->d) {
        fprintf(stderr, "Error in parameters\n");
        exit(1);
    }
    int size = game_nb_rows(g) * game_nb_cols(g);
    for (uint i = 0; i < size; i++) {
        g->d[i] = NORTH;
    }
}

/**
 * Randomizes the orientation of all pieces in the game.
 */
void game_shuffle_orientation(game g) {
    if (!g) {
        fprintf(stderr, "Error in parameters\n");
        exit(1);
    }
    int size = game_nb_rows(g) * game_nb_cols(g);
    for (uint i = 0; i < size; i++) {
        int randomNumber = rand() % 4;
        g->d[i] = randomNumber;
    }
}