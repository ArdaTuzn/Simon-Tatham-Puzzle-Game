#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(int argc, char* argv[]) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        usage(argc, argv);
        exit(EXIT_FAILURE);
    }
    game g;
    if (argc == 3) {
        if (strcmp(argv[1], "-s") == 0) {
            char* filename = argv[2];
            g = game_load(filename);
            if (!game_solve(g)) {
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[1], "-c") == 0) {
            char* filename = argv[2];
            g = game_load(filename);
            int a = game_nb_solutions(g);
            printf("%d\n", a);
        } else {
            usage(argc, argv);
        }
    }

    else if (argc == 4) {
        if (strcmp(argv[1], "-s") == 0) {
            char* filename = argv[2];
            char* output = argv[3];
            g = game_load(filename);
            if (!game_solve(g)) {
                return EXIT_FAILURE;
            } else {
                game_save(g, output);
            }
        } else if (strcmp(argv[1], "-c") == 0) {
            char* filename = argv[2];
            char* output = argv[3];
            FILE* f = fopen(output, "w");
            g = game_load(filename);
            int a = game_nb_solutions(g);
            fprintf(f, "%d\n", a);
        } else {
            usage(argc, argv);
        }
    }
    return EXIT_SUCCESS;
}
