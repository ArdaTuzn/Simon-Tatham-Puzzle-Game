#include "game_aux.h"
#include "game.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void game_print(cgame g) {
    if (!g) {
        fprintf(stderr, "Error!\n"); // If the game is invalid
        exit(1);
    }
    // Column indices
    printf("  ");
    for (uint j = 0; j < game_nb_cols(g) && j < 10; j++) {
        printf(" %u", j);
    }
    printf("\n");

    // Horizontal border
    printf("   ");
    for (uint j = 0; j < game_nb_cols(g) && j < 10; j++) {
        printf("--");
    }
    printf("\n");

    for (uint i = 0; i < game_nb_rows(g) && i < 10; i++) {
        printf("%u |", i);
        for (uint j = 0; j < game_nb_cols(g) && j < 10; j++) {
            shape s = game_get_piece_shape(g, i, j);
            direction d = game_get_piece_orientation(g, i, j);

            // Display the piece according to its shape and orientation
            if (s == EMPTY) {
                printf("  ");
            } else if (s == ENDPOINT) {
                if (d == NORTH)
                    printf("^ "); // End top
                else if (d == EAST)
                    printf("> "); // End right
                else if (d == SOUTH)
                    printf("v "); // End bottom
                else if (d == WEST)
                    printf("< "); // End left
            } else if (s == SEGMENT) {
                if (d == NORTH || d == SOUTH)
                    printf("| "); // Vertical segment
                else if (d == EAST || d == WEST)
                    printf("- "); // Horizontal segment
            } else if (s == CORNER) {
                if (d == NORTH)
                    printf("└ "); // Top-right corner
                else if (d == EAST)
                    printf("┌ "); // Bottom-right corner
                else if (d == SOUTH)
                    printf("┐ "); // Bottom-left corner
                else if (d == WEST)
                    printf("┘ "); // Top-left corner
            } else if (s == TEE) {
                if (d == NORTH)
                    printf("┴ "); // T top
                else if (d == EAST)
                    printf("├ "); // T right
                else if (d == SOUTH)
                    printf("┬ "); // T bottom
                else if (d == WEST)
                    printf("┤ "); // T left
            } else if (s == CROSS) {
                printf("+ "); // Cross shape in any direction
            }
        }
        // Right vertical border
        printf("|\n");
    }

    // Final horizontal border
    printf("   ");
    for (uint j = 0; j < game_nb_cols(g) && j < 10; j++) {
        printf("--");
    }
    printf("\n");
}

game game_default(void) {
    // Initial shapes
    shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {
        CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT,
        TEE, TEE, TEE, TEE, TEE,
        ENDPOINT, ENDPOINT, TEE, ENDPOINT, SEGMENT,
        ENDPOINT, TEE, TEE, CORNER, SEGMENT,
        ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT
    };

    // Initial orientations
    direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {
        WEST, NORTH, WEST, NORTH, SOUTH,
        SOUTH, WEST, NORTH, EAST, EAST,
        EAST, NORTH, WEST, WEST, EAST,
        SOUTH, SOUTH, NORTH, WEST, NORTH,
        EAST, WEST, SOUTH, EAST, SOUTH
    };

    game g = game_new(shapes, orientations);
    g->nb_columns = DEFAULT_SIZE;
    g->nb_rows = DEFAULT_SIZE;
    g->wrapping = false;
    // g->undo_stack = queue_new();
    // g->redo_stack = queue_new();
    return g;
}

game game_default_solution(void) {
    // Shapes for the solution
    game g = game_default();
    shape shapes[] = {
        CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT,
        TEE, TEE, TEE, TEE, TEE,
        ENDPOINT, ENDPOINT, TEE, ENDPOINT, SEGMENT,
        ENDPOINT, TEE, TEE, CORNER, SEGMENT,
        ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT
    };

    // Orientations for the solution
    direction orientations[] = {
        EAST, WEST, EAST, SOUTH, SOUTH,
        EAST, SOUTH, SOUTH, NORTH, WEST,
        NORTH, NORTH, EAST, WEST, NORTH,
        EAST, SOUTH, NORTH, SOUTH, SOUTH,
        EAST, NORTH, WEST, NORTH, NORTH
    };

    for (int i = 0; i < game_nb_rows(g); i++) {
        for (int j = 0; j < game_nb_cols(g); j++) {
            game_set_piece_orientation(g, i, j, orientations[i * game_nb_cols(g) + j]);
            game_set_piece_shape(g, i, j, shapes[i * game_nb_cols(g) + j]);
        }
    }
    return g;
}

