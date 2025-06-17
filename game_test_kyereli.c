#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include "queue/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_dummy() { return EXIT_SUCCESS; }

bool test_game_default(void) {
    game g = game_default();
    bool result = (g != NULL);
    game_delete(g);
    return result;
}

bool test_game_default_solution(void) {
    game g = game_default_solution();
    bool result = (g != NULL && game_won(g));
    game_delete(g);
    return result;
}

bool test_game_get_adjacent_square(void) {
    game g = game_default();
    game g2 = game_new_empty_ext(4, 4, true);
    direction orientations[] = {EAST, EAST, EAST, EAST};
    shape shapes[] = {SEGMENT, SEGMENT, SEGMENT, CORNER};
    game g3 = game_new_ext(1, 4, shapes, orientations, true);
    uint next_i, next_j;
    uint next_i2, next_j2;
    uint next_i3, next_j3;
    uint next_i4, next_j4;
    bool result = game_get_ajacent_square(g, 0, 0, EAST, &next_i, &next_j);
    bool result2 = game_get_ajacent_square(g, 4, 4, EAST, &next_i2, &next_j2);
    bool result3 = game_get_ajacent_square(g2, 3, 3, EAST, &next_i3, &next_j3);
    bool result4 = game_get_ajacent_square(g3, 0, 3, EAST, &next_i4, &next_j4);
    game_delete(g);
    game_delete(g2);
    game_delete(g3);
    return (result && !result2 && result3 && result4 && next_i == 0 && next_j == 1 && next_i3 == 3 && next_j3 == 0 && next_i4 == 0 && next_j4 == 0);
}
bool test_game_has_half_edge(void) {
    direction orientations[] = {EAST, WEST, NORTH, WEST, SOUTH, EAST, WEST, NORTH, WEST, NORTH, EAST, EAST, EAST, WEST, SOUTH, EAST, WEST, NORTH, WEST, NORTH, EAST, EAST, NORTH, EAST, WEST};
    shape shapes[] = {ENDPOINT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT, SEGMENT, CORNER, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT, SEGMENT, TEE, SEGMENT, ENDPOINT};
    direction orientations2[] = {EAST, EAST, EAST, EAST};
    shape shapes2[] = {SEGMENT, SEGMENT, SEGMENT, CORNER};
    direction orientations3[] = {SEGMENT};
    shape shapes3[] = {CROSS};
    game g = game_new(shapes, orientations);
    game g2 = game_new_ext(1, 4, shapes2, orientations2, true);
    game g3 = game_new(shapes3, orientations3);
    bool result = (!game_has_half_edge(g, 0, 0, NORTH) && game_has_half_edge(g, 2, 1, WEST));
    bool result2 = (game_has_half_edge(g2, 0, 3, EAST) && game_has_half_edge(g2, 0, 0, WEST));
    bool result3 = (game_has_half_edge(g3, 0, 0, NORTH) && game_has_half_edge(g3, 0, 0, EAST) && game_has_half_edge(g3, 0, 0, SOUTH) && game_has_half_edge(g3, 0, 0, WEST));
    game_delete(g);
    game_delete(g2);
    game_delete(g3);
    return result && result2 && result3;
}

bool test_game_check_edge(void) {
    direction orientations[] = {EAST, WEST, NORTH, WEST, SOUTH, EAST, WEST, NORTH, WEST, NORTH, EAST, EAST, EAST, WEST, SOUTH, EAST, WEST, NORTH, WEST, NORTH, EAST, EAST, NORTH, EAST, WEST};
    shape shapes[] = {ENDPOINT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT, SEGMENT, CORNER, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT, SEGMENT, TEE, SEGMENT, ENDPOINT};
    game g = game_new(shapes, orientations);
    edge_status status = game_check_edge(g, 1, 0, EAST);
    edge_status status2 = game_check_edge(g, 0, 4, EAST);
    edge_status status3 = game_check_edge(g, 2, 1, EAST);
    direction orientations2[] = {EAST, EAST, EAST, EAST};
    shape shapes2[] = {SEGMENT, SEGMENT, SEGMENT, CORNER};
    game g2 = game_new_ext(1, 4, shapes2, orientations2, true);
    edge_status status4 = game_check_edge(g2, 0, 3, EAST);
    bool result = (status == MATCH && status2 == NOEDGE && status3 == MISMATCH && status4 == MATCH);
    game_delete(g);
    game_delete(g2);
    return result;
}

