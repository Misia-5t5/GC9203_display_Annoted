#ifndef _GC9203_DISPLAY_H_
#define _GC9203_DISPLAY_H_

// Define some colors

#define BLACK   0x0000
#define GRAY    0x8410
#define WHITE   0xFFFF
#define RED     0xF800
#define ORANGE  0xFA60
#define YELLOW  0xFFE0  
#define LIME    0x07FF
#define GREEN   0x07E0
#define CYAN    0x07FF
#define AQUA    0x04FF
#define BLUE    0x001F
#define MAGENTA 0xF81F
#define PINK    0xF8FF

#include <Adafruit_GFX.h>

class GC9203_Display : public Adafruit_GFX {
  public:
    GC9203_Display(int8_t ss, int8_t rst, int8_t scl, int8_t sdi, int8_t dc, uint8_t use_horizontal);

    void begin();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillScreen(uint16_t color);
    void clearScreen(uint16_t color);
    void LCD_SetPos(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void printNew(const long value, const int chCount);
    void printNew(const float value, const unsigned decimals, const int chCount);
    void printNew(const String &newString, const String &oldString); 

  private:
    int8_t _ss, _rst, _scl, _sdi, _dc;
    uint8_t USE_HORIZONTAL;

    void Write_Cmd(uint8_t cmd);
    void Write_Data(uint8_t data);
    void Write_Data_U16(uint8_t DH, uint8_t DL);
    void Write_Data_16(uint16_t data);
    void Write_Three_Bytes(uint8_t byte1, uint8_t byte2, uint8_t byte3);

    void getNumberBounds(const int chCount);
    int16_t x, y, OriginalCursor_x, OriginalCursor_y;
    uint16_t w, h, charWidth, char2Width;
};

#endif

