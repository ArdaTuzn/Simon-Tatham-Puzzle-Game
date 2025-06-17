#include "game_tools.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue/queue.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)

// @copyright University of Bordeaux. All rights reserved, 2024.

/* ************************************************************************** */

/** @brief Hard-coding of pieces (shape & orientation) in an integer array.
 * @details The 4 least significant bits encode the presence of an half-edge in
 * the N-E-S-W directions (in that order). Thus, binary coding 1100 represents
 * the piece "└" (a corner in north orientation).
 */
static uint _code[NB_SHAPES][NB_DIRS] = {
    {0b0000, 0b0000, 0b0000, 0b0000}, // EMPTY {" ", " ", " ", " "}
    {0b1000, 0b0100, 0b0010, 0b0001}, // ENDPOINT {"^", ">", "v", "<"},
    {0b1010, 0b0101, 0b1010, 0b0101}, // SEGMENT {"|", "-", "|", "-"},
    {0b1100, 0b0110, 0b0011, 0b1001}, // CORNER {"└", "┌", "┐", "┘"}
    {0b1101, 0b1110, 0b0111, 0b1011}, // TEE {"┴", "├", "┬", "┤"}
    {0b1111, 0b1111, 0b1111, 0b1111}  // CROSS {"+", "+", "+", "+"}
};

/* ************************************************************************** */

/** encode a shape and an orientation into an integer code */
static uint _encode_shape(shape s, direction o) { return _code[s][o]; }

/* ************************************************************************** */

/** decode an integer code into a shape and an orientation */
static bool _decode_shape(uint code, shape* s, direction* o) {
    assert(code >= 0 && code < 16);
    assert(s);
    assert(o);
    for (int i = 0; i < NB_SHAPES; i++)
        for (int j = 0; j < NB_DIRS; j++)
            if (code == _code[i][j]) {
                *s = i;
                *o = j;
                return true;
            }
    return false;
}

/* ************************************************************************** */

/** add an half-edge in the direction d */
static void _add_half_edge(game g, uint i, uint j, direction d) {
    assert(g);
    assert(i < game_nb_rows(g));
    assert(j < game_nb_cols(g));
    assert(d < NB_DIRS);

    shape s = game_get_piece_shape(g, i, j);
    direction o = game_get_piece_orientation(g, i, j);
    uint code = _encode_shape(s, o);
    uint mask = 0b1000 >> d;    // mask with half-edge in the direction d
    assert((code & mask) == 0); // check there is no half-edge in the direction d
    uint newcode = code | mask; // add the half-edge in the direction d
    shape news;
    direction newo;
    bool ok = _decode_shape(newcode, &news, &newo);
    assert(ok);
    game_set_piece_shape(g, i, j, news);
    game_set_piece_orientation(g, i, j, newo);
}

/* ************************************************************************** */

#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)

/* ************************************************************************** */

/**
 * @brief Add an edge between two adjacent squares.
 * @details This is done by modifying the pieces of the two adjacent squares.
 * More precisely, we add an half-edge to each adjacent square, so as to build
 * an edge between these two squares.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param d the direction of the adjacent square
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if an edge can be added, false otherwise
 */
static bool _add_edge(game g, uint i, uint j, direction d) {
    assert(g);
    assert(i < game_nb_rows(g));
    assert(j < game_nb_cols(g));
    assert(d < NB_DIRS);

    uint nexti, nextj;
    bool next = game_get_ajacent_square(g, i, j, d, &nexti, &nextj);
    if (!next)
        return false;

    // check if the two half-edges are free
    bool he = game_has_half_edge(g, i, j, d);
    if (he)
        return false;
    bool next_he = game_has_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));
    if (next_he)
        return false;

    _add_half_edge(g, i, j, d);
    _add_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));

    return true;
}

/* ************************************************************************** */

game game_load(char* filename) {
    assert(filename != NULL);

    // Ouvrir le fichier en lecture
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Lire les dimensions et wrapping
    uint nb_rows, nb_cols;
    int wrapping;
    if (fscanf(file, "%u %u %d", &nb_rows, &nb_cols, &wrapping) != 3) {
        fprintf(stderr, "Erreur: Format invalide dans le fichier %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Créer le jeu vide
    game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);

    // Lire la grille
    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            char shape_char, dir_char;
            if (fscanf(file, " %c %c", &shape_char, &dir_char) != 2) {
                fprintf(stderr, "Erreur: Données manquantes ou incorrectes\n");
                game_delete(g);
                fclose(file);
                exit(EXIT_FAILURE);
            }

            shape s;
            direction d;

            // Conversion des caractères en `shape`
            switch (shape_char) {
            case 'E':
                s = EMPTY;
                break;
            case 'N':
                s = ENDPOINT;
                break;
            case 'S':
                s = SEGMENT;
                break;
            case 'C':
                s = CORNER;
                break;
            case 'T':
                s = TEE;
                break;
            case 'X':
                s = CROSS;
                break;
            default:
                fprintf(stderr, "Erreur: Shape inconnu '%c'\n", shape_char);
                game_delete(g);
                fclose(file);
                exit(EXIT_FAILURE);
            }

            // Conversion des caractères en `direction`
            switch (dir_char) {
            case 'N':
                d = NORTH;
                break;
            case 'E':
                d = EAST;
                break;
            case 'S':
                d = SOUTH;
                break;
            case 'W':
                d = WEST;
                break;
            default:
                fprintf(stderr, "Erreur: Direction inconnue '%c'\n", dir_char);
                game_delete(g);
                fclose(file);
                exit(EXIT_FAILURE);
            }

            // Mettre à jour la pièce dans le jeu
            game_set_piece_shape(g, i, j, s);
            game_set_piece_orientation(g, i, j, d);
        }
    }

    fclose(file);
    return g;
}

