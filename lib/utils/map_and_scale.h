/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#ifndef MAP_AND_SCALE_H
#define MAP_AND_SCALE_H

/**
 * @brief Realiza o mapeamento e escalonamento de um valor de uma faixa de entrada para uma faixa de saída.
 *
 * Esta função é útil quando se deseja converter, por exemplo, uma leitura de ADC (que varia de 0 a 4095)
 * para coordenadas de tela (como de 0 a 127 em X ou 0 a 63 em Y), respeitando os limites das faixas.
 *
 * @param x         Valor a ser mapeado.
 * @param in_min    Valor mínimo da faixa de entrada.
 * @param in_max    Valor máximo da faixa de entrada.
 * @param out_min   Valor mínimo da faixa de saída.
 * @param out_max   Valor máximo da faixa de saída.
 * @return          Valor escalonado na nova faixa, já limitado aos extremos de entrada.
 */

uint8_t map_and_scale(int x, int in_min, int in_max, int out_min, int out_max) {
    if (x < in_min) x = in_min;
    if (x > in_max) x = in_max;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif