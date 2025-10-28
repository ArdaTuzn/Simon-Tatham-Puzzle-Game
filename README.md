This project is a version of the Simon Tatham puzzle game developed by a team of three during our second year in the Computer Science department, as part of the Technological Project course.

The original files were hosted on our university's GitLab platform. I've transferred them to my personal GitHub repository to ensure long-term access and preservation. I just translated the comments from French to English.

While working on this project, we gained hands-on experience in:

-Effective use of the Git platform for group collaboration
-Using build tools such as CMake and Makefile
-Writing and organizing test functions
-Designing and implementing core game algorithms
-Debugging and troubleshooting code
-Reviewing and writing reports based on another group's project
-Reading and writing files using C libraries,
-Designing a graphical user interface (GUI) for the puzzle using the SDL library
-Creating a web interface using HTML, CSS, and JavaScript technologies

Game Logic:
Net is a single-player logic puzzle. It involves a 5x5 grid filled with "cable-like" pieces. Each piece represents a segment of a network (like wires or pipes) that must be connected correctly.

The goal is to rotate each piece so that all pieces together form one large connected network — that is, a connected graph. In the version you're using, loops are allowed, and empty squares are permitted.

Puzzle Mechanics
Each square on the grid contains a piece that may be:
-An endpoint (1 connection)
-A segment (2 opposite connections)
-A corner (2 adjacent connections) 
-A tee (3 connections)

Each piece has a direction (North, East, South, West) and can be rotated in 90° increments.

These connections are called half-edges. When two half-edges from adjacent squares face each other, they form a valid connection (edge).

Rules
Objective:
Rotate the pieces so that all valid connections (edges) form a single connected structure (connected graph).

In our version:
Loops are allowed.
Empty squares are allowed.

Winning condition:
All valid half-edges are matched correctly, and all non-empty pieces are part of the same connected graph.

When checking if two adjacent squares connect properly:
Both have half-edges ->	MATCH
Only one has a half-edge ->	MISMATCH
Neither has a half-edge	-> NOEDGE

Terminal Representations:
Shapes:	endpoint, segment, corner, tee	  
Direction: North, East, South, West
Respectfully: Shapes,Direction:
^, >, v, <
|, -, |, -
└, ┌, ┐, ┘
┴, ├, ┬, ┤

We also added a shape called "Cross" which has the representation: + 

Project Architecture

The project consists of a C library libgame.a split across:

game.h – basic game functions
game_aux.h – helper utilities
game_ext.h – advanced functions

It also includes several programs:

game_text – play in the terminal
game_test – run unit tests
game_solve – auto-solve puzzles
game_sdl – GUI version using SDL

Error Handling
Library functions assume valid arguments; input validation is done at a higher level. To catch errors during development, the assert() macro is used. Assertions are removed in RELEASE mode for performance.

