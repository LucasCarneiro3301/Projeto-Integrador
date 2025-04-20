/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#include "is_inside.h"
#include "calc_intensity.h"
#include "../config/config.h"

// Atualiza os níveis dos LEDs com base na posição do visor
void update_led_color(uint8_t visor_left, uint8_t visor_top, uint8_t box, uint8_t rect_left, uint8_t rect_top, uint8_t rect_width, uint8_t rect_height, uint8_t tol_left, uint8_t tol_top, uint8_t tol_width, uint8_t tol_height) {
    uint8_t center_x = rect_left + rect_width / 2;
    uint8_t center_y = rect_top + rect_height / 2;

    uint8_t max_radius_x = rect_width / 2;
    uint8_t max_radius_y = rect_height / 2;
    int max_dist_squared = max_radius_x * max_radius_x + max_radius_y * max_radius_y;

    bool in_operation = is_inside(visor_left, visor_top, rect_left, rect_top, rect_width, rect_height, box);
    bool in_tolerance = is_inside(visor_left, visor_top, tol_left, tol_top, tol_width, tol_height, box);

    if (in_operation) {         // Faixa de operação: Níveis de intensidade do LED verde
        uint16_t level = calc_intensity(visor_left, visor_top, box, center_x, center_y, max_dist_squared);
        pwm_set_gpio_level(GREEN, level);
        pwm_set_gpio_level(RED, 0);
        pwm_set_gpio_level(BLUE, 0);
    }
    else if (in_tolerance) {    // Faixa de tolerância: LED amarelo
        pwm_set_gpio_level(GREEN, 4095 / 4);
        pwm_set_gpio_level(RED, 4095 / 4);
        pwm_set_gpio_level(BLUE, 0);
    }
    else {                      // Fora dos limites: LED vermelho
        pwm_set_gpio_level(RED, 4095 / 4);
        pwm_set_gpio_level(GREEN, 0);
        pwm_set_gpio_level(BLUE, 0);
    }
}