#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Crée un jeu vide avec des valeurs par défaut.
 * Retourne un pointeur vers la structure de jeu, ou NULL en cas d'erreur.
 */
game game_new_empty(void) {
    // alouer de la memoire pour la structure de jeu
    game g = malloc(sizeof(game_s));
    if (!g) {
        fprintf(stderr, "Erreur de creation du jeu \n");
        return NULL;
    }

    // taille totale de la grille
    uint size = DEFAULT_SIZE * DEFAULT_SIZE;
    g->nb_columns = DEFAULT_SIZE;
    g->nb_rows = DEFAULT_SIZE;
    g->wrapping = false;
    g->undo_stack = queue_new();
    g->redo_stack = queue_new();

    // allouer et initiliaser les tableaux de formes et des orientations
    g->s = calloc(size, sizeof(shape));
    g->d = calloc(size, sizeof(direction));
    // verifications des allocations
    if (!g->s || !g->d || !g->undo_stack || !g->redo_stack) {
        game_delete(g);
        return NULL;
    }
    return g; // return le jeu creer
}

/**
 * Crée un nouveau jeu basé sur les formes et orientations fournies.
 * Si les tableaux sont NULL, les cases sont initialisées avec des valeurs par défaut.
 */
game game_new(shape* shapes, direction* orientations) {
    // Crée un jeu vide en utilisant la fonction game_new_empty
    game g = game_new_empty();
    if (!g) {
        fprintf(stderr, "Erreur de creation du jeu \n");
        exit(1);
    }

    // Calculer la taille de la grille
    uint size = DEFAULT_SIZE * DEFAULT_SIZE;
    // Initialiser les formes et les orientations pour chaque case
    for (uint i = 0; i < size; i++) {
        g->s[i] = (shapes) ? shapes[i] : EMPTY;
        g->d[i] = (orientations) ? orientations[i] : NORTH;
    }
    return g;
}

/**
 * Copie un jeu existant.
 * Retourne une nouvelle instance de jeu identique ou NULL en cas d'erreur.
 */

game game_copy(cgame g) {
    if (g == NULL) {
        return NULL;
    }
    uint size = game_nb_rows(g) * game_nb_cols(g);
    game new_game = game_new_empty();
    if (new_game == NULL) {
        fprintf(stderr, "Erreur allocation de memoire \n");
        return NULL;
    }
    // Copier les propriétés et les tableaux
    memcpy(new_game->s, g->s, size * sizeof(shape));
    memcpy(new_game->d, g->d, size * sizeof(direction));
    new_game->nb_rows = game_nb_rows(g);
    new_game->nb_columns = game_nb_cols(g);
    new_game->wrapping = game_is_wrapping(g);
    return new_game;
}

/**
 * Compare deux jeux pour vérifier s'ils sont identiques.
 * Si ignore_orientation est vrai, seules les formes sont comparées.
 */
bool game_equal(cgame g1, cgame g2, bool ignore_orientation) {
    // Vérifier si les deux jeux sont valides
    if (g1 == NULL || g2 == NULL) {
        return false;
    }

    // Récupérer les dimensions des jeux
    uint rows1 = game_nb_rows(g1);
    uint cols1 = game_nb_cols(g1);
    uint rows2 = game_nb_rows(g2);
    uint cols2 = game_nb_cols(g2);

    // Vérifier si les dimensions des deux jeux sont identiques
    if (rows1 != rows2 || cols1 != cols2) {
        return false;
    }

    // Calculer la taille totale
    uint size = rows1 * cols1;

    // Comparer les formes avec memcmp
    if (memcmp(g1->s, g2->s, size * sizeof(shape)) != 0) {
        return false;
    }

    // Si on n'ignore pas les orientations, comparer également
    if (!ignore_orientation) {
        if (memcmp(g1->d, g2->d, size * sizeof(direction)) != 0) {
            return false;
        }
    }
    // On teste si l'option 'wrapping' des deux jeux sont égaux
    if (game_is_wrapping(g1) != game_is_wrapping(g2)) {
        return false;
    }

    return true; // Les jeux sont identiques
}

/**
 * Libère les ressources associées à un jeu.
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
 * Définit la forme d'une pièce dans le jeu.
 */
void game_set_piece_shape(game g, uint i, uint j, shape s) {
    if (!g || i >= g->nb_rows || j >= g->nb_columns) {
        fprintf(stderr, "Erreur dans les paramètres ou indices invalides.\n");
        exit(EXIT_FAILURE);
    }
    // Vérifier si le "shape" est valide
    if (s != EMPTY && s != ENDPOINT && s != SEGMENT && s != CORNER && s != TEE && s != CROSS) {
        fprintf(stderr, "Shape invalide\n");
        exit(1);
    }
    g->s[i * game_nb_cols(g) + j] = s;
}

