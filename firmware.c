/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/irq.h" 
#include "hardware/pwm.h" 
#include "pico/bootrom.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

// Pinagem dos componentes
#define I2C_PORT i2c1   // Define que o barramento I2C usado será o "i2c1"
#define I2C_SDA 14      // Define que o pino GPIO 14 será usado como SDA (linha de dados do I2C)
#define I2C_SCL 15      // Define que o pino GPIO 15 será usado como SCL (linha de clock do I2C)
#define address 0x3C    // Define o endereço I2C do dispositivo (0x3C é o endereço padrão de muitos displays OLED SSD1306)
#define JOY_Y 26        // Eixo Y do joystick 
#define JOY_X 27        // Eixo X do joystick 
#define BTNJ 22         // Botão do joystick
#define BTNA 5          // Botão A
#define BTNB 6          // Botão B
#define RED 13          // LED vermelho
#define BLUE 12         // LED azul
#define GREEN 11        // LED verde

// Parâmetros do PWM 
/*
    fpwm = fckl / (di * (wrap + 1)), fpwm = 1KHz (Tpwm = 1ms)
    wrap = 4095, normalizar com os nívels ADC
    1KHz = 125Mhz/(div*(4095 + 1))
    1KHz = 30.52KHz/div
    1KHz*div = 30.52KHz
    div = 30.52KHz/1KHz
    div = 30.52
*/
#define WRAP 4095 // Wrap
#define DIV 30.52 // Divisor inteiro

#define PWM_MAX 4095

#define DISPLAY_WIDTH 128   // Largura do display
#define DISPLAY_HEIGHT 64   // Altura do display

// Valores calibrados
#define X_MIN 18
#define X_MAX 4084
#define Y_MIN 595 
#define Y_MAX 3350


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

bool res = true;            // Resolução (1x1 ou 8x8)
bool reset = false;         // True para modo de gravação

static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

void setup(); // Prototipação da função que define os LEDs RGB como saídas e os botões como entradas
void i2c_setup();
void pwm_setup();
void ssd1306_setup(ssd1306_t* ssd);
void gpio_irq_handler(uint gpio, uint32_t events);
void adc_setup();

// Retorna a leitura de um determinado canal ADC
uint16_t select_adc_channel(unsigned short int channel) {
    adc_select_input(channel);
    return adc_read();
}

