#ifndef GLCD_H
#define GLCD_H

void glcd_init(void);

void clear_screen(void);

// Plotting functions.
void setpixel(uint8_t *buf, uint8_t x, uint8_t y, uint8_t color);

void drawrect(uint8_t *buf,
              uint8_t x, uint8_t y, uint8_t w, uint8_t h,
              uint8_t color);
void drawline(uint8_t *buf,
              uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
              uint8_t color);

void drawbitmap(uint8_t *buf, uint8_t x, uint8_t y,
                const uint8_t bitmap, uint8_t w, uint8_t h,
                uint8_t color);
void drawchar(uint8_t *buf, uint8_t x, uint8_t line, uint8_t c);
void drawcircle(uint8_t *buf,
                uint8_t x0, uint8_t y0, uint8_t r,
                uint8_t color);

void fillrect(uint8_t *buf,
              uint8_t x, uint8_t y, uint8_t w, uint8_t h,
              uint8_t color);

#endif
