#include "glcd.h"

#ifdef ST7565
#include "driver/st7565.h"
#elif
#error No supported controller defined.
#endif

void glcd_init(void)
{
	st7565_init();
}

void clear_screen(void)
{
	st7565_clear_screen();
}
