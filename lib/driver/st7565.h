#ifndef ST7565
#define ST7565

#define LCD_WIDTH 128
#define LCD_HEIGHT 64

// Pinouts.
#ifdef SPI
	#define SID_DDR DDRB
	#define SID_PIN PINB
	#define SID_PORT PORTB
	#define SID 1

	#define SCLK_DDR DDRB
	#define SCLK_PIN PINB
	#define SCLK_PORT PORTB
	#define SCLK 0
#elif
	#define DATA_DDR  DDRB
	#define DATA_PORT  DDRB
#endif

#define A0_DDR DDRD
#define A0_PIN PIND
#define A0_PORT PORTD
#define A0 7

#define RST_DDR DDRD
#define RST_PIN PIND
#define RST_PORT PORTD
#define RST 6

#define CS_DDR DDRD
#define CS_PIN PIND
#define CS_PORT PORTD
#define CS 5

// Basic control.
void st7565_init(void);
void st7565_clear_screen(void);

// Buffer control.
void st7565_clear_buffer(uint8_t *buffer);
void st7565_write_buffer(uint8_t *buffer);

// Communications.
void st7656_send_command(uint8_t c);
void st7565_send_data(uint8_t c);

// Low-level controls.
#ifdef SPI
void spi_write(uint8_t c);
#elif
void parallel_write(uint8_t c);
#endif

// Additional functions.
void st7565_set_brightness(uint8_t val);

// Commands.
#define CMD_DISPLAY_OFF 0xAE
#define CMD_DISPLAY_ON 0xAF

#define CMD_SET_DISP_START_LINE 0x40
#define CMD_SET_PAGE 0xB0

#define CMD_SET_COLUMN_UPPER 0x10
#define CMD_SET_COLUMN_LOWER 0x00

#define CMD_SET_ADC_NORMAL 0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON 0xA5
#define CMD_SET_BIAS_9 0xA2
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW 0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET 0xE2
#define CMD_SET_COM_NORMAL 0xC0
#define CMD_SET_COM_REVERSE 0xC8
#define CMD_SET_POWER_CONTROL 0x28
#define CMD_SET_RESISTOR_RATIO 0x20
#define CMD_SET_VOLUME_FIRST 0x81
#define  CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF 0xAC
#define  CMD_SET_STATIC_ON 0xAD
#define CMD_SET_STATIC_REG 0x0
#define CMD_SET_BOOSTER_FIRST 0xF8
#define CMD_SET_BOOSTER_234  0
#define  CMD_SET_BOOSTER_5  1
#define  CMD_SET_BOOSTER_6  3
#define CMD_NOP 0xE3
#define CMD_TEST 0xF0

#endif
