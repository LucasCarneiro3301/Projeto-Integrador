/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#include "../config/config.h"

void draw_visor(uint8_t x, uint8_t y, uint8_t box, bool color, ssd1306_t *ssd) {
    for (uint8_t dx = 0; dx < box; dx++) {
        for (uint8_t dy = 0; dy < box; dy++) {
            ssd1306_pixel(ssd, x + dx, y + dy, color);
        }
    }
}
