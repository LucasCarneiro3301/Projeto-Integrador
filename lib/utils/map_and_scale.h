/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#ifndef MAP_AND_SCALE_H
#define MAP_AND_SCALE_H

uint8_t map_and_scale(int x, int in_min, int in_max, int out_min, int out_max) {
    if (x < in_min) x = in_min;
    if (x > in_max) x = in_max;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif