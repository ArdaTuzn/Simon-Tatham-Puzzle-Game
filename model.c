#include "model.h"
#include "game.h"
#include "game_aux.h"
#include "game_tools.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* **************************************************************** */

struct Env_t {
    game g;
    SDL_Texture* piece_textures[6];
    SDL_Texture* background_texture;
    TTF_Font* font;
    int cell_size;
    int grid_x, grid_y;
    int window_width, window_height;
    int button_area_height;
    int margin;
    
    // Boutons
    SDL_Rect reset_btn;
    SDL_Rect quit_btn;
    SDL_Rect undo_btn;
    SDL_Rect redo_btn;
    SDL_Rect solve_btn;  // Nouveau bouton
};

/* **************************************************************** */

void renderText(SDL_Renderer* ren, TTF_Font* font, const char* text, SDL_Color color, SDL_Rect rect) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
        if (texture) {
            SDL_Rect text_rect = {
                rect.x + (rect.w - surface->w)/2,
                rect.y + (rect.h - surface->h)/2,
                surface->w,
                surface->h
            };
            SDL_RenderCopy(ren, texture, NULL, &text_rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

/* **************************************************************** */

Env *init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]) {
    Env *env = malloc(sizeof(struct Env_t));
    if (!env) ERROR("Erreur d'allocation mémoire pour Env\n");

    env->g = (argc == 2) ? game_load(argv[1]) : game_default();
    env->button_area_height = 60;
    env->margin = 20;

    SDL_GetWindowSize(win, &(env->window_width), &(env->window_height));

    env->font = TTF_OpenFont("../res/Arial.ttf", 24);
    if (!env->font) ERROR("Erreur de chargement de la police: %s\n", TTF_GetError());

    const char* textures_path[6] = {
       "../res/corner.png", "../res/cross.png", "../res/empty.png",
       "../res/endpoint.png", "../res/segment.png", "../res/tee.png"
    };

    for (int i = 0; i < 6; i++) {
        env->piece_textures[i] = IMG_LoadTexture(ren, textures_path[i]);
        if (!env->piece_textures[i]) ERROR("Erreur de chargement de la texture %s: %s\n", textures_path[i], IMG_GetError());
    }

    // Charger l'image de fond
    env->background_texture = IMG_LoadTexture(ren, "../res/background.png");
    if (!env->background_texture) {
        printf("Warning: Impossible de charger l'image de fond (%s)\n", IMG_GetError());
    }

    // Configuration des boutons
    int btn_width = 80;
    int btn_height = 30;
    
    env->reset_btn = (SDL_Rect){env->margin, env->margin, btn_width, btn_height};
    env->quit_btn = (SDL_Rect){env->margin*2 + btn_width, env->margin, btn_width, btn_height};
    env->undo_btn = (SDL_Rect){env->margin*3 + btn_width*2, env->margin, btn_width, btn_height};
    env->redo_btn = (SDL_Rect){env->margin*4 + btn_width*3, env->margin, btn_width, btn_height};
    env->solve_btn = (SDL_Rect){env->margin*5 + btn_width*4, env->margin, btn_width, btn_height}; 

    // Calcul de la taille des cellules
    int rows = game_nb_rows(env->g);
    int cols = game_nb_cols(env->g);
    
    int available_width = env->window_width - 2 * env->margin;
    int available_height = env->window_height - env->button_area_height - 2 * env->margin;
    
    int cell_size_width = available_width / cols;
    int cell_size_height = available_height / rows;
    
    env->cell_size = (cell_size_width < cell_size_height) ? cell_size_width : cell_size_height;
    
    // Centrage de la grille
    env->grid_x = (env->window_width - cols * env->cell_size) / 2;
    env->grid_y = env->button_area_height + (available_height - rows * env->cell_size) / 2;

    return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
    // Affichage du fond
    if (env->background_texture) {
        SDL_Rect bg_rect = {0, 0, env->window_width, env->window_height};
        SDL_RenderCopy(ren, env->background_texture, NULL, &bg_rect);
    } else {
        SDL_SetRenderDrawColor(ren, 240, 240, 240, 255);
        SDL_RenderClear(ren);
    }

    // Zone des boutons
    SDL_Rect button_bg = {0, 0, env->window_width, env->button_area_height + 2*env->margin};
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderFillRect(ren, &button_bg);

    // Boutons
    SDL_Color btn_color = {200, 200, 200, 255};
    SDL_Color text_color = {0, 0, 0, 255};
    
    SDL_SetRenderDrawColor(ren, btn_color.r, btn_color.g, btn_color.b, btn_color.a);
    SDL_RenderFillRect(ren, &env->reset_btn);
    SDL_RenderFillRect(ren, &env->quit_btn);
    SDL_RenderFillRect(ren, &env->undo_btn);
    SDL_RenderFillRect(ren, &env->redo_btn);
    SDL_RenderFillRect(ren, &env->solve_btn);  
    
    renderText(ren, env->font, "Reset", text_color, env->reset_btn);
    renderText(ren, env->font, "Quitter", text_color, env->quit_btn);
    renderText(ren, env->font, "Undo", text_color, env->undo_btn);
    renderText(ren, env->font, "Redo", text_color, env->redo_btn);
    renderText(ren, env->font, "Solve", text_color, env->solve_btn); 

    // Dessiner les pièces
    float scale = 1.0f;
    int offset = (env->cell_size - (env->cell_size * scale)) / 2;
    
    for (int i = 0; i < game_nb_rows(env->g); i++) {
        for (int j = 0; j < game_nb_cols(env->g); j++) {
            shape s = game_get_piece_shape(env->g, i, j);
            direction d = game_get_piece_orientation(env->g, i, j);
         
            SDL_Rect piece_rect = {
                env->grid_x + j * env->cell_size + offset,
                env->grid_y + i * env->cell_size + offset,
                env->cell_size * scale,
                env->cell_size * scale
            };

            SDL_Texture* tex = NULL;
            switch (s) {
                case CORNER: tex = env->piece_textures[0]; break;
                case CROSS: tex = env->piece_textures[1]; break;
                case EMPTY: tex = env->piece_textures[2]; break;
                case ENDPOINT: tex = env->piece_textures[3]; break;
                case SEGMENT: tex = env->piece_textures[4]; break;
                case TEE: tex = env->piece_textures[5]; break;
                default: break;
            }

            if (tex) {
                SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                SDL_RenderDrawRect(ren, &piece_rect);
                SDL_RenderCopyEx(ren, tex, NULL, &piece_rect, d * 90, NULL, SDL_FLIP_NONE);
            }
        }
    }

    // Lignes de grille
    int grid_width = game_nb_cols(env->g) * env->cell_size;
    int grid_height = game_nb_rows(env->g) * env->cell_size;

    SDL_SetRenderDrawColor(ren, 255, 100, 100, 180);
    for (int i = 0; i <= game_nb_rows(env->g); i++) {
        SDL_RenderDrawLine(ren, env->grid_x, env->grid_y + i * env->cell_size, 
                         env->grid_x + grid_width, env->grid_y + i * env->cell_size);
    }
    for (int j = 0; j <= game_nb_cols(env->g); j++) {
        SDL_RenderDrawLine(ren, env->grid_x + j * env->cell_size, env->grid_y, 
                         env->grid_x + j * env->cell_size, env->grid_y + grid_height);
    }
    
    // Message de victoire
    if (game_won(env->g)) {
        SDL_Color green = {0, 180, 0, 255};
        SDL_Color bg_color = {255, 255, 255, 230};  
        
        // Positionne des buttons
        SDL_Rect msg_bg = {
            env->window_width/2 - 100,
            env->button_area_height + env->margin, 
            200, 
            40
        };
        
        SDL_SetRenderDrawColor(ren, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderFillRect(ren, &msg_bg);
        
        
        SDL_SetRenderDrawColor(ren, green.r, green.g, green.b, 255);
        SDL_RenderDrawRect(ren, &msg_bg);
        
        renderText(ren, env->font, "Game Won!", green, msg_bg);
    }

    SDL_RenderPresent(ren);
}

/* **************************************************************** */

bool process(SDL_Window* win, SDL_Renderer* ren, Env* env, SDL_Event* e) {
    switch (e->type) {
        case SDL_QUIT:
            return true;

        case SDL_KEYDOWN:
            switch (e->key.keysym.sym) {
                case SDLK_q: return true;
                case SDLK_r: game_shuffle_orientation(env->g); break;
                case SDLK_z: game_undo(env->g); break;
                case SDLK_y: game_redo(env->g); break;
                case SDLK_s:game_solve(env->g) ; break; 
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (e->button.button == SDL_BUTTON_LEFT) {
                SDL_Point mouse_pos = {e->button.x, e->button.y};
                
                if (SDL_PointInRect(&mouse_pos, &env->reset_btn)) {
                    game_shuffle_orientation(env->g);
                }
                else if (SDL_PointInRect(&mouse_pos, &env->quit_btn)) {
                    return true;
                }
                else if (SDL_PointInRect(&mouse_pos, &env->undo_btn)) {
                    game_undo(env->g);
                }
                else if (SDL_PointInRect(&mouse_pos, &env->redo_btn)) {
                    game_redo(env->g);
                }
                else if (SDL_PointInRect(&mouse_pos, &env->solve_btn)) {
                    // Résoudre le jeu
                    game_solve(env->g);
                }
                else {
                    // Clic sur la grille
                    int i = (e->button.y - env->grid_y) / env->cell_size;
                    int j = (e->button.x - env->grid_x) / env->cell_size;
                    if (i >= 0 && i < game_nb_rows(env->g) && j >= 0 && j < game_nb_cols(env->g)) {
                        game_play_move(env->g, i, j, (e->button.button == SDL_BUTTON_LEFT) ? 1 : -1);
                    }
                }
            }
            break;

        case SDL_WINDOWEVENT:
            if (e->window.event == SDL_WINDOWEVENT_RESIZED) {
                env->window_width = e->window.data1;
                env->window_height = e->window.data2;
                
                // Recalculer la taille des cellules
                int rows = game_nb_rows(env->g);
                int cols = game_nb_cols(env->g);
                
                int available_width = env->window_width - 2 * env->margin;
                int available_height = env->window_height - env->button_area_height - 2 * env->margin;
                
                int cell_size_width = available_width / cols;
                int cell_size_height = available_height / rows;
                
                env->cell_size = (cell_size_width < cell_size_height) ? cell_size_width : cell_size_height;
                
                // Recentrer la grille
                env->grid_x = (env->window_width - cols * env->cell_size) / 2;
                env->grid_y = env->button_area_height + (available_height - rows * env->cell_size) / 2;
            }
            break;
    }
    return false;
}

/* **************************************************************** */

void clean(SDL_Window* win, SDL_Renderer* ren, Env* env) {
    if (!env) return;

    for (int i = 0; i < 6; i++) {
        if (env->piece_textures[i]) SDL_DestroyTexture(env->piece_textures[i]);
    }
    if (env->background_texture) SDL_DestroyTexture(env->background_texture);
    if (env->font) TTF_CloseFont(env->font);
    if (env->g) game_delete(env->g);
    free(env);
}