/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#include "lib/config/config.h"
#include "lib/utils/map_and_scale.h"
#include "lib/utils/draw_visor.h"
#include "lib/utils/update_led_color.h"
#include "lib/utils/values_position.h"
#include "lib/utils/calc_temp_and_lum.h"

uint8_t mode = 0;           // Modos de operação (Idle, Cold, Relax, Work, Guest)

uint8_t rect_width = 128;   // Largura de operação
uint8_t rect_height = 64;   // Altura de operação
uint8_t left = 0;           // Distância à esquerda da da faixa de operação
uint8_t top  = 0;           // Distância do topo da da faixa de operação

uint8_t tol_width = 128;    // Largura de tolerância
uint8_t tol_height = 64;    // Altura de tolerância
uint8_t tol_left = 0;       // Distância à esquerda da da faixa de tolerância
uint8_t tol_top = 0;        // Distância do topo da da faixa de tolerância

uint8_t box  = 8;           // Tamanho do visor (1 ou 8)

uint8_t res = 3;            // Resolução (1x1, 2x2, 4x4 ou 8x8)
bool reset = false;         // True para modo de gravação

static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

void gpio_irq_handler(uint gpio, uint32_t events);

// Retorna a leitura de um determinado canal ADC
uint16_t select_adc_channel(unsigned short int channel) {
    adc_select_input(channel);
    return adc_read();
}

int main() {
    float avg_temp, avg_lum;
    char str[16];  // Buffer para armazenar a string
    ssd1306_t ssd;
    bool color = true;
    
    config(&ssd);
    
    gpio_set_irq_enabled_with_callback(BTNJ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão do Joystick
    gpio_set_irq_enabled_with_callback(BTNA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão A
    gpio_set_irq_enabled_with_callback(BTNB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão B

    uint slice_blue = pwm_gpio_to_slice_num(BLUE);  // Slice PWM do pino 12
    uint slice_red = pwm_gpio_to_slice_num(RED);    // Slice PWM do pino 13

    while (true) {
        if(!reset) { 
            uint16_t x = select_adc_channel(1);
            uint16_t y = select_adc_channel(0);

            uint8_t display_x = map_and_scale(x, X_MIN, X_MAX, 0, DISPLAY_WIDTH - box);
            uint8_t display_y = map_and_scale(y, Y_MIN, Y_MAX, DISPLAY_HEIGHT - box, 0);
    
            ssd1306_fill(&ssd, !color);                                                     // Limpa ou mostra a tela
            ssd1306_rect(&ssd, top, left, rect_width, rect_height, color, !color);          // Faixa de operação
            ssd1306_rect(&ssd, tol_top, tol_left, tol_width, tol_height, color, !color);    // Faixa de tolerância

            draw_visor(display_x,display_y,box,color,&ssd);
    
            update_led_color(display_x, display_y, box, left, top, rect_width, rect_height, tol_left, tol_top, tol_width, tol_height);

            calc_temp_and_lum(display_x,display_y,box,&avg_temp,&avg_lum);

            values_position(&ssd, avg_temp, avg_lum, mode);

            ssd1306_send_data(&ssd);    // Atualiza o display 
        } else {
            printf("Saindo para o modo de gravação...\n\n");

            ssd1306_fill(&ssd, false);  // Limpa a tela
            ssd1306_draw_string(&ssd, "MODO DE", 28, 28); 
            ssd1306_draw_string(&ssd, "GRAVACAO", 24, 40);
            ssd1306_send_data(&ssd);    // Envia os dados para o display

            reset_usb_boot(0,0);    // Sai para o modo de gravação
        }
    }
}

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Obtém o tempo atual em microssegundos
  
    if (current_time - last_time > 2e5) { // 200 ms de debouncing
        last_time = current_time; 
        if(gpio == BTNA) {
            mode = (mode + 1)%5;
            if (mode==0) {          // Modo Idle
                rect_width = 128;   // 0 px à 128 px = 50 lux à 1100 lux    (128/1050=0.122 px/lux)
                rect_height = 64;   // 0 px à 64 px = 14° à 26°             (64/13=4.92 px/C°)
                left = 0;           // 0 px = 50 lux min
                top = 0;            // 64 px = 27° max

            }
            else if(mode==1) {      // Modo Cold
                rect_width = 37;    // (400-100)*0.122 = 36.6 px
                rect_height = 10;   // (18-16)*4.92 = 9.84 px
                left = 6;           // (100-50)*0.122 = 6.1 px
                top = 44;           // 64 - (18-14)*4.92 = 44.32 px
            }
            else if(mode==2) {      // Modo Relax
                rect_width = 37;    // (400-100)*0.122 = 36.6 px
                rect_height = 10;   // (20-18)*4.92 = 9.84 px
                left = 6;           // (100-50)*0.122 - 0 = 6.1 px
                top = 35;           // 64 - (20-14)*4.92 = 34.48 px
            }
            else if(mode==3) {      // Modo Work
                rect_width = 49;    // (1000-600)*0.122 = 48.8 px
                rect_height = 25;   // (25-20)*4.92 = 24.6 px
                left = 67;          // (600-50)*0.122 - 0 = 67.1 px
                top = 10;           // 64 - (25-14)*4.92 = 9.88 px
            }
            else if(mode==4) {      // Modo Guest
                rect_width = 49;    // (700-300)*0.122 = 48.8 px
                rect_height = 25;   // (25-20)*4.92 = 24.6 px
                left = 31;          // (300-50)*0.122 - 0 = 30.5 px
                top = 10;           // 64 - (25-14)*4.92 = 9.88 px
            }

            tol_width = (!mode) ? rect_width : rect_width + 6;      // Largura de tolerância
            tol_height = (!mode) ? rect_height : rect_height + 6;   // Altura de tolerância
            tol_left = (!mode) ? left : left - 3;
            tol_top = (!mode) ? top : top - 3;
        }
        else if(gpio == BTNB) {
            res = (res + 1)%4;
            box = pow(2, res);
        } 
        else if(gpio == BTNJ) {
            reset = true;
        }
    }
}



