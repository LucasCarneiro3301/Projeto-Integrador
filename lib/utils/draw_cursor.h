/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#include "../config/config.h"

// Desenha o cursor na tela conforme x e y e a resolução (1x1,2x2,4x4 e 8x8)
void draw_cursor(uint8_t x, uint8_t y, uint8_t box, bool color, ssd1306_t *ssd) {
    for (uint8_t dx = 0; dx < box; dx++) {
        for (uint8_t dy = 0; dy < box; dy++) {
            ssd1306_pixel(ssd, x + dx, y + dy, color);
        }
    }
}
