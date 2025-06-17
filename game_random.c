#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (argc != 7 && argc != 8) {
        fprintf(stderr, "Usage ./game_random <nb_rows> <nb_cols> <wrapping> <nb_empty> <nb_extra> <shuffle> [<filename>]\n");
        fprintf(stderr, "Example: ./game_random 4 4 0 0 0 0 random.sol\n");
        return EXIT_FAILURE;
    }
    game g = game_random(strtoul(argv[1], NULL, 10), strtoul(argv[2], NULL, 10), strtoul(argv[3], NULL, 10), strtoul(argv[4], NULL, 10), strtoul(argv[5], NULL, 10));
    printf("nb_rows: %u ", game_nb_rows(g));
    printf("nb_cols: %u ", game_nb_cols(g));
    printf("wrapping: %d\n", game_is_wrapping(g));
    printf("nb_empty: %lu ", strtoul(argv[4], NULL, 10));
    printf("nb_extra: %lu ", strtoul(argv[5], NULL, 10));
    printf("shuffle: %lu\n", strtoul(argv[6], NULL, 10));
    if (strtoul(argv[6], NULL, 10)) {
        game_shuffle_orientation(g);
    }
    if (argc == 8) {
        game_save(g, argv[7]);
    }
    game_print(g);
    game_delete(g);
    return EXIT_SUCCESS;
}