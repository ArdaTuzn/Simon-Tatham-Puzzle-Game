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
        fprintf(stderr, "Error!\n"); // Si le jeu est invalide
        exit(1);
    }
    // Index de colonnes
    printf("  ");
    for (uint j = 0; j < game_nb_cols(g) && j < 10; j++) {
        printf(" %u", j);
    }
    printf("\n");

    //  Bordure horizontale
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

            // Affiche la pièce selon sa forme et orientation
            if (s == EMPTY) {
                printf("  ");
            } else if (s == ENDPOINT) {
                if (d == NORTH)
                    printf("^ "); // End haut
                else if (d == EAST)
                    printf("> "); // End droite
                else if (d == SOUTH)
                    printf("v "); // End bas
                else if (d == WEST)
                    printf("< "); // End gauche
            } else if (s == SEGMENT) {
                if (d == NORTH || d == SOUTH)
                    printf("| "); // Segment vertical
                else if (d == EAST || d == WEST)
                    printf("- "); // Segment horizontal
            } else if (s == CORNER) {
                if (d == NORTH)
                    printf("└ "); // Coin haut-droite
                else if (d == EAST)
                    printf("┌ "); // Coin bas-droite
                else if (d == SOUTH)
                    printf("┐ "); // Coin bas-gauche
                else if (d == WEST)
                    printf("┘ "); // Coin haut-gauche
            } else if (s == TEE) {
                if (d == NORTH)
                    printf("┴ "); // T haut
                else if (d == EAST)
                    printf("├ "); // T droite
                else if (d == SOUTH)
                    printf("┬ "); // T bas
                else if (d == WEST)
                    printf("┤ "); // T gauche
            } else if (s == CROSS) {
                if (d == NORTH)
                    printf("+ "); // + haut
                else if (d == EAST)
                    printf("+ "); // + droite
                else if (d == SOUTH)
                    printf("+ "); // + bas
                else if (d == WEST)
                    printf("+ "); // + gauche
            }
        }
        // Bordure verticale droite
        printf("|\n");
    }

    // Bordure horizontale finale
    printf("   ");
    for (uint j = 0; j < game_nb_cols(g) && j < 10; j++) {
        printf("--");
    }
    printf("\n");
}

game game_default(void) {
    // Formes initiales
    shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE, TEE, TEE, TEE, TEE, ENDPOINT, ENDPOINT, TEE, ENDPOINT, SEGMENT, ENDPOINT, TEE, TEE, CORNER, SEGMENT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT};
    // Orientations initiales
    direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {WEST, NORTH, WEST, NORTH, SOUTH, SOUTH, WEST, NORTH, EAST, EAST, EAST, NORTH, WEST, WEST, EAST, SOUTH, SOUTH, NORTH, WEST, NORTH, EAST, WEST, SOUTH, EAST, SOUTH};
    game g = game_new(shapes, orientations);
    g->nb_columns = DEFAULT_SIZE;
    g->nb_rows = DEFAULT_SIZE;
    g->wrapping = false;
    //   g->undo_stack = queue_new();
    //   g->redo_stack = queue_new();
    return g;
}

game game_default_solution(void) {
    // Formes de la solution
    game g = game_default();
    shape shapes[] = {CORNER, ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE, TEE, TEE, TEE, TEE, ENDPOINT, ENDPOINT, TEE, ENDPOINT, SEGMENT, ENDPOINT, TEE, TEE, CORNER, SEGMENT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT};
    // Orientations de la solution
    direction orientations[] = {EAST, WEST, EAST, SOUTH, SOUTH, EAST, SOUTH, SOUTH, NORTH, WEST, NORTH, NORTH, EAST, WEST, NORTH, EAST, SOUTH, NORTH, SOUTH, SOUTH, EAST, NORTH, WEST, NORTH, NORTH};
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
    // Selon la direction, calcule les indices
    if (d == NORTH) {
        if (!game_is_wrapping(g) && i == 0) {
            return false;
        } else if (game_is_wrapping(g) && i == 0) {
            *pi_next = game_nb_rows(g) - 1;
            *pj_next = j;
        } else {
            *pi_next = i - 1;
            *pj_next = j;
        }

    } else if (d == EAST) {
        if (!game_is_wrapping(g) && j == game_nb_cols(g) - 1) {
            return false;
        } else if (game_is_wrapping(g) && j == game_nb_cols(g) - 1) {
            *pi_next = i;
            *pj_next = 0;
        } else {
            *pi_next = i;
            *pj_next = j + 1;
        }

    } else if (d == SOUTH) {
        if (!game_is_wrapping(g) && i == game_nb_rows(g) - 1) {
            return false;
        } else if (game_is_wrapping(g) && i == game_nb_rows(g) - 1) {
            *pi_next = 0;
            *pj_next = j;
        } else {
            *pi_next = i + 1;
            *pj_next = j;
        }

    } else if (d == WEST) {
        if (!game_is_wrapping(g) && j == 0) {
            return false;
        } else if (game_is_wrapping(g) && j == 0) {
            *pi_next = i;
            *pj_next = game_nb_cols(g) - 1;
        } else {
            *pi_next = i;
            *pj_next = j - 1;
        }
    } else {
        return false; // Direction invalide
    }
    return true; // Direction valide
}

