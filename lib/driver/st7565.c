#include "st7565.h"

void st7565_init(void)
{

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

void st7565_clear_buffer(uint8_t *buf);
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
			st7565_data(buffer[(128*p)+c]);
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
