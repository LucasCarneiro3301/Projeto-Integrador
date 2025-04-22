# Projeto-Integrador
Este projeto tem como objetivo aplicar e consolidar os conhecimentos adquiridos sobre o microcontrolador RP2040 utilizando exclusivamente os recursos disponíveis na placa de desenvolvimento BitDogLab.

__Aluno:__
Lucas Carneiro de Araújo Lima

## ATIVIDADE 

__Para este trabalho, os seguintes componentes e ferramentas se fazem necessários:__
1) Microcontrolador Raspberry Pi Pico W.
2) Ambiente de trabalho VSCode.
3) LEDs RGB
4) _Display_ SSD1306
5) 3 Botões Pull-Up
6) _Joystick_
7) Ferramenta educacional BitDogLab.
8) Matriz de LEDs 5x5
9) Buzzer

__O resultado do projeto pode ser assistido através deste link: [Vídeo de Apresentação - Projeto Integrador](https://youtu.be/kbyInoCkLzw?si=E-TNbJHGJnNWX8GX).__

## Instruções de Uso

### 1. Clone o repositório
Abra o terminal e execute o comando abaixo para clonar o repositório em sua máquina:
```bash
git clone https://github.com/LucasCarneiro3301/Projeto-Integrador.git
```

### 2. Configure o ambiente de desenvolvimento
Certifique-se de que o [SDK do Raspberry Pi Pico](https://github.com/raspberrypi/pico-sdk) esteja instalado e configurado corretamente no seu sistema.

### 3. Conexão com a Rapberry Pico
1. Conecte o Raspberry Pi Pico ao seu computador via USB.
2. Inicie o modo de gravação pressionando o botão **BOOTSEL** e **RESTART**.
3. O Pico será montado como um dispositivo de armazenamento USB.
4. Execute através do comando **RUN** a fim de copiar o arquivo `firmware.uf2` para o Pico.
5. O Pico reiniciará automaticamente e executará o programa.

### 4. Observações (IMPORTANTE !!!)
2. Manuseie a placa com cuidado.

## Recursos e Funcionalidades

### 1. Potenciômetro do Joystick
Usado como entrada analógica (ADC) para controlar a posição do cursor no display OLED. Os eixos X e Y simulam sensores de luminosidade e temperatura.

### 2. Botões

| BOTÃO                            | DESCRIÇÃO                                     | 
|:----------------------------------:|:---------------------------------------------:|
| A                                  | Botão de Modo de Confirmação                 | 
| B                                  | Botão de Resolução             | 
| JOYSTICK                                  | Modo de Gravação              | 

### 3. Display OLED
O display OLED simula um gráfico temperatura × luminosidade, onde um cursor móvel representa as condições do ambiente. As escalas de conversão utilizadas são aproximadamente 0.122 pixels por lux e 4.92 pixels por grau Celsius. Além do cursor, o display também apresenta faixas de operação e tolerância, atualizadas dinamicamente conforme a posição do cursor.

### 4. Matriz de LEDs (5x5)
Usada como alerta visual do estado do sistema (erro, sucesso, advertência).

### 5. LED RGB

| COR                            | DESCRIÇÃO                                     | 
|:----------------------------------:|:---------------------------------------------:|
| VERMELHO                                  | ERRO                 | 
| AMARELO                           | ADVERTÊNCIA             | 
| VERDE (variável)                                 | Proporcional à precisão da posição do cursor em relação ao centro da faixa operacional.              | 

### 6. Buzzer
Gera um som constante em caso de erro como alerta sonoro.

### 7. Interrupções
Detectam pressionamento dos botões de forma assíncrona, sem a necessidade de polling contínuo. Ativadas na borda de descida dos sinais.

### 8. Tratamento de Debounce
Evita múltiplas leituras acidentais dos botões usando um tempo mínimo de 200 ms entre leituras consecutivas.

### 9. Comunicação Serial
Envia mensagens do sistema via USB para o terminal do computador. Útil para depuração, exibição de modos de operação, resolução do cursor e estados do sistema em tempo real
