// Bibliotecas utilizadas no programa
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"


// Definições dos pinos
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define PIN_WS2812 7
#define NUM_PIXELS 25
#define BUTTON_A 5
#define BUTTON_B 6
#define BUZZER 21


// Variáveis globais para controlar o display, debouncing dos botões, notificações e intensidade dos LEDs da matriz (entre 0 e 255 para intensidade)
ssd1306_t ssd;
static volatile uint32_t last_time = 0;
static volatile uint8_t authorize = 0;
static volatile uint8_t notification_number = 3;
static volatile uint8_t notification_control;
static volatile bool release_button_b = false;
uint8_t led_r = 255;
uint8_t led_g = 255;
uint8_t led_b = 255;


// Matriz para armazenar os símbolos que serão exibidos
bool symbols[25][NUM_PIXELS] = {
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de intruso - ❌ (LEDs vermelhos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 1, 0, 0, // Símbolo de intruso - ❌ (LEDs vermelhos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 1, 0, 1, 0, 
  0, 0, 1, 0, 0, // Símbolo de intruso - ❌ (LEDs vermelhos)
  0, 1, 0, 1, 0, 
  0, 0, 0, 0, 0
  },
  {
  1, 0, 0, 0, 1, 
  0, 1, 0, 1, 0, 
  0, 0, 1, 0, 0, // Símbolo de intruso - ❌ (LEDs vermelhos)
  0, 1, 0, 1, 0, 
  1, 0, 0, 0, 1
  },
  {
  0, 0, 0, 0, 0, 
  0, 1, 0, 1, 0, 
  0, 0, 1, 0, 0, // Símbolo de intruso - ❌ (LEDs vermelhos)
  0, 1, 0, 1, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 1, 0, 0, // Símbolo de intruso - ❌ (LEDs vermelhos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de intruso - ❌ (LEDs vermelhos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de não autorizado - ⚠️ (LEDs amarelos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 1, 0, 0, // Símbolo de não autorizado - ⚠️ (LEDs amarelos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 1, 1, 1, 0, 
  0, 0, 1, 0, 0, // Símbolo de não autorizado - ⚠️ (LEDs amarelos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  1, 1, 1, 1, 1, 
  0, 1, 1, 1, 0, // Símbolo de não autorizado - ⚠️ (LEDs amarelos)
  0, 0, 1, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 1, 1, 1, 0, 
  0, 0, 1, 0, 0, // Símbolo de não autorizado - ⚠️ (LEDs amarelos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 1, 0, 0, // Símbolo de não autorizado - ⚠️ (LEDs amarelos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de não autorizado - ⚠️ (LEDs amarelos)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  1, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 1, 0, 
  1, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 1, 0, 
  1, 0, 1, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 1, 0, 
  1, 0, 1, 0, 0, 
  0, 1, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 1, 0, 
  1, 0, 1, 0, 0, 
  0, 1, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 1, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 1, 0, 
  1, 0, 1, 0, 0, 
  0, 1, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 1, 0, 
  1, 0, 1, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 1, 0, 
  1, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  1, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, // Símbolo de autorizado - ✅ (LEDs verdes)
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
  }
};


// Prototipação das rotinas
void initialize_all();
void gpio_irq_handler(uint gpio, uint32_t events);
static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void set_one_led(uint8_t r, uint8_t g, uint8_t b, uint8_t initial_matrix, uint8_t final_matrix);
int64_t turn_off_callback(alarm_id_t id, void *user_data);
void notify();
void play_sound(int type);
void play_tone(uint16_t frequency, uint16_t duration);


// Rotina principal
int main()
{
  // Inicialização e configuração dos pinos e periféricos
  initialize_all();

  // Configuração da interrupção com callback para os botões A e B
  gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); 

  while (1)
  {
    // Chama a rotina de exibição das notificações
    notify();
    sleep_ms(50);
  }
}


// Inicializa e configura os pinos e periféricos que serão utilizados no programa
void initialize_all()
{
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Inicializa o pino do botão A e define a direção como entrada, com pull-up
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);
  
  // Inicializa o pino do botão B e define a direção como entrada, com pull-up
  gpio_init(BUTTON_B);
  gpio_set_dir(BUTTON_B, GPIO_IN);
  gpio_pull_up(BUTTON_B);

  // Inicializa o pino do buzzer A e define a direção como saída
  gpio_init(BUZZER);
  gpio_set_dir(BUZZER, GPIO_OUT);

  // Inicializa o PIO e configura o estado da máquina e o programa WS2812
  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);

  // Configura o programa WS2812
  ws2812_program_init(pio, sm, offset, PIN_WS2812, 800000, false);
}

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
  // Obtém o tempo atual em microssegundos
  uint32_t current_time = to_us_since_boot(get_absolute_time());
    
  // Verifica se passou tempo suficiente desde o último evento
  if (current_time - last_time > 200000) // 200 ms de debouncing
  {
    last_time = current_time; // Atualiza o tempo do último evento

    // Botão A (sensor de presença)
    if (gpio == 5)
    {
      // Agenda um alarme para 3 segundos (3000 ms)
      // A função 'turn_off_callback' será chamada após esse tempo
      add_alarm_in_ms(3000, turn_off_callback, NULL, false);
      release_button_b = true;
    }
    // Botão B (identificação da pessoa)
    else if (gpio == 6 && authorize >= 0 && authorize < 2 && release_button_b)
    {
      authorize++; 
    }
  }
}

// Envia o valor do pixel para a máquina de estado do PIO
static inline void put_pixel(uint32_t pixel_grb)
{
  pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Converte os valores RGB para um único valor de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Define a cor de um LED específico na matriz
void set_one_led(uint8_t r, uint8_t g, uint8_t b, uint8_t initial_matrix, uint8_t final_matrix)
{
  // Define a cor com base nos parâmetros fornecidos
  uint32_t color = urgb_u32(r, g, b);
  
  // Percorre todos os LEDs e define a cor com base no valor da matriz symbols
  // Exibe a animação 3 vezes
  for (uint8_t repetition = 0; repetition < 3; repetition++)
  {
    for (uint8_t i = initial_matrix; i < final_matrix; i++)
    {
      for (uint8_t j = 0; j < NUM_PIXELS; j++)
      {
        if (symbols[i][j])
        {
          put_pixel(color);
        }
        else
        {
          put_pixel(0);
        }
      }
      sleep_ms(150);
    }
  }
}

// Função de callback para definir a notificação que será exibida após o tempo programado
int64_t turn_off_callback(alarm_id_t id, void *user_data)
{
  if (authorize == 0)
  {
    notification_number = 0;
  }
  else if (authorize == 1)
  {
    notification_number = 1;
  }
  else if (authorize == 2)
  {
    notification_number = 2;
  }

  // Retorna 0 para indicar que o alarme não deve se repetir
  return 0;
}

// Rotina para exibir os símbolos na matriz 5x5, mensagens no display e sinais sonoros via buzzer
void notify()
{
  // Notificação para intruso
  if (notification_number == 0)
  {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Intruso", 35, 0);
    ssd1306_draw_string(&ssd, "Detectado", 30, 15);
    ssd1306_draw_string(&ssd, "Acionando", 30, 35);
    ssd1306_draw_string(&ssd, "Seguranca", 30, 50);
    ssd1306_send_data(&ssd);
    play_sound(0);
    set_one_led(led_r, 0, 0, 1, 7);
    authorize = 0;
    notification_number = 3;
    release_button_b = false;
  }
  // Notificação para pessoa não autorizada
  else if (notification_number == 1)
  {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Pessoa", 45, 10);
    ssd1306_draw_string(&ssd, "Nao", 55, 30);
    ssd1306_draw_string(&ssd, "Autorizada", 30, 50);
    ssd1306_send_data(&ssd);
    play_sound(1);
    set_one_led(led_r, led_g, 0, 8, 14);
    authorize = 0;
    notification_number = 3;
    release_button_b = false;
  }
  // Notificação para pessoa autorizada
  else if (notification_number == 2)
  {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Pessoa", 40, 20);
    ssd1306_draw_string(&ssd, "Autorizada", 25, 40);
    ssd1306_send_data(&ssd);
    play_sound(2);
    set_one_led(0, led_g, 0, 15, 25);
    authorize = 0;
    notification_number = 3;
    release_button_b = false;
  }
  else
  {
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    set_one_led(0, 0, 0, 0, 0);
  }
}

// Rotina para tocar um som com base no tipo fornecido
void play_sound(int type)
{
  if (type == 0)
  {
    // Sinal intruso - Alterna entre duas frequências lentamente
    uint16_t duration = 2000; // Duração total de 2 segundos
    uint16_t tone_duration = 250; // Duração de cada tom

    uint32_t low_frequency = 440;  // Frequência baixa em Hz
    uint32_t high_frequency = 880; // Frequência alta em Hz

    for (uint16_t i = 0; i < duration; i += (2 * tone_duration))
    {
      play_tone(low_frequency, tone_duration);
      play_tone(high_frequency, tone_duration);
    }
  }
  else if (type == 1)
  {
    // Sinal não autorizado - Alterna entre duas frequências rápidas
    for (int i = 0; i < 3; i++) { // Repetir 3 vezes
      play_tone(1000, 100); // 1000 Hz por 100 ms
      play_tone(1500, 100); // 1500 Hz por 100 ms
    }
  }
  else if (type == 2)
  {
    // Sinal autorizado - Uma única frequência curta
    play_tone(1000, 200); // 1000 Hz por 200 ms
  }
}

// Rotina para tocar um tom específico usando o buzzer
void play_tone(uint16_t frequency, uint16_t duration)
{
  uint32_t period = 1000000 / frequency; // Período em microssegundos
  uint32_t half_period = period / 2;     // Metade do período

  for (uint16_t i = 0; i < (duration * 1000) / period; i++)
  {
    gpio_put(BUZZER, 1);     // Liga o buzzer
    sleep_us(half_period);   // Espera metade do período
    gpio_put(BUZZER, 0);     // Desliga o buzzer
    sleep_us(half_period);   // Espera metade do período
  }
}
