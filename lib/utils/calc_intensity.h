/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

// Calcula intensidade verde com base na distância ao centro da faixa de operação
uint16_t calc_intensity(uint8_t visor_left, uint8_t visor_top, uint8_t box, uint8_t center_x, uint8_t center_y, int max_dist_squared) {
    int visor_center_x = visor_left + box / 2;
    int visor_center_y = visor_top + box / 2;
    int dx = visor_center_x - center_x;
    int dy = visor_center_y - center_y;
    int dist_sq = dx * dx + dy * dy;

    float normalized = (float)dist_sq / (float)max_dist_squared;

    if (normalized < 0.05f)
        return 4095 * 0.5f;      // 100%
    else if (normalized < 0.15f)
        return 4095 * 0.375f;    // 75%
    else if (normalized < 0.30f)
        return 4095 * 0.25f;     // 50%
    else if (normalized < 0.50f)
        return 4095 * 0.125f;    // 25%
    else if (normalized < 1.0f)
        return 4095 * 0.0625f;   // 10%

    return 0;
}