bool game_has_half_edge(cgame g, uint i, uint j, direction d) {
    if (!g || i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
        return false;
    }

    shape s = game_get_piece_shape(g, i, j);
    direction dir = game_get_piece_orientation(g, i, j);
    // Vérifie selon la forme de la pièce
    if (s == EMPTY) {
        return false;
    } else if (s == ENDPOINT) {
        return (d == dir);
    } else if (s == SEGMENT) {
        return (d == NORTH && dir == NORTH) || (d == SOUTH && dir == NORTH) || (d == SOUTH && dir == SOUTH) || (d == NORTH && dir == SOUTH) || (d == EAST && dir == EAST) || (d == WEST && dir == EAST) || (d == EAST && dir == WEST) || (d == WEST && dir == WEST);
    } else if (s == CORNER) {
        return (d == dir) || (d == (dir + 1) % 4);
    } else if (s == TEE) {
        return d != ((dir + 2) % 4); // Direction opposée invalide
    } else if (s == CROSS) {
        return (dir == NORTH) || (dir == EAST) || (dir == SOUTH) || (dir == WEST);
    }
    return false;
}

edge_status game_check_edge(cgame g, uint i, uint j, direction d) {
    uint a, b;
    if (!game_get_ajacent_square(g, i, j, d, &a, &b)) {
        if (game_has_half_edge(g, i, j, d)) {
            return MISMATCH;
        } else {
            return NOEDGE;
        }
    }

    bool check_edge_1 = game_has_half_edge(g, i, j, d);
    bool check_edge_2 = game_has_half_edge(g, a, b, (d + 2) % 4);

    if (check_edge_1 && check_edge_2) {
        return MATCH; // Les deux demi-arêtes correspondent
    }
    if (check_edge_1 || check_edge_2) {
        return MISMATCH; // Une seule demi-arête présente
    }
    return NOEDGE;
}

bool game_is_well_paired(cgame g) {
    // Parcours toutes les cases de la grille
    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            for (direction d = NORTH; d <= WEST; d++) {
                if (game_has_half_edge(g, i, j, d)) {
                    if (game_check_edge(g, i, j, d) != MATCH) {
                        return false;
                    }
                }
                // Pour chaque direction
            }
        }
    }
    return true;
}

bool connectedRec(cgame g, uint si, uint sj, uint fi, uint fj, bool tab[]) {
    uint si_next;
    uint sj_next;
    tab[game_nb_cols(g) * si + sj] = true;
    if (si == fi && sj == fj) {
        return true;
    }
    if (game_check_edge(g, si, sj, NORTH) == MATCH) {
        game_get_ajacent_square(g, si, sj, NORTH, &si_next, &sj_next);
        if (!(tab[si_next * game_nb_cols(g) + sj_next])) {
            if (connectedRec(g, si_next, sj_next, fi, fj, tab)) {
                return true;
            }
        }
    }
    if (game_check_edge(g, si, sj, EAST) == MATCH) {
        game_get_ajacent_square(g, si, sj, EAST, &si_next, &sj_next);
        if (!(tab[si_next * game_nb_cols(g) + sj_next])) {
            if (connectedRec(g, si_next, sj_next, fi, fj, tab)) {
                return true;
            }
        }
    }
    if (game_check_edge(g, si, sj, SOUTH) == MATCH) {
        game_get_ajacent_square(g, si, sj, SOUTH, &si_next, &sj_next);
        if (!(tab[si_next * game_nb_cols(g) + sj_next])) {
            if (connectedRec(g, si_next, sj_next, fi, fj, tab)) {
                return true;
            }
        }
    }
    if (game_check_edge(g, si, sj, WEST) == MATCH) {
        game_get_ajacent_square(g, si, sj, WEST, &si_next, &sj_next);
        if ((!(tab[si_next * game_nb_cols(g) + sj_next]))) {
            if (connectedRec(g, si_next, sj_next, fi, fj, tab)) {
                return true;
            }
        }
    }
    return false;
}

bool game_is_connected(cgame g) {
    bool tab[game_nb_rows(g) * game_nb_cols(g)];
    uint si;
    uint sj;
    bool found = false;
    for (uint i = 0; i < game_nb_rows(g) && !found; i++) {
        for (uint j = 0; j < game_nb_cols(g) && !found; j++) {
            if (game_get_piece_shape(g, i, j) != EMPTY) {
                found = true;
                si = i;
                sj = j;
            }
        }
    }

    if (!found) {
        return true;
    }

    for (uint k = 0; k < game_nb_rows(g); k++) {
        for (uint l = 0; l < game_nb_cols(g); l++) {
            for (uint t = 0; t < game_nb_cols(g) * game_nb_rows(g); t++) {
                tab[t] = false;
            }
            if (game_get_piece_shape(g, k, l) != EMPTY) {
                if (!connectedRec(g, si, sj, k, l, tab)) {
                    return false;
                }
            }
        }
    }
    return true;
}