bool game_get_ajacent_square(cgame g, uint i, uint j, direction d, uint* pi_next, uint* pj_next) {
    if (!g || i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
        return false;
    }

    // Compute indices based on direction
    if (d == NORTH) {
        if (!game_is_wrapping(g) && i == 0) return false;
        *pi_next = (i == 0 ? game_nb_rows(g) - 1 : i - 1);
        *pj_next = j;
    } else if (d == EAST) {
        if (!game_is_wrapping(g) && j == game_nb_cols(g) - 1) return false;
        *pi_next = i;
        *pj_next = (j == game_nb_cols(g) - 1 ? 0 : j + 1);
    } else if (d == SOUTH) {
        if (!game_is_wrapping(g) && i == game_nb_rows(g) - 1) return false;
        *pi_next = (i == game_nb_rows(g) - 1 ? 0 : i + 1);
        *pj_next = j;
    } else if (d == WEST) {
        if (!game_is_wrapping(g) && j == 0) return false;
        *pi_next = i;
        *pj_next = (j == 0 ? game_nb_cols(g) - 1 : j - 1);
    } else {
        return false; // Invalid direction
    }
    return true;
}

bool game_has_half_edge(cgame g, uint i, uint j, direction d) {
    if (!g || i >= game_nb_rows(g) || j >= game_nb_cols(g)) return false;

    shape s = game_get_piece_shape(g, i, j);
    direction dir = game_get_piece_orientation(g, i, j);

    // Check based on the shape of the piece
    if (s == EMPTY) return false;
    if (s == ENDPOINT) return (d == dir);
    if (s == SEGMENT)
        return (d == NORTH || d == SOUTH) ? (dir == NORTH || dir == SOUTH)
                                          : (dir == EAST || dir == WEST);
    if (s == CORNER)
        return (d == dir) || (d == (dir + 1) % 4);
    if (s == TEE)
        return d != ((dir + 2) % 4); // Opposite direction is invalid
    if (s == CROSS)
        return true;

    return false;
}

edge_status game_check_edge(cgame g, uint i, uint j, direction d) {
    uint a, b;
    if (!game_get_ajacent_square(g, i, j, d, &a, &b)) {
        return game_has_half_edge(g, i, j, d) ? MISMATCH : NOEDGE;
    }

    bool e1 = game_has_half_edge(g, i, j, d);
    bool e2 = game_has_half_edge(g, a, b, (d + 2) % 4);

    if (e1 && e2) return MATCH;
    if (e1 || e2) return MISMATCH;
    return NOEDGE;
}

bool game_is_well_paired(cgame g) {
    // Check all squares in the grid
    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            for (direction d = NORTH; d <= WEST; d++) {
                if (game_has_half_edge(g, i, j, d)) {
                    if (game_check_edge(g, i, j, d) != MATCH) return false;
                }
            }
        }
    }
    return true;
}

bool connectedRec(cgame g, uint si, uint sj, uint fi, uint fj, bool tab[]) {
    uint si_next, sj_next;
    tab[game_nb_cols(g) * si + sj] = true;
    if (si == fi && sj == fj) return true;

    for (direction d = NORTH; d <= WEST; d++) {
        if (game_check_edge(g, si, sj, d) == MATCH) {
            game_get_ajacent_square(g, si, sj, d, &si_next, &sj_next);
            if (!tab[si_next * game_nb_cols(g) + sj_next]) {
                if (connectedRec(g, si_next, sj_next, fi, fj, tab)) return true;
            }
        }
    }
    return false;
}

bool game_is_connected(cgame g) {
    bool tab[game_nb_rows(g) * game_nb_cols(g)];
    uint si = 0, sj = 0;
    bool found = false;

    // Find the first non-empty piece
    for (uint i = 0; i < game_nb_rows(g) && !found; i++) {
        for (uint j = 0; j < game_nb_cols(g) && !found; j++) {
            if (game_get_piece_shape(g, i, j) != EMPTY) {
                si = i;
                sj = j;
                found = true;
            }
        }
    }

    if (!found) return true; // Empty board is trivially connected

    for (uint k = 0; k < game_nb_rows(g); k++) {
        for (uint l = 0; l < game_nb_cols(g); l++) {
            if (game_get_piece_shape(g, k, l) != EMPTY) {
                memset(tab, 0, sizeof(tab));
                if (!connectedRec(g, si, sj, k, l, tab)) return false;
            }
        }
    }
    return true;
}
