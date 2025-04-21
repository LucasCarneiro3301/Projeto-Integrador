/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

// Converte pixel para C° ou lux
float map_range_float(float value, float in_min, float in_max, float out_min, float out_max) {
    if (value < in_min) value = in_min;
    if (value > in_max) value = in_max;
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Calcula a temperatura e luminosidade média
void calc_temp_and_lum(uint8_t base_x, uint8_t base_y, uint8_t box, float *avg_temp, float *avg_lum) {
    float total_temp = 0.0f;
    float total_lum = 0.0f;

    // Número total de pixels
    uint8_t num_pixels = box*box;

    for (uint8_t dy = 0; dy < box; dy++) {
        for (uint8_t dx = 0; dx < box; dx++) {
            uint8_t px = base_x + dx;
            uint8_t py = base_y + dy;

            // Calculando a temperatura (invertendo o eixo Y, topo = 27°C, fundo = 14°C)
            float temp = map_range_float(py, 0, 63, 27.0f, 14.0f);

            // Calculando a luminosidade (mapeando X para lux)
            float lum = map_range_float(px, 0, 127, 50.0f, 1100.0f);

            total_temp += temp;
            total_lum += lum;
        }
    }

    *avg_temp = total_temp / num_pixels;
    *avg_lum = total_lum / num_pixels;
}