#include "st7565.h"

void st7565_init(void)
{
	// Set pin directions.
	#ifdef SPI
	SID_DDR |= _BV(SID);
	SCLK_DDR |= _BV(SCLK);
	#elif
	DATA_DDR |= _BV(0xFF);
	#endif
	A0_DDR |= _BV(A0);
	RST_DDR |= _BV(RST);
	CS_DDR |= _BV(CS);

	// Manually initiate the chip selection.
	CS_PORT &= ~_BV(CS);
	RST_PORT &= ~_BV(RST);
	_delay_ms(500);
	RST_PORT |= _BV(RST);

	// LCD bias select.
	st7565_command(CMD_SET_BIAS_7);
	// ADC select.
	st7565_command(CMD_SET_ADC_NORMAL);
	// SHL select.
	st7565_command(CMD_SET_COM_NORMAL);
	// Initial display line.
	st7565_command(CMD_SET_DISP_START_LINE);

	// Turn on voltage converter (VC=1, VR=0, VF=0)..
	st7565_command(CMD_SET_POWER_CONTROL | 0x4);
	// Wait for 50% rising.
	_delay_ms(50);

	// turn on voltage regulator (VC=1, VR=1, VF=0).
	st7565_command(CMD_SET_POWER_CONTROL | 0x6);
	// Wait >=50ms.
	_delay_ms(50);

	// Turn on voltage follower (VC=1, VR=1, VF=1).
	st7565_command(CMD_SET_POWER_CONTROL | 0x7);
	// Short wait for the command to complete.
	_delay_ms(10);

	// Set LCD operating voltage (regulator resistor, ref voltage resistor)
	st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);
}

void st7565_clear_screen(void)
{
	uint8_t p, c;

	for (p = 0; p < 8; p++)
	{
		st7565_command(CMD_SET_PAGE | p);
		for (c = 0; c < 129; c++)
		{
			st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
			st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
			st7565_data(0x0);
		}
	}
}

void st7565_clear_buffer(uint8_t *buf)
{

}

void st7565_write_buffer(uint8_t *buf)
{
	uint8_t c, p;

	for (p = 0; p < 8; p++)
	{
		st7565_command(CMD_SET_PAGE | pagemap[p]);
		st7565_command(CMD_SET_COLUMN_LOWER | (0x0 & 0xf));
		st7565_command(CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));
		st7565_command(CMD_RMW);
		st7565_data(0xff);

		for (c = 0; c < 128; c++)
			st7565_data(buf[(128*p)+c]);
	}
}

// Communications.
void st7656_send_command(uint8_t c)
{
	A0_PORT &= ~_BV(A0);
	#ifdef SPI
	st7565_spi_write(c);
	#elif
	st7565_parallel_write(c);
	#endif
}

void st7565_send_data(uint8_t c)
{
	A0_PORT |= _BV(A0);
	#ifdef SPI
	st7565_spi_write(c);
	#elif
	st7565_parallel_write(c);
	#endif
}

#ifdef SPI
inline void st7565_spi_write(uint8_t c)
{
	int8_t i;
	for (i=7; i>=0; i--)
	{
		SCLK_PORT &= ~_BV(SCLK);
		if (c & _BV(i))
			SID_PORT |= _BV(SID);
		else
			SID_PORT &= ~_BV(SID);
		SCLK_PORT |= _BV(SCLK);
	}
}
#elif
void st7565_parallel_write(uint8_t c)
{

}
#endif

void st7565_set_brightness(uint8_t val)
{
	st7565_command(CMD_SET_VOLUME_FIRST);
	st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}
