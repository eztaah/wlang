#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned char U8;
typedef unsigned int U32;
typedef int I32;
typedef int B32;
typedef float F32;
typedef char Char;
typedef void Void;

#define true 1
#define false 0

#define GRID_LENGTH 9

typedef enum {
    TILE_CROSS, 
    TILE_CIRCLE, 
    TILE_EMPTY
} Tile;

typedef enum {
    STATE_PLAYING,
    STATE_WIN, 
    STATE_DRAW
} State;

typedef enum {
    HUMAN, 
    AI
} Player;

void panic(const Char* file_path, I32 line, const Char* message)
{
    printf("%s:%d: APPLICATION PANIC: %s\n", file_path, line, message);
    abort();
}

#define UNREACHABLE() panic(__FILE__, __LINE__, "unreachable")

static Char tile_to_char(Tile tile)
{
    switch (tile) {
        case TILE_CROSS:
            return 'X';
        case TILE_CIRCLE:
            return 'O';
        case TILE_EMPTY:
            return ' ';
        default:
            UNREACHABLE();
            return ' ';
    }
}

static Void init_grid(Tile* grid) 
{
    for (I32 i = 0; i < GRID_LENGTH; i++) {
        grid[i] = TILE_EMPTY;
    }
}

static Void display_grid(const Tile* grid) 
{
    for (I32 i = 0; i < 3; i++) {
        for (I32 j = 0; j < 3; j++) {
            printf("[%c]", tile_to_char(grid[3*i + j]));
        }
        printf("\n");
    }
}

static State update_game_state(const Tile* grid) {
    // Vérification des lignes, des colonnes et des diagonales
    for (I32 i = 0; i < 3; i++) {
        if (grid[3*i + 0] == grid[3*i + 1] && grid[3*i + 1] == grid[3*i + 2] && grid[3*i + 2] != TILE_EMPTY) {
            return STATE_WIN;
        }
        if (grid[0 + i] == grid[3*1 + i] && grid[3*1 + i] == grid[3*2 + i] && grid[3*0 + i] != TILE_EMPTY) {
            return STATE_WIN;
        }
    }
    if (grid[3*0 + 0] == grid[3*1 + 1] && grid[3*1 + 1] == grid[3*2 + 2] && grid[3*0 + 0] != TILE_EMPTY) { 
        return STATE_WIN;
    }
    if (grid[3*0 + 2] == grid[3*1 + 1] && grid[3*1 + 1] == grid[3*2 + 0] && grid[3*0 + 2] != TILE_EMPTY) {
        return STATE_WIN;
    }

    // Vérification si la grid est pleine
    for (I32 i = 0; i < GRID_LENGTH; i++) {
        if (grid[i] == TILE_EMPTY) {
            return STATE_PLAYING;
        }
    }

    return STATE_DRAW;
}

static Void play_human_turn(Tile* grid) 
{
    I32 x;
    I32 y;

    do {
        printf("Enter your move (row column): ");
        scanf("%d %d", &x, &y);
        // Reduce the number because the player gives a position between 1 and 3, but the game logic is between 0 and 2
        x--; 
        y--;
    } 
    while (x < 0 || x > 2 || y < 0 || y > 2 || grid[3*x + y] != TILE_EMPTY);

    grid[3*x + y] = TILE_CROSS;
}

static Void play_ia_turn(Tile* grid) 
{
    I32 x;
    I32 y;

    do {
        x = rand() % 3;
        y = rand() % 3;
    } 
    while (grid[3*x + y] != TILE_EMPTY);

    grid[3*x + y] = TILE_CIRCLE;
}

I32 main(Void) {
    srand(time(NULL));

    Tile grid[GRID_LENGTH];
    Player current_player = HUMAN;
    State state = STATE_PLAYING;

    init_grid(grid);
    display_grid(grid);

    while (state == STATE_PLAYING) {
        if (current_player == HUMAN) {
            play_human_turn(grid);
            current_player = AI;
        } 
        else {
            play_ia_turn(grid);
            current_player = HUMAN;
            display_grid(grid);
        }
        state = update_game_state(grid);
    }

    // The game is either win of draw
    display_grid(grid);
    if (state == STATE_WIN) {
        if (current_player == HUMAN) {
            printf("End of game! AI wins!\n");
        } 
        else {
            printf("End of game! Human wins!\n");
        }
    } 
    else if (state == STATE_DRAW) {
        printf("It's a draw!\n");
    } 
    else { 
        UNREACHABLE(); 
    }

    return 0;
}
