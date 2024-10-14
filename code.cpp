#include "GC9203_Display.h"
#include <SPI.h>

GC9203_Display::GC9203_Display(int8_t ss, int8_t rst, int8_t scl, int8_t sdi, int8_t dc, uint8_t use_horizontal)
  : Adafruit_GFX((use_horizontal % 2 == 0) ? 220 : 128, (use_horizontal % 2 == 0) ? 128 : 220),// 这是一个对Adafruit_GFX基类图形库的初始化
  // 根据use_horizontal参数的值来设置显示屏的宽度和高度。
    _ss(ss), _rst(rst), _scl(scl), _sdi(sdi), _dc(dc), USE_HORIZONTAL(use_horizontal) {
  pinMode(_ss, OUTPUT);
  pinMode(_rst, OUTPUT);
  pinMode(_scl, OUTPUT);
  pinMode(_sdi, OUTPUT);
  pinMode(_dc, OUTPUT);
}


void GC9203_Display::Write_Cmd(uint8_t cmd) {
  unsigned char i;
 /** void说明这个函数不返回任何值。
  GC9203_Display::：这是类名和作用域解析运算符（::）。它表示 Write_Cmd 是 GC9203_Display 类的一个成员函数。
  Write_Cmd：这是函数的名称。
  uint8_t cmd：这是函数的参数列表。uint8_t 是一个无符号8位整数类型，表示这个函数接受一个8位的信号作为输入参数。cmd 是这个参数的名字。***/
  digitalWrite(_ss, LOW);  // 拉低片选信号，选择当前设备
  digitalWrite(_dc, LOW);  // 拉低数据/命令选择引脚，表示接下来发送的是命令
  ## digitalWrite 是 Arduino 平台中的一个内置函数，用于设置数字引脚的值为高电平（HIGH）或低电平（LOW）。
  for (i = 0; i < 8; i++) {
    digitalWrite(_scl, LOW);
    if (cmd & 0x80) digitalWrite(_sdi, HIGH);
    else digitalWrite(_sdi, LOW);
    digitalWrite(_scl, HIGH);
    cmd = cmd << 1;
  }
  digitalWrite(_ss, HIGH);
}

void GC9203_Display::Write_Data(uint8_t data) {
 uint8_t m, n;
  m = data >> 8;
  n = data;
  Write_Data_U16(m, n);
}

void GC9203_Display::Write_Data_U16(uint8_t DH, uint8_t DL) {

  uint8_t i;
  digitalWrite(_ss, LOW);
 digitalWrite(_dc, HIGH);
  for (i = 0; i < 8; i++) {
   digitalWrite(_scl, LOW);
    if (DH & 0x80)digitalWrite(_sdi, HIGH);
    else digitalWrite(_sdi, LOW);
     digitalWrite(_scl, HIGH);
    DH = DH << 1;
  }
  digitalWrite(_ss, HIGH);
  digitalWrite(_ss, LOW);
  for (i = 0; i < 8; i++) {
    digitalWrite(_scl, LOW);
    if (DL & 0x80) digitalWrite(_sdi, HIGH);
    else digitalWrite(_sdi, LOW);
    digitalWrite(_scl, HIGH);
    DL = DL << 1;
  }
  digitalWrite(_ss, HIGH);
}

void GC9203_Display::Write_Three_Bytes(uint8_t byte1, uint8_t byte2, uint8_t byte3) {
    uint8_t i;

    // Assert chip select line (SS low)
    digitalWrite(_ss, LOW);
    digitalWrite(_dc, HIGH);
    // Send first byte (byte1)
    for (i = 0; i < 8; i++) {
        digitalWrite(_scl, LOW);
        if (byte1 & 0x80) digitalWrite(_sdi, HIGH);
        else digitalWrite(_sdi, LOW);
        digitalWrite(_scl, HIGH);
        byte1 <<= 1;
    }

    // Send second byte (byte2)
    for (i = 0; i < 8; i++) {
        digitalWrite(_scl, LOW);
        if (byte2 & 0x80) digitalWrite(_sdi, HIGH);
        else digitalWrite(_sdi, LOW);
        digitalWrite(_scl, HIGH);
        byte2 <<= 1;
    }

    // Send third byte (byte3)
    for (i = 0; i < 8; i++) {
        digitalWrite(_scl, LOW);
        if (byte3 & 0x80) digitalWrite(_sdi, HIGH);
        else digitalWrite(_sdi, LOW);
        digitalWrite(_scl, HIGH);
        byte3 <<= 1;
    }

    // De-assert chip select line (SS high)
    digitalWrite(_ss, HIGH);
}

void GC9203_Display::Write_Data_16(uint16_t data) {

unsigned char i;
   digitalWrite(_ss, LOW);
  digitalWrite(_dc, HIGH);
  for (i = 0; i < 15; i++) {
   digitalWrite(_scl, LOW);
    if (data & 0x80) digitalWrite(_sdi, HIGH);
    else digitalWrite(_sdi, LOW);
    digitalWrite(_scl, HIGH);
    data = data << 1;
  }
    digitalWrite(_ss,HIGH);
}

void GC9203_Display::begin() {
  digitalWrite(_ss, HIGH);
  delay(5);
  digitalWrite(_rst, LOW);  // Reset the display
  delay(10);
  digitalWrite(_rst, HIGH);
  delay(120);

  Write_Cmd(0xff);
  Write_Data_U16(0x5a, 0xa5);

  Write_Cmd(0xf6);
  Write_Data_U16(0x01, 0x12);

  Write_Cmd(0xef);
  Write_Data_U16(0x10, 0x52); // 1052

  Write_Cmd(0xa5);
  Write_Data_U16(0x07, 0x80);

  Write_Cmd(0x02);
  Write_Data_U16(0x01, 0x00);

  // Write_Cmd(0x03);
  // Write_Data_U16(0x10, 0x00);   // cambiato da 0x10, 0x00 a  0x1, 0x00

   Write_Cmd(0x03);
if(USE_HORIZONTAL == 0)
  Write_Data_U16(0x12, 0x08);
else if(USE_HORIZONTAL == 1)
  Write_Data_U16(0x12, 0x38);
else if(USE_HORIZONTAL == 2)
  Write_Data_U16(0x12, 0x20);
else if(USE_HORIZONTAL == 3)
  Write_Data_U16(0x12, 0x10);


  Write_Cmd(0x01);
  Write_Data_U16(0x01, 0x1c);   // cambiato da 0x03, 0x1c a 0x00, 0x1c

  /**************************************************************************/
  Write_Cmd(0x05);
  Write_Data_U16(0x00, 0x01);  //262K color 1pixel/transition

  /*************************************************************************/

  Write_Cmd(0xf6);
  Write_Data_U16(0x01, 0x12);

  Write_Cmd(0x11);
  Write_Data_U16(0x10, 0x00); // 0d1f 1630  // vreg1b[13:8]_vreg1a_[5:0]     122B   3319  0D1F

  Write_Cmd(0xEB);
  Write_Data_U16(0x0d, 0x1f); // 160a  // vreg2b[13:8]_vreg2a_[5:0]     1606   2334  0528

  Write_Cmd(0xEC);
  Write_Data_U16(0x05, 0x28);

  Write_Cmd(0x50);
  Write_Data_U16(0xf3, 0x80); // H:V0(7-4) V13(3-0)   L:V63(3-0)

  Write_Cmd(0x51);
  Write_Data_U16(0x11, 0x0c); // H:V61(5-0)   L:V62(5-0)

  Write_Cmd(0x52);
  Write_Data_U16(0x09, 0x09); // H:V57(4-0)  L:V59(4-0)

  Write_Cmd(0x53);
  Write_Data_U16(0x37, 0x06); // H:V43(6-0)  L:V50(3-0)

  Write_Cmd(0x54);
  Write_Data_U16(0x4e, 0x1f); // H:V20(6-0)   L:V27(5-3) V36(2-0)  4e27

  Write_Cmd(0x55);
  Write_Data_U16(0x12, 0x12); // H:V4(4-0)   L:V6(4-0)

  Write_Cmd(0x56);
  Write_Data_U16(0x37, 0x36); // H:V1(5-0)   L:V2(5-0)

  Write_Cmd(0x57);
  Write_Data_U16(0xfc, 0x80); // H:V0(7-4) V13(3-0)   L:V63(3-0)

  Write_Cmd(0x58);
  Write_Data_U16(0x11, 0x0c); // H:V61(5-0)   L:V62(5-0)

  Write_Cmd(0x59);
  Write_Data_U16(0x09, 0x09); // H:V57(4-0)  L:V59(4-0)

  Write_Cmd(0x5A);
  Write_Data_U16(0x37, 0x06); // H:V43(6-0)  L:V50(3-0)

  Write_Cmd(0x5B);
  Write_Data_U16(0x4e, 0x19); // H:V20(6-0)   L:V27(5-3) V36(2-0)  4e21

  Write_Cmd(0x5C);
  Write_Data_U16(0x12, 0x12); // H:V4(4-0)   L:V6(4-0)

  Write_Cmd(0x5D);
  Write_Data_U16(0x37, 0x36); // H:V1(5-0)   L:V2(5-0)

   Write_Cmd(0x07);
  Write_Data_U16(0x10, 0x13);

  Write_Cmd(0xfe);
}

