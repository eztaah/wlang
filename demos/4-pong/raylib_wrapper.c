#include "raylib.h"
#include <stdint.h>  // Pour utiliser les types d'entiers de largeur fixe

// Convertit un entier en une couleur Raylib (assume que l'entier est formaté comme 0xAARRGGBB)
// Color int_to_color(uint64_t color) {
//     Color col;
//     col.a = (color >> 24) & 0xFF;
//     col.r = (color >> 16) & 0xFF;
//     col.g = (color >> 8) & 0xFF;
//     col.b = color & 0xFF;
//     return col;
// }

// Redéfinitions des fonctions Raylib avec des entiers sur 64 bits

// void clear_background(Color color) {
//     ClearBackground(color);
// }

// void begin_drawing(void) {
//     BeginDrawing();
// }

// void draw_rectangle(int64_t posX, int64_t posY, int64_t width, int64_t height, Color color) {
//     DrawRectangle((int)posX, (int)posY, (int)width, (int)height, color);
// }

// void draw_text(char* text_addr, int64_t posX, int64_t posY, int64_t fontSize, Color color) {
//     DrawText(text_addr, (int)posX, (int)posY, (int)fontSize, color);
// }

// void end_drawing(void) {
//     EndDrawing();
// }

// void init_window(int64_t width, int64_t height, char* title) {
//     InitWindow((int)width, (int)height, title);
// }

// int64_t window_should_close(void) {
//     return (int64_t)WindowShouldClose();
// }

int64_t is_key_down(int64_t key) {
    return (int64_t)IsKeyDown((int)key);
}

// void close_window(void) {
//     CloseWindow();
// }