bool test_game_is_well_paired(void) {
    game g = game_default_solution();
    game g2 = game_default();
    shape shapes[] = {ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, SEGMENT, SEGMENT, SEGMENT, ENDPOINT};
    direction orientations[] = {EAST, WEST, EAST, WEST, SOUTH, EAST, WEST, EAST, WEST, NORTH, EAST, WEST, EAST, WEST, SOUTH, EAST, WEST, EAST, WEST, NORTH, EAST, EAST, EAST, EAST, WEST};
    shape shapes2[] = {TEE, SEGMENT, CORNER, SEGMENT, TEE, SEGMENT, TEE, TEE, SEGMENT, CORNER, TEE, SEGMENT, ENDPOINT, TEE, SEGMENT, CORNER, SEGMENT, TEE, CORNER, TEE, TEE, SEGMENT, CORNER, SEGMENT, ENDPOINT};
    direction orientations2[] = {EAST, EAST, SOUTH, WEST, SOUTH, SOUTH, WEST, NORTH, EAST, NORTH, EAST, NORTH, EAST, WEST, SOUTH, SOUTH, EAST, SOUTH, WEST, NORTH, SOUTH, WEST, NORTH, EAST, NORTH};
    shape shapes3[] = {ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT};
    direction orientations3[] = {WEST, EAST, WEST, EAST, WEST, EAST, WEST, EAST, WEST, EAST, WEST, EAST, WEST, EAST, WEST, EAST};
    shape shapes4[] = {SEGMENT, SEGMENT, SEGMENT, CORNER};
    direction orientations4[] = {EAST, EAST, EAST, EAST};

    game g3 = game_new_ext(5, 5, shapes, orientations, false);
    game g4 = game_new_ext(5, 5, shapes2, orientations2, false);
    game g5 = game_new_ext(4, 4, shapes3, orientations3, true);
    game g6 = game_new_ext(4, 4, shapes3, orientations3, false);
    game g7 = game_new_ext(4, 1, shapes4, orientations4, true);
    bool result = game_is_well_paired(g);
    bool result2 = !game_is_well_paired(g2);
    bool result3 = game_is_well_paired(g3);
    bool result4 = !game_is_well_paired(g4);
    bool result5 = game_is_well_paired(g5);
    bool result6 = !game_is_well_paired(g6);
    bool result7 = !game_is_well_paired(g7);
    game_delete(g);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    game_delete(g5);
    game_delete(g6);
    game_delete(g7);
    return result && result2 && result3 && result4 && result5 && result6 && result7;
}

bool test_game_is_connected(void) {
    direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {EAST, EAST, EAST, EAST, SOUTH, EAST, EAST, EAST, EAST, WEST, NORTH, EAST, EAST, EAST, SOUTH, EAST, WEST, WEST, WEST, WEST, NORTH, EAST, EAST, EAST, WEST};
    shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {ENDPOINT, SEGMENT, SEGMENT, SEGMENT, CORNER, CORNER, SEGMENT, SEGMENT, SEGMENT, CORNER, CORNER, SEGMENT, SEGMENT, SEGMENT, CORNER, CORNER, SEGMENT, SEGMENT, SEGMENT, CORNER, CORNER, SEGMENT, SEGMENT, SEGMENT, ENDPOINT};
    game g1 = game_default_solution();
    game g2 = game_default();
    game g3 = game_new_empty();
    game g4 = game_new(shapes, orientations);
    if (!g1 || !g2 || !g3) {
        return false;
    }
    bool result1 = game_is_connected(g1);
    bool result2 = !game_is_connected(g2);
    bool result3 = game_is_connected(g3);
    bool result4 = game_is_connected(g4);

    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    game_delete(g4);
    return result1 && result2 && result3 && result4;
}

bool test_game_nb_rows(void) {
    game g1 = game_new_empty();
    game g2 = game_new_empty_ext(3, 4, true);
    bool result1 = game_nb_rows(g1) == 5;
    bool result2 = game_nb_rows(g2) == 3;
    game_delete(g1);
    game_delete(g2);
    return (result1 && result2);
}

bool test_game_nb_cols(void) {
    game g1 = game_new_empty();
    game g2 = game_new_empty_ext(3, 4, true);
    bool result1 = game_nb_cols(g1) == 5;
    bool result2 = game_nb_cols(g2) == 4;
    game_delete(g1);
    game_delete(g2);
    return (result1 && result2);
}

bool test_game_is_wrapping(void) {
    game g1 = game_new_empty();
    game g2 = game_new_empty_ext(3, 4, true);
    bool result1 = game_is_wrapping(g1);
    bool result2 = game_is_wrapping(g2);
    game_delete(g1);
    game_delete(g2);
    return (!result1 && result2);
}
bool test_game_load(void) {
    game g1 = game_load("default.txt");
    game g2 = game_default();
    bool result1 = game_equal(g1, g2, true);
    game_delete(g1);
    game_delete(g2);
    return result1;
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

    if (strcmp("game_default", argv[1]) == 0)
        ok = test_game_default();
    else if (strcmp("game_default_solution", argv[1]) == 0)
        ok = test_game_default_solution();
    else if (strcmp("game_get_adjacent_square", argv[1]) == 0)
        ok = test_game_get_adjacent_square();
    else if (strcmp("game_has_half_edge", argv[1]) == 0)
        ok = test_game_has_half_edge();
    else if (strcmp("game_check_edge", argv[1]) == 0)
        ok = test_game_check_edge();
    else if (strcmp("game_is_well_paired", argv[1]) == 0)
        ok = test_game_is_well_paired();
    else if (strcmp("game_is_connected", argv[1]) == 0)
        ok = test_game_is_connected();
    else if (strcmp("game_nb_rows", argv[1]) == 0)
        ok = test_game_nb_rows();
    else if (strcmp("game_nb_cols", argv[1]) == 0)
        ok = test_game_nb_cols();
    else if (strcmp("game_is_wrapping", argv[1]) == 0)
        ok = test_game_is_wrapping();
    else if (strcmp("game_load", argv[1]) == 0)
        ok = test_game_load();
    else {
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