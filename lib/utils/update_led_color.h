/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#include "../config/config.h"

// Verifica se o visor está dentro dos limites de operação ou tolerância
bool is_valid(float avg_temp, float avg_lum, uint8_t mode, float temp_tol, float lum_tol) {
    if(mode==0)
        return true;
    else if(mode==1 && (avg_temp >= 16-temp_tol && avg_temp <= 18+temp_tol) && (avg_lum >= 100-lum_tol && avg_lum <= 400+lum_tol))
        return true;
    else if(mode==2 && (avg_temp >= 18-temp_tol && avg_temp <= 20+temp_tol) && (avg_lum >= 100-lum_tol && avg_lum <= 400+lum_tol))
        return true;
    else if(mode==3 && (avg_temp >= 20-temp_tol && avg_temp <= 25+temp_tol) && (avg_lum >= 600-lum_tol && avg_lum <= 1000+lum_tol))
        return true;
    else if(mode==4 && (avg_temp >= 20-temp_tol && avg_temp <= 25+temp_tol) && (avg_lum >= 300-lum_tol && avg_lum <= 700+lum_tol))
        return true;
    else
        return false;
}

// Calcula intensidade com base na distância da média ao ponto ideal do modo (centro da faixa de operação)
uint16_t calc_intensity(float avg_temp, float avg_lum, uint8_t mode) {
    float ideal_temp, ideal_lum;

    switch (mode) {
        case 0: ideal_temp = 20.5f; ideal_lum = 525.0f; break;
        case 1: ideal_temp = 17.0f; ideal_lum = 250.0f; break;
        case 2: ideal_temp = 19.0f; ideal_lum = 250.0f; break;
        case 3: ideal_temp = 22.5f; ideal_lum = 800.0f; break;
        case 4: ideal_temp = 22.5f; ideal_lum = 500.0f; break;
        default: return 0; 
    }

    // Normaliza as diferenças
    float dt = (avg_temp - ideal_temp) / 13.0f;    // temp varia de 14 a 27
    float dl = (avg_lum - ideal_lum) / 1050.0f;    // lux varia de 50 a 1100

    // Distância normalizada
    float dist = sqrtf(dt * dt + dl * dl);

    // Faixas normalizadas (0.0 a ~1.0)
    if (dist < 0.05f)
        return 4095 / 2;       
    else if (dist < 0.1f)
        return 4095 / 4;
    else if (dist < 0.2f)
        return 4095 / 8;
    else if (dist < 0.3f)
        return 4095 / 16;
    else if (dist < 0.4f)
        return 4095 / 32;
    else if (dist < 0.6f)
        return 4095 / 64;
    else if (dist < 0.8f)
        return 4095 / 128;

    return 0;
}



// Atualiza os níveis dos LEDs com base na posição do visor
void update_led_color(float avg_temp, float avg_lum, uint8_t mode) {
    bool in_operation = is_valid(avg_temp,avg_lum,mode,0,0);
    bool in_tolerance = is_valid(avg_temp,avg_lum,mode,3/4.92,3/0.122);

    if (in_operation) {         // Faixa de operação: Níveis de intensidade do LED verde
        uint16_t level = calc_intensity(avg_temp, avg_lum, mode);
        pwm_set_gpio_level(GREEN, level/2);
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