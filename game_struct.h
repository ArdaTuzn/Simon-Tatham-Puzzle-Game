#include "game.h"
#include "game_aux.h"
#include "queue/queue.h"

#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

struct game_s {
    shape* s;
    direction* d;
    uint nb_columns;
    uint nb_rows;
    bool wrapping;
    queue* undo_stack; // Historique des coups
    queue* redo_stack; // Historique des coups annulés
};

typedef struct game_s game_s;

#endif