void GC9203_Display::LCD_SetPos(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) { // Use uint16_t
  Write_Cmd(0x03);
  if (USE_HORIZONTAL == 0)
    Write_Data_U16(0x12, 0x08);
  else if (USE_HORIZONTAL == 1)
    Write_Data_U16(0x12, 0x38);
  else if (USE_HORIZONTAL == 2)
    Write_Data_U16(0x12, 0x20);
  else if (USE_HORIZONTAL == 3)
    Write_Data_U16(0x12, 0x10);

  if (USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1) {
    Write_Cmd(0x37);       //Horizontal Start
    Write_Data(x1 + 24);
    Write_Cmd(0x36);      //Horizontal End
    Write_Data(x2 + 24);
    Write_Cmd(0x39);      //Vertical Start
    Write_Data(y1);
    Write_Cmd(0x38);      //Vertical End
    Write_Data(y2);
    Write_Cmd(0x22);
  } else {
    Write_Cmd(0x39);
    Write_Data(x1);
    Write_Cmd(0x38);
    Write_Data(x2);
    Write_Cmd(0x37);
    Write_Data(y1 + 24);
    Write_Cmd(0x36);
    Write_Data(y2 + 24);
    Write_Cmd(0x22);
  }
}

// Helper function to reverse the bits in an 8-bit number
uint8_t reverseBits(uint8_t n) {
    uint8_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result <<= 1;
        result |= (n & 1);
        n >>= 1;
    }
    return result;
}

void GC9203_Display::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

    LCD_SetPos(x, y, x, y);

    // Extract RGB components from 16-bit color (RGB565 format)
    uint8_t r = (color >> 11) & 0x1F; // Extract the 5 bits for red
    uint8_t g = (color >> 5) & 0x3F;  // Extract the 6 bits for green
    uint8_t b = color & 0x1F;         // Extract the 5 bits for blue

    // Convert to 6-bit values (scale from 5-bit to 6-bit for red and blue)
    r = (r << 1) | (r >> 4);  // Scale red 5 bits to 6 bits
    b = (b << 1) | (b >> 4);  // Scale blue 5 bits to 6 bits

    // Format the color data according to the GC9203's requirements
    uint8_t red_byte = (r << 2);   // 6 Red bits + 2 dummy bits
    uint8_t green_byte = (g << 2); // 6 Green bits + 2 dummy bits
    uint8_t blue_byte = (b << 2);  // 6 Blue bits + 2 dummy bits

    // Reverse the bits in each color byte if needed
    // red_byte = reverseBits(red_byte);
    // green_byte = reverseBits(green_byte);
    // blue_byte = reverseBits(blue_byte);

    // Use the new function to write the three color bytes
    Write_Three_Bytes(red_byte, green_byte, blue_byte);
}