/**
 * Définit l'orientation d'une pièce dans le jeu.
 */
void game_set_piece_orientation(game g, uint i, uint j, direction o) {
    if (!g || i >= g->nb_rows || j >= g->nb_columns) {
        fprintf(stderr, "Erreur dans les paramètres ou indices invalides.\n");
        exit(EXIT_FAILURE);
    }
    // Vérifier si le "direction" est valide
    if (o != NORTH && o != EAST && o != SOUTH && o != WEST) {
        fprintf(stderr, "Orientation invalide\n");
    }
    g->d[i * game_nb_cols(g) + j] = o;
}

/**
 * Récupère la forme d'une pièce.
 */
shape game_get_piece_shape(cgame g, uint i, uint j) {

    if (!g || i >= g->nb_rows || j >= g->nb_columns) {
        fprintf(stderr, "Indices invalides ou jeu invalide.\n");
        exit(EXIT_FAILURE);
    }
    return g->s[i * game_nb_cols(g) + j];
}

direction game_get_piece_orientation(cgame g, uint i, uint j) {
    // Vérifier si le jeu est valide
    if (!g) {
        fprintf(stderr, "Erreur dans les parametrès\n");
        exit(1);
    }
    // Vérifier si les indices sont valides
    if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
        fprintf(stderr, "Erreur dans les indices\n");
        exit(1);
    }
    return g->d[i * game_nb_cols(g) + j];
}

/**
 * Joue un mouvement en faisant tourner une pièce d'un nombre de quarts de tour.
 */
void game_play_move(game g, uint i, uint j, int nb_quarter_turns) {
    // Varifier si le jeu est valid
    if (!g) {
        fprintf(stderr, "Erreur dans les parametrès\n");
        exit(1);
    }
    // Vérifier si les indices sont valides
    if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
        fprintf(stderr, "Erreur dans les indices\n");
        exit(1);
    }
    // On stocke le mouvement en tant que tableau d'entiers pour les fonctions game_undo et game_redo
    int* move = malloc(3 * sizeof(int));
    if (!move) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }
    move[0] = i;
    move[1] = j;
    move[2] = nb_quarter_turns;
    // On enfile le mouvement en undo_stack
    queue_push_tail(g->undo_stack, move);
    // On libére le redo_stack car un nouveau mouvement a été effectué
    while (!queue_is_empty(g->redo_stack)) {
        free(queue_pop_head(g->redo_stack));
    }

    // Si nb_quarter_turns >= 4, on fait un tour est on arrive au début donc on peut prendre nb_quarter_turns modulo 4,
    // on ajoute 4 pour gèrer les valeurs négatives et on fait modulo 4 car on a ajouté 4
    int move2 = (nb_quarter_turns % 4 + 4) % 4;
    for (uint k = 0; k < move2; k++) {
        if (g->d[i * game_nb_cols(g) + j] == WEST) {
            g->d[i * game_nb_cols(g) + j] = NORTH;
        } else {
            g->d[i * game_nb_cols(g) + j] = (g->d[i * game_nb_cols(g) + j]) + 1;
        }
    }
}

bool game_won(cgame g) {
    // Vérifie que le jeu est valide
    if (!g) {
        fprintf(stderr, "Erreur: pointeur de jeu invalide.\n");
        exit(EXIT_FAILURE);
    }
    // Renvoie vrai si les deux conditions sont remplies
    return game_is_well_paired(g) && game_is_connected(g);
}

void game_reset_orientation(game g) {
    // Vérifier si le jeu est valide
    if (!g && !g->d) {
        fprintf(stderr, "Erreur dans les parametrès\n");
        exit(1);
    }
    int size = game_nb_rows(g) * game_nb_cols(g);
    // On parcourut le jeu et on met tout les orientation en nord
    for (uint i = 0; i < size; i++) {
        g->d[i] = NORTH;
    }
}

void game_shuffle_orientation(game g) {
    // Vérifier si le jeu est valide
    if (!g) {
        fprintf(stderr, "Erreur dans les parametrès\n");
        exit(1);
    }
    int size = game_nb_rows(g) * game_nb_cols(g);
    // Pour tous les indices du jeu, il faut choisir un direction (entre 0=NORTH et 3=WEST) de façon aléatoire pour faire "shuffle"
    for (uint i = 0; i < size; i++) {
        int randomNumber = rand() % 4;
        g->d[i] = randomNumber;
    }
}
