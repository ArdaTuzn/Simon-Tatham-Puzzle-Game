cmake_minimum_required(VERSION 3.3)
project(net_c15)
include(CTest)

# Définir les flags du compilateur
set(CMAKE_C_FLAGS "-Wall -std=c99")
set(CMAKE_C_FLAGS_DEBUG "-g -Og")             
set(CMAKE_C_FLAGS_RELEASE "-O3") 

#Ajout du librarie 'game'
#add_library(game STATIC game.c game_aux.c)
# Ajout des sources pour la bibliothèque game
include_directories(queue)
add_library(game STATIC game.c game_aux.c game_ext.c queue/queue.c game_tools.c) 

# Ajout des exécutables
add_executable(game_text game_text.c)
add_executable(game_test_kyereli game_test_kyereli.c)
add_executable(game_test_elhaddiallo game_test_elhaddiallo.c)
add_executable(game_test_atuzun game_test_atuzun.c)
add_executable(game_random game_random.c)
add_executable(game_solve game_solve.c)

target_link_libraries(game_text game)
target_link_libraries(game_test_atuzun PRIVATE game)
target_link_libraries(game_test_kyereli PRIVATE game)
target_link_libraries(game_test_elhaddiallo PRIVATE game)
target_link_libraries(game_random game)
target_link_libraries(game_solve game)

#SDL2 
include(sdl2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")
include_directories(${SDL2_ALL_INC})
add_executable(game_sdl game_sdl.c model.c)
target_link_libraries(game_sdl PRIVATE ${SDL2_ALL_LIBS} game)

# file copy
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/default.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


# Configuration des tests
enable_testing()
add_test(test_kyereli_dummy ./game_test_kyereli dummy)
add_test(test_kyereli_game_default ./game_test_kyereli game_default)
add_test(test_kyereli_game_default_solution ./game_test_kyereli game_default_solution)
add_test(test_kyereli_game_get_adjacent_square ./game_test_kyereli game_get_adjacent_square)
add_test(test_kyereli_game_has_half_edge ./game_test_kyereli game_has_half_edge)
add_test(test_kyereli_game_check_edge ./game_test_kyereli game_check_edge)
add_test(test_kyereli_game_is_well_paired ./game_test_kyereli game_is_well_paired)
add_test(test_kyereli_game_is_connected ./game_test_kyereli game_is_connected)
add_test(test_kyereli_game_nb_rows ./game_test_kyereli game_nb_rows)
add_test(test_kyereli_game_nb_cols ./game_test_kyereli game_nb_cols)
add_test(test_kyereli_game_is_wrapping ./game_test_kyereli game_is_wrapping)
add_test(test_kyereli_game_load ./game_test_kyereli game_load)

add_test(test_elhaddiallo_dummy ./game_test_elhaddiallo dummy)
add_test(test_elhaddiallo_game_new_empty ./game_test_elhaddiallo game_new_empty)
add_test(test_elhaddiallo_game_new ./game_test_elhaddiallo game_new)
add_test(test_elhaddiallo_game_copy ./game_test_elhaddiallo game_copy)
add_test(test_elhaddiallo_game_equal ./game_test_elhaddiallo game_equal)
add_test(test_elhaddiallo_game_delete ./game_test_elhaddiallo game_delete)
add_test(test_elhaddiallo_game_set_piece_shape ./game_test_elhaddiallo game_set_piece_shape)
add_test(test_elhaddiallo_game_set_piece_orientation ./game_test_elhaddiallo game_set_piece_orientation)
add_test(test_elhaddiallo_game_new_ext ./game_test_elhaddiallo game_new_ext)
add_test(test_elhaddiallo_game_new_empty_ext ./game_test_elhaddiallo game_new_empty_ext)
add_test(test_elhaddiallo_game_save ./game_test_elhaddiallo game_save)

add_test(test_atuzun_dummy ./game_test_atuzun dummy)
add_test(test_atuzun_game_get_piece_shape ./game_test_atuzun game_get_piece_shape)
add_test(test_atuzun_game_get_piece_orientation ./game_test_atuzun game_get_piece_orientation)
add_test(test_atuzun_game_play_move ./game_test_atuzun game_play_move)
add_test(test_atuzun_game_won ./game_test_atuzun game_won)
add_test(test_atuzun_game_reset_orientation ./game_test_atuzun game_reset_orientation)
add_test(test_atuzun_game_shuffle_orientation ./game_test_atuzun game_shuffle_orientation)
add_test(test_atuzun_game_print ./game_test_atuzun game_print)
add_test(test_atuzun_game_undo ./game_test_atuzun game_undo)
add_test(test_atuzun_game_redo ./game_test_atuzun game_redo)