void GC9203_Display::fillScreen(uint16_t color) {
clearScreen(color);
}
  void GC9203_Display::clearScreen(uint16_t color){
    uint8_t r = (color >> 11) & 0x1F; // Extract the 5 bits for red
    uint8_t g = (color >> 5) & 0x3F;  // Extract the 6 bits for green
    uint8_t b = color & 0x1F;         // Extract the 5 bits for blue

    // Convert to 6-bit values (scale from 5-bit to 6-bit for red and blue)
    r = (r << 1) | (r >> 4);  // Scale red 5 bits to 6 bits
    b = (b << 1) | (b >> 4);  // Scale blue 5 bits to 6 bits

    // Format the color data according to the GC9203's requirements
    uint8_t red_byte = (r << 2);   // 6 Red bits + 2 dummy bits
    uint8_t green_byte = (g << 2); // 6 Green bits + 2 dummy bits
    uint8_t blue_byte = (b << 2);  // 6 Blue bits + 2 dummy bits

  uint8_t i, j;
  if ((USE_HORIZONTAL == 0) || (USE_HORIZONTAL == 1))
    LCD_SetPos(0, 0, 128, 220);
    for (i = 0; i < 128; i++) {
    for (j = 0; j < 220; j++)
      Write_Three_Bytes(red_byte, green_byte, blue_byte);
  }
  if ((USE_HORIZONTAL == 2) || (USE_HORIZONTAL == 3))
    LCD_SetPos(0, 0, 220, 128);
  for (i = 0; i < 220; i++) {
    for (j = 0; j < 128; j++)
      Write_Three_Bytes(red_byte, green_byte, blue_byte);
  }
}

void GC9203_Display::printNew(const long value, const int chCount)
{ getNumberBounds(chCount);
  print(value); 
  setCursor(OriginalCursor_x, OriginalCursor_y);  
}

void GC9203_Display::printNew(const float value, const unsigned decimals, const int chCount)
{ getNumberBounds(chCount);
  print(value, decimals); 
  setCursor(OriginalCursor_x, OriginalCursor_y);  
}

void GC9203_Display::getNumberBounds(const int chCount)
{ OriginalCursor_x = cursor_x; 
  OriginalCursor_y = cursor_y;
  getTextBounds("3",  OriginalCursor_x, OriginalCursor_y, &x, &y, &charWidth, &h); // THIS CODE IS UGLY 
  getTextBounds("33", OriginalCursor_x, OriginalCursor_y, &x, &y, &char2Width, &h); // font of 3 is wider than 0
  int space = char2Width-2*charWidth;
  w = chCount*charWidth + (chCount-1)*space;
  // Serial << x, y, w, h; // 0: 22 64 22 34 // 00: 22 64 48 34
  // textbgcolor = ILI9341_GREEN; // testing with green background
  fillRect(x, y, w, h, textbgcolor); // textbgcolor is protected in Adafruit_GFX.h 
}

void GC9203_Display::printNew(const String &newString, const String &oldString) // overloading needs const here
{ OriginalCursor_x = cursor_x;
  OriginalCursor_y = cursor_y;
  getTextBounds(oldString.c_str(), OriginalCursor_x, OriginalCursor_y, &x, &y, &w, &h);
  fillRect(x, y, w, h, textbgcolor); // textbgcolor is protected in Adafruit_GFX.h
  print(newString.c_str()); 
  setCursor(OriginalCursor_x, OriginalCursor_y);   
}