void game_save(cgame g, char* filename) {
    assert(g != NULL);
    assert(filename != NULL);

    // Ouvrir le fichier en écriture
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier %s pour sauvegarde\n", filename);
        exit(EXIT_FAILURE);
    }

    // Sauvegarder les dimensions du jeu et l'option de wrapping
    fprintf(file, "%u %u %d\n", game_nb_rows(g), game_nb_cols(g), game_is_wrapping(g));

    // Sauvegarder la grille (formes et orientations)
    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            shape s = game_get_piece_shape(g, i, j);
            direction d = game_get_piece_orientation(g, i, j);

            char shape_char, dir_char;

            // Conversion de shape en caractère
            switch (s) {
            case EMPTY:
                shape_char = 'E';
                break;
            case ENDPOINT:
                shape_char = 'N';
                break;
            case SEGMENT:
                shape_char = 'S';
                break;
            case CORNER:
                shape_char = 'C';
                break;
            case TEE:
                shape_char = 'T';
                break;
            case CROSS:
                shape_char = 'X';
                break;
            default:
                fprintf(stderr, "Erreur: Shape invalide lors de la sauvegarde\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }

            // Conversion de direction en caractère
            switch (d) {
            case NORTH:
                dir_char = 'N';
                break;
            case EAST:
                dir_char = 'E';
                break;
            case SOUTH:
                dir_char = 'S';
                break;
            case WEST:
                dir_char = 'W';
                break;
            default:
                fprintf(stderr, "Erreur: Direction invalide lors de la sauvegarde\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }

            // Écrire la forme et la direction dans le fichier
            fprintf(file, "%c%c ", shape_char, dir_char);
        }
        fprintf(file, "\n");
    }

    // Fermer le fichier après l'écriture
    fclose(file);
}

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_empty, uint nb_extra) {
    if (nb_empty > nb_rows * nb_cols) {
        fprintf(stderr, "Erreur: Trop de cases vides\n");
        exit(EXIT_FAILURE);
    }
    if (nb_empty == (nb_rows * nb_cols) - 1) {
        fprintf(stderr, "Erreur: Il n'existe pas de solution avec 1 seul case non-vide\n");
        exit(EXIT_FAILURE);
    }
    game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
    if (nb_empty == nb_rows * nb_cols) {
        return g;
    }

    uint i = (((double)rand() * nb_rows) / RAND_MAX);
    uint j = (((double)rand() * nb_cols) / RAND_MAX);
    uint d = (((double)rand() * NB_DIRS) / RAND_MAX);

    game_set_piece_shape(g, i, j, ENDPOINT);
    uint i_adj, j_adj;
    for (uint attempts = 0; attempts < NB_DIRS; attempts++) {
        if (game_get_ajacent_square(g, i, j, d, &i_adj, &j_adj)) {
            break;
        }
        d = (d + 1) % NB_DIRS;
    }
    game_set_piece_orientation(g, i, j, d);
    game_set_piece_shape(g, i_adj, j_adj, ENDPOINT);
    game_set_piece_orientation(g, i_adj, j_adj, OPPOSITE_DIR(d));
    for (int k = 2; k < nb_rows * nb_cols - nb_empty; k++) {
        bool found = false;
        while (!found) {
            i = rand() % nb_rows;
            j = rand() % nb_cols;
            if (game_get_piece_shape(g, i, j) != EMPTY) {
                for (uint attempts = 0; attempts < NB_DIRS; attempts++) {
                    d = (d + 1) % NB_DIRS;
                    if (game_get_ajacent_square(g, i, j, d, &i_adj, &j_adj)) {
                        if (game_get_piece_shape(g, i_adj, j_adj) == EMPTY) {
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
        _add_edge(g, i, j, d);
    }
    if (nb_extra > (nb_cols - 1) * (nb_rows - 1)) {
        fprintf(stderr, "Impoosible d'ajouter %u cycles, vérifiéz nb_extra\n", nb_extra);
        exit(EXIT_FAILURE);
    }
    for (int l = 0; l < nb_extra; l++) {
        bool found = false;
        while (!found) {
            i = rand() % nb_rows;
            j = rand() % nb_cols;
            if (game_get_piece_shape(g, i, j) != EMPTY) {
                for (uint attempts = 0; attempts < NB_DIRS; attempts++) {
                    d = (d + 1) % NB_DIRS;
                    if (game_get_ajacent_square(g, i, j, d, &i_adj, &j_adj)) {
                        if (game_check_edge(g, i, j, d) != MATCH) {
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
        _add_edge(g, i, j, d);
    }
    return g;
}

void game_nb_solutions_aux(cgame g, uint index, uint* cpt) {
    if (index >= game_nb_cols(g) * game_nb_rows(g)) {
        if (game_won(g)) {
            (*cpt)++;
        }
        return;
    }

    uint x = index / game_nb_cols(g);
    uint y = index % game_nb_cols(g);

    shape shape = game_get_piece_shape(g, x, y);
    direction dir = game_get_piece_orientation(g, x, y);

    int rotations = 4;

    if (shape == SEGMENT) {
        rotations = 2;
    } else if (shape == CROSS || shape == EMPTY) {
        rotations = 1;
    }

    for (int i = 0; i < rotations; i++) {
        game_set_piece_orientation((game)g, x, y, (dir + i) % 4);
        if (!game_is_wrapping(g)) {
            if (game_check_edge(g, x, y, NORTH) == MISMATCH || (game_check_edge(g, x, y, WEST)) == MISMATCH) {
                continue;
            }
        } else {
            if (x == game_nb_rows(g) - 1) {
                if (game_check_edge(g, x, y, SOUTH) == MISMATCH) {
                    continue;
                }
            }
            if (y == game_nb_cols(g) - 1) {
                if (game_check_edge(g, x, y, EAST) == MISMATCH) {
                    continue;
                }
            }
            if (x == 0 && y != 0) {
                if (game_check_edge(g, x, y, WEST) == MISMATCH) {
                    continue;
                }
            }
            if (x != 0 && y == 0) {
                if (game_check_edge(g, x, y, NORTH) == MISMATCH) {
                    continue;
                }
            }
            if (x != 0 && y != 0) {
                if (game_check_edge(g, x, y, NORTH) == MISMATCH || (game_check_edge(g, x, y, WEST)) == MISMATCH) {
                    continue;
                }
            }
        }
        game_nb_solutions_aux(g, index + 1, cpt);
    }
    game_set_piece_orientation((game)g, x, y, dir);
}

uint game_nb_solutions(cgame g) {
    uint cpt = 0;
    game_nb_solutions_aux(g, 0, &cpt);
    return cpt;
}

bool game_solve_aux(game g, uint index) {
    if (index >= game_nb_cols(g) * game_nb_rows(g)) {
        if (game_won(g)) {
            game_print(g);
            return true;
        }
        return false;
    }

    uint x = index / game_nb_cols(g);
    uint y = index % game_nb_cols(g);

    shape shape = game_get_piece_shape(g, x, y);
    direction dir = game_get_piece_orientation(g, x, y);

    int rotations = 4;

    if (shape == SEGMENT) {
        rotations = 2;
    } else if (shape == CROSS || shape == EMPTY) {
        rotations = 1;
    }

    for (int i = 0; i < rotations; i++) {
        game_set_piece_orientation(g, x, y, (dir + i) % rotations);
        if (!game_is_wrapping(g)) {
            if (game_check_edge(g, x, y, NORTH) == MISMATCH || (game_check_edge(g, x, y, WEST)) == MISMATCH) {
                continue;
            }
        } else {
            if (x == game_nb_rows(g) - 1) {
                if (game_check_edge(g, x, y, SOUTH) == MISMATCH) {
                    continue;
                }
            }
            if (y == game_nb_cols(g) - 1) {
                if (game_check_edge(g, x, y, EAST) == MISMATCH) {
                    continue;
                }
            }
            if (x == 0 && y != 0) {
                if (game_check_edge(g, x, y, WEST) == MISMATCH) {
                    continue;
                }
            }
            if (x != 0 && y == 0) {
                if (game_check_edge(g, x, y, NORTH) == MISMATCH) {
                    continue;
                }
            }
            if (x != 0 && y != 0) {
                if (game_check_edge(g, x, y, NORTH) == MISMATCH || (game_check_edge(g, x, y, WEST)) == MISMATCH) {
                    continue;
                }
            }
        }
        if (game_solve_aux(g, index + 1)) {
            return true;
        }
    }
    game_set_piece_orientation(g, x, y, dir);
    return false;
}

bool game_solve(game g) { return game_solve_aux(g, 0); }
