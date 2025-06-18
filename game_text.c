#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include <stdio.h>
#include <stdlib.h>

void print_help(void) {
    printf("Available commands:\n");
    printf("h : Show help\n");
    printf("r : Reset the grid (shuffle)\n");
    printf("q : Quit\n");
    printf("c i j : Rotate the piece at (i,j) clockwise\n");
    printf("a i j : Rotate the piece at (i,j) counterclockwise\n");
    printf("z : Undo the last move\n");
    printf("y : Redo the last undone move\n");
}

void usage(int argc, char* argv[]) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        usage(argc, argv);
        exit(EXIT_FAILURE);
    }
    game g = NULL;
    if (argc == 2) {
        char* filename = argv[1];
        g = game_load(filename);
    } else {
        g = game_default();
    }

    // variables
    char command;
    int i, j;
    bool game_over = false;

    while (!game_over && !game_won(g)) {
        // display the game
        game_print(g);
        printf("\n");
        printf("Enter a command (h for help):\n");
        scanf(" %c", &command);

        if (command == 'h') {
            print_help();
        } else if (command == 'r') {
            game_shuffle_orientation(g);
            printf("Grid has been reset\n");
        } else if (command == 'q') {
            game_over = true;
            printf("You have quit the game\n");
        } else if (command == 'z') {
            game_undo(g);
        } else if (command == 'y') {
            game_redo(g);
        } else if (command == 'c' || command == 'a') {
            printf("Enter the coordinates i and j:\n");
            scanf("%d %d", &i, &j);
            if (i >= 0 && i < DEFAULT_SIZE && j >= 0 && j < DEFAULT_SIZE) {
                if (command == 'c') {
                    game_play_move(g, i, j, 1);
                } else if (command == 'a') {
                    game_play_move(g, i, j, -1);
                }
            } else {
                printf("Invalid coordinates\n");
            }
        }
    }

    // display the final grid
    game_print(g);
    printf("\n");

    // check if the game is won
    if (game_won(g)) {
        printf("Congratulations: You won the game!\n");
    } else if (game_over) {
        printf("You gave up the game. Better luck next time!\n");
    }

    game_delete(g);
    return EXIT_SUCCESS;
}
