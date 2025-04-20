/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

// Verifica se o visor está dentro de um retângulo
bool is_inside(uint8_t visor_left, uint8_t visor_top, uint8_t rect_left, uint8_t rect_top, uint8_t rect_width, uint8_t rect_height, uint8_t box) {
    return 
        visor_left >= rect_left &&
        visor_left + box <= rect_left + rect_width &&
        visor_top >= rect_top &&
        visor_top + box <= rect_top + rect_height;
}