int map_range(int x, int in_min, int in_max, int out_min, int out_max) {
    if (x < in_min) x = in_min;
    if (x > in_max) x = in_max;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Verifica se o visor está dentro de um retângulo
bool is_inside(int visor_left, int visor_top, int rect_left, int rect_top, int rect_width, int rect_height) {
    return 
        visor_left >= rect_left &&
        visor_left + box <= rect_left + rect_width &&
        visor_top >= rect_top &&
        visor_top + box <= rect_top + rect_height;
}

// Calcula intensidade verde com base na distância ao centro da faixa de operação
int calculate_intensity(int visor_left, int visor_top, int box, int center_x, int center_y, int max_dist_squared) {
    int visor_center_x = visor_left + box / 2;
    int visor_center_y = visor_top + box / 2;
    int dx = visor_center_x - center_x;
    int dy = visor_center_y - center_y;
    int dist_sq = dx * dx + dy * dy;

    float normalized = (float)dist_sq / (float)max_dist_squared;

    if (normalized < 0.05f)
        return PWM_MAX * 0.5f;      // 100%
    else if (normalized < 0.15f)
        return PWM_MAX * 0.375f;    // 75%
    else if (normalized < 0.30f)
        return PWM_MAX * 0.25f;     // 50%
    else if (normalized < 0.50f)
        return PWM_MAX * 0.125f;    // 25%
    else if (normalized < 1.0f)
        return PWM_MAX * 0.0625f;   // 10%

    return 0;
}

// Atualiza os níveis dos LEDs com base na posição do visor
void update_led_color(int visor_left, int visor_top, int box, int rect_left, int rect_top, int rect_width, int rect_height, int tol_left, int tol_top, int tol_width, int tol_height) {
    int center_x = rect_left + rect_width / 2;
    int center_y = rect_top + rect_height / 2;

    int max_radius_x = rect_width / 2;
    int max_radius_y = rect_height / 2;
    int max_dist_squared = max_radius_x * max_radius_x + max_radius_y * max_radius_y;

    bool in_operation = is_inside(visor_left, visor_top, rect_left, rect_top, rect_width, rect_height);
    bool in_tolerance = is_inside(visor_left, visor_top, tol_left, tol_top, tol_width, tol_height);

    if (in_operation) {         // Faixa de operação: Níveis de intensidade do LED verde
        int level = calculate_intensity(visor_left, visor_top, box, center_x, center_y, max_dist_squared);
        pwm_set_gpio_level(GREEN, level);
        pwm_set_gpio_level(RED, 0);
        pwm_set_gpio_level(BLUE, 0);
    }
    else if (in_tolerance) {    // Faixa de tolerância: LED amarelo
        pwm_set_gpio_level(GREEN, PWM_MAX / 4);
        pwm_set_gpio_level(RED, PWM_MAX / 4);
        pwm_set_gpio_level(BLUE, 0);
    }
    else {                      // Fora dos limites: LED vermelho
        pwm_set_gpio_level(RED, PWM_MAX / 4);
        pwm_set_gpio_level(GREEN, 0);
        pwm_set_gpio_level(BLUE, 0);
    }
}


int main() {
    ssd1306_t ssd;
    bool color = true;
    
    stdio_init_all();       // Inicialização dos recursos de entrada e saída padrão
    adc_setup();            // Inicialização e configuração dos pinos ADC
    setup();                // Inicialização e configuração dos LEDs e botões 
    i2c_setup();            // Inicialização e configuração da comunicação serial I2C 
    pwm_setup();            // Inicialização e configuração do PWM
    ssd1306_setup(&ssd);    // Inicializa a estrutura do display
    
    gpio_set_irq_enabled_with_callback(BTNJ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão do Joystick
    gpio_set_irq_enabled_with_callback(BTNA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão A
    gpio_set_irq_enabled_with_callback(BTNB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão B

    uint slice_blue = pwm_gpio_to_slice_num(BLUE);  // Slice PWM do pino 12
    uint slice_red = pwm_gpio_to_slice_num(RED);    // Slice PWM do pino 13

    while (true) {
        if(!reset) {
            uint16_t x = select_adc_channel(1);
            uint16_t y = select_adc_channel(0);

            uint8_t display_x = map_range(x, X_MIN, X_MAX, 0, DISPLAY_WIDTH - box);
            uint8_t display_y = map_range(y, Y_MIN, Y_MAX, DISPLAY_HEIGHT - box, 0);
    
            ssd1306_fill(&ssd, !color);                                                     // Limpa ou mostra a tela
            ssd1306_rect(&ssd, top, left, rect_width, rect_height, color, !color);          // Faixa de operação
            ssd1306_rect(&ssd, tol_top, tol_left, tol_width, tol_height, color, !color);    // Faixa de tolerância

            if(res)
                ssd1306_draw_string(&ssd, "#", display_x, display_y);   // Movimenta o quadrado ao longo da tela   
            else
                ssd1306_pixel(&ssd, display_x, display_y, color);       // Movimenta o pixel ao longo da tela

            ssd1306_send_data(&ssd);    // Atualiza o display 
    
            update_led_color(display_x, display_y, box, left, top, rect_width, rect_height, tol_left, tol_top, tol_width, tol_height);

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

// Inicializa e configura os LEDs RGB como saída. Inicializa e configura os botões como entradas.
void setup() {
    gpio_init(BTNA);
    gpio_set_dir(BTNA, GPIO_IN);
    gpio_pull_up(BTNA);  
  
    gpio_init(BTNB);
    gpio_set_dir(BTNB, GPIO_IN);
    gpio_pull_up(BTNB);  

    gpio_init(BTNJ);
    gpio_set_dir(BTNJ, GPIO_IN);
    gpio_pull_up(BTNJ); 
}

// Inicializa e configura os pinos do joystick como periféricos ADC
void adc_setup() {
    adc_init();
    adc_gpio_init(JOY_Y);
    adc_gpio_init(JOY_X);
}

// Inicializa e configura a comunicação serial I2C 
void i2c_setup() {
    i2c_init(I2C_PORT, 4e2 * 1e3); // Inicialização I2C.
  
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define a função do pino GPIO para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define a função do pino GPIO para I2C
    gpio_pull_up(I2C_SDA); // Pull up para linha de dados
    gpio_pull_up(I2C_SCL); // Pull up para linha de clock
}

// Limpa o display
void clear(ssd1306_t* ssd) {
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

// Inicializa e configura o display
void ssd1306_setup(ssd1306_t* ssd) {
    ssd1306_init(ssd, WIDTH, HEIGHT, false, address, I2C_PORT); 
    ssd1306_config(ssd); 
    ssd1306_send_data(ssd);
    clear(ssd);

  
    ssd1306_draw_char(ssd, '#', 28, 60);  
    ssd1306_send_data(ssd);
}

// Inicializa e configura os pinos 13 e 12 como PWM
void pwm_setup() {
    gpio_set_function(GREEN, GPIO_FUNC_PWM); // Define o pino como PWM
    gpio_set_function(RED, GPIO_FUNC_PWM); // Define o pino como PWM

    uint slice_blue = pwm_gpio_to_slice_num(GREEN); // Obtém o slice
    uint slice_red = pwm_gpio_to_slice_num(RED); // Obtém o slice
    
    pwm_set_clkdiv(slice_blue, DIV); // Define o divisor inteiro de clock
    pwm_set_wrap(slice_blue, WRAP); // Define o wrap
    pwm_set_enabled(slice_blue, true);

    pwm_set_clkdiv(slice_red, DIV); // Define o divisor inteiro de clock
    pwm_set_wrap(slice_red, WRAP); // Define o wrap
    pwm_set_enabled(slice_red, true);
    
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
            res = !res;
            box = (res) ? 8 : 1;
        } 
        else if(gpio == BTNJ) {
            reset = true;
        }
    }
}



