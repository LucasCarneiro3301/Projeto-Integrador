/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#include "../config/config.h"


// Ajusta a posição dos valores de temperatura e luminosidade conforme a posição do cursor na tela e o modo de operação atual
void values_position(ssd1306_t *ssd, float avg_temp, float avg_lum, uint8_t mode) {
    char temp[10], lum[10];

    sprintf(temp, "%.2f", avg_temp);
    sprintf(lum, (avg_lum >= 1e3) ? "%.1f" : "%.2f", avg_lum);

    if(mode==0) {
        ssd1306_draw_string(ssd,temp, 3, avg_temp >= 23 ? (64-7-8) : 7);
        ssd1306_draw_string(ssd,lum, 77, avg_temp >= 23 ? (64-7-8) : 7);
    } 
    else if(mode==1 || mode ==2) {
        ssd1306_draw_string(ssd,temp, avg_temp >= 23 ?  85 : 3, avg_temp >= 23 ? (64-7-19) : 7);
        ssd1306_draw_string(ssd,lum,  77, avg_temp >= 23 ? (64-7-8) : 7);
    }
    else if(mode==3) {
        ssd1306_draw_string(ssd,temp, 3, avg_temp < 17 ? 7 : (64-7-8));
        ssd1306_draw_string(ssd,lum, avg_temp < 17 ? 3 : 77, avg_temp < 17 ? 20 : (64-7-8));
    }
    else if(mode==4) {
        ssd1306_draw_string(ssd,temp, avg_temp < 17 && (avg_lum < 426 || avg_lum > 740) ? 87 : 3, avg_temp < 17 && (avg_lum < 426 || avg_lum > 740) ? (64-7-19) :  (64-7-8));
        ssd1306_draw_string(ssd,lum, avg_temp < 17 && avg_lum > 740 ? 3 : (avg_lum < 100) ? 87 : (avg_lum < 1000) ? 79 : 77, (64-7-8));
    }
}