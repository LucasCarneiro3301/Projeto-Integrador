/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/irq.h" 
#include "hardware/pwm.h" 
#include "pico/bootrom.h"
#include "ws2812.pio.h"
#include "../ssd1306/ssd1306.h"
#include "../ssd1306/font.h"

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
#define BUZZER 21 
#define WS2812_PIN 7 // Matriz de LEDs 5x5

#define NUM_PIXELS 25 // 5x5 = 25
#define IS_RGBW false

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

#define DISPLAY_WIDTH 128   // Largura do display
#define DISPLAY_HEIGHT 64   // Altura do display

// Valores calibrados
#define X_MIN 18
#define X_MAX 4084
#define Y_MIN 595 
#define Y_MAX 3350

PIO pio = pio0;
int sm = 0;

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
    gpio_set_function(BUZZER, GPIO_FUNC_PWM); // Define o pino como PWM

    uint slice_blue = pwm_gpio_to_slice_num(GREEN); // Obtém o slice
    uint slice_red = pwm_gpio_to_slice_num(RED); // Obtém o slice
    uint slice_buzzer = pwm_gpio_to_slice_num(BUZZER); // Obtém o slice
    
    pwm_set_clkdiv(slice_blue, DIV); // Define o divisor inteiro de clock
    pwm_set_wrap(slice_blue, WRAP); // Define o wrap
    pwm_set_enabled(slice_blue, true);

    pwm_set_clkdiv(slice_red, DIV); // Define o divisor inteiro de clock
    pwm_set_wrap(slice_red, WRAP); // Define o wrap
    pwm_set_enabled(slice_red, true);

    pwm_set_clkdiv(slice_buzzer, DIV); // Define o divisor inteiro de clock
    pwm_set_wrap(slice_buzzer, WRAP); // Define o wrap
    pwm_set_enabled(slice_buzzer, true);
}

// Inicializa e configura a matriz de LEDs 5x5 
void ws2812_setup(PIO pio, uint sm) {
    uint offset = pio_add_program(pio, &ws2812_program);
  
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW); //Inicializa a matriz de leds
}

void config(ssd1306_t *ssd) {
    stdio_init_all();       // Inicialização dos recursos de entrada e saída padrão
    adc_setup();            // Inicialização e configuração dos pinos ADC
    setup();                // Inicialização e configuração dos LEDs e botões 
    i2c_setup();            // Inicialização e configuração da comunicação serial I2C 
    pwm_setup();            // Inicialização e configuração do PWM
    ssd1306_setup(ssd);    // Inicializa a estrutura do display
    ws2812_setup(pio, sm);
}

#endif