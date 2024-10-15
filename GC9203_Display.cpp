#include "GC9203_Display.h"
#include <SPI.h>

GC9203_Display::GC9203_Display(int8_t ss, int8_t rst, int8_t scl, int8_t sdi, int8_t dc, uint8_t use_horizontal)
  : Adafruit_GFX((use_horizontal % 2 == 0) ? 220 : 128, (use_horizontal % 2 == 0) ? 128 : 220),// 这是一个对Adafruit_GFX基类图形库的初始化
  // 根据use_horizontal参数的值来设置显示屏的宽度和高度。
  // 但是怎么两个数值都是Horizontal？假设use_horizontal是偶数，则选220，那第二个值自动成为128.两次判断没有意义。
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
  for (i = 0; i < 8; i++) {  // 循环8次，每次处理一个比特
    digitalWrite(_scl, LOW);  // 拉低时钟信号
    if (cmd & 0x80) digitalWrite(_sdi, HIGH);  // 如果最高位是1，则拉高数据线
    else digitalWrite(_sdi, LOW);  // 如果最高位是0，则拉低数据线
    digitalWrite(_scl, HIGH);  // 拉高时钟信号，触发数据传输
    cmd = cmd << 1;  // 左移命令字节，处理下一个比特
  }
  digitalWrite(_ss, HIGH);  // 拉高片选信号，结束传输
}

void GC9203_Display::Write_Data(uint8_t data) {
 uint8_t m, n; // 定义了两个8比特的参数 m和n
  m = data >> 8; // m=00000000. 因为data只有8位，右移8位就全部移出了，只有0了
  n = data; // n=data
  Write_Data_U16(m, n); // 得到了一个高8位是0的16位数据
}

void GC9203_Display::Write_Data_U16(uint8_t DH, uint8_t DL) {

  uint8_t i;
  digitalWrite(_ss, LOW); // 片选信号拉低，开始传输数据。第一步对DH进行操作
 digitalWrite(_dc, HIGH); // 拉高传输data
  for (i = 0; i < 8; i++) { // 循环在第八次时停止
   digitalWrite(_scl, LOW); // 当时序拉低时
    if (DH & 0x80)digitalWrite(_sdi, HIGH); // 如果最高位等于1，就把串口输入信号线拉高
    else digitalWrite(_sdi, LOW);  // 如果最高位等于0，就把串口输入信号线拉低
     digitalWrite(_scl, HIGH); // 结束以上操作时，把时序拉高
    DH = DH << 1; // 将DH信号左移一位
  }
  digitalWrite(_ss, HIGH); // 结束以上操作后，把片选信号拉高
  digitalWrite(_ss, LOW); // 片选信号拉低，开始传输数据。第二步对DL进行操作，逻辑同上。
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
    // 三个字节情况下的写入函数，逻辑和上面是一样的
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
// 不分DH DL时的16位输入数据，逻辑也同上
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
// 至此，输入函数定义完了，接下来定义方法函数
  /*************************************************************************/
  /*************************************************************************/
  /*************************************************************************/

  /**********第一个方法函数：开始函数*****************************************/

void GC9203_Display::begin() {// “开始”函数 
  digitalWrite(_ss, HIGH);
  delay(5);
  digitalWrite(_rst, LOW);  // Reset the display
  delay(10);
  digitalWrite(_rst, HIGH); // 结束复位
  delay(120);

  // 接下来是命令和数据传输给显示屏，但是不知道含义？因为他的高8位也一直在变，没搞懂是地址还是什么意思。
  // 猜测是地址吧
  Write_Cmd(0xff); // 发送命令，全部拉高
  Write_Data_U16(0x5a, 0xa5); // 发送数据，不知道这个数据是发给谁的，前面是高八位，后面是第八位

  Write_Cmd(0xf6); 
  Write_Data_U16(0x01, 0x12);

  Write_Cmd(0xef); // 内部寄存器CHP Control6, 
  Write_Data_U16(0x10, 0x52); // 1052, 0001 0000 0101 0010, 把VGL_AD1拉高，其他为0，后八位是默认值。
  // 结果查表得到，VGL(010) = -9V, VGH(000) = 9V .挺合理的。验证了Write_Cmd是传输寄存器地址的意思。 

  Write_Cmd(0xa5); // a5是哪个寄存器？介于Gamma control14和CHP-CTRL1之间
  Write_Data_U16(0x07, 0x80);

  Write_Cmd(0x02); // LCD AC Driving Control
  Write_Data_U16(0x01, 0x00);

  // Write_Cmd(0x03);
  // Write_Data_U16(0x10, 0x00);   // cambiato da 0x10, 0x00 a  0x1, 0x00

   Write_Cmd(0x03); // 设置entry mode寄存器，0x12意味着BGR=1, MDT=10（也就是transfer和GRAM一一对应的格式）
  // 不同的HORIZONTAL改变的 只有数据写入GRAM的模式
if(USE_HORIZONTAL == 0)
  Write_Data_U16(0x12, 0x08);
else if(USE_HORIZONTAL == 1)
  Write_Data_U16(0x12, 0x38);
else if(USE_HORIZONTAL == 2)
  Write_Data_U16(0x12, 0x20);
else if(USE_HORIZONTAL == 3)
  Write_Data_U16(0x12, 0x10);


  Write_Cmd(0x01); // driver output control register.
  Write_Data_U16(0x01, 0x1c);   // cambiato da（从） 0x03, 0x1c a 0x00, 0x1c
  // ss=1, NL=11100, 即528*220dots，g1到g220全开
  /**************************************************************************/
  Write_Cmd(0x05); // spi_2data. 
  Write_Data_U16(0x00, 0x01);  //262K color 1pixel/transition, 2dataMDT=001. 这是RGB666
  // 外部寄存器设置完成
  /*************************************************************************/

  // 进入内部寄存器
  Write_Cmd(0xf6); //这是哪个寄存器? between CHP-ctrl6 and Inter_Reg_Dis 
  Write_Data_U16(0x01, 0x12);

  Write_Cmd(0x11); // power control2
  Write_Data_U16(0x10, 0x00); // 0d1f 1630  // vreg1b[13:8]_vreg1a_[5:0]     122B   3319  0D1F

  Write_Cmd(0xEB);  // Vreg_Ctrl1
  Write_Data_U16(0x0d, 0x1f); // 160a  // vreg2b[13:8]_vreg2a_[5:0]     1606   2334  0528

  Write_Cmd(0xEC); // Vreg_Ctrl2
  Write_Data_U16(0x05, 0x28);

  Write_Cmd(0x50); // Gamma Control1
  Write_Data_U16(0xf3, 0x80); // H:V0(7-4) V13(3-0)   L:V63(3-0)
  // 这里就开始看不懂了，高八位全部拉高，参数和注释是对应的；但是低八位，注释的意思是0x0f，参数却是0x80
  Write_Cmd(0x51);
  Write_Data_U16(0x11, 0x0c); // H:V61(5-0)   L:V62(5-0)
  // 这一组也是，高八位合理，低八位看不懂
  Write_Cmd(0x52);
  Write_Data_U16(0x09, 0x09); // H:V57(4-0)  L:V59(4-0)
  // 同上
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
  Write_Data_U16(0x37, 0x36); // H:V1(5-0)   L:V2(5-0) //以上全是Gamma控制的内容。

   Write_Cmd(0x07); // display control1 
  Write_Data_U16(0x10, 0x13); // TEMON=1, TM=0, GON=1, CL=0,REV=0,D=11 

  Write_Cmd(0xfe); // Inter_REG_DIS，未传递参数。
}
  /**********第二个方法函数：连续写入GRAM病限定窗口范围 函数*****************************************/
void GC9203_Display::LCD_SetPos(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) { // Use uint16_t
// 输入信号有两组坐标，这个函数的意义应该是设置LCD的起始和结束点。
  Write_Cmd(0x03); // R03h 还是entry mode寄存器。这一段和begin的内容有点重复，不知道原因。
  if (USE_HORIZONTAL == 0)
    Write_Data_U16(0x12, 0x08);
  else if (USE_HORIZONTAL == 1)
    Write_Data_U16(0x12, 0x38);
  else if (USE_HORIZONTAL == 2)
    Write_Data_U16(0x12, 0x20);
  else if (USE_HORIZONTAL == 3)
    Write_Data_U16(0x12, 0x10);

  if (USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1) { //即，0x08和0x38,AM=1,为垂直方向扫描时的设置
    // 接下来四个寄存器就是Horizontal and Vertical RAM Address Position，即设置窗口函数的连续写入范围。
    Write_Cmd(0x37);       //Horizontal Start
    Write_Data(x1 + 24); // 为啥要加24？
    Write_Cmd(0x36);      //Horizontal End
    Write_Data(x2 + 24);
    Write_Cmd(0x39);      //Vertical Start
    Write_Data(y1);
    Write_Cmd(0x38);      //Vertical End
    Write_Data(y2);
    // 36-39四个寄存器的命名是WIN_(VER/HOR)_(ST/ED)_ADDR我能理解中间两组命名，但是这里的WIN和ADDR是什么含义？
    Write_Cmd(0x22); // write data to GRAM. 范围设定完毕开始对GRAM连续写入。
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
  /**********插入一个功能函数：反转字节，返回反转后的数组*************************/
// Helper function to reverse the bits in an 8-bit number，好像没看到有调用？
uint8_t reverseBits(uint8_t n) {
    uint8_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result <<= 1;
        result |= (n & 1);
        n >>= 1;
    }
    return result;
}
  /**********第三个方法函数：画像素函数*****************************************/
  /最基本的像素推送器。你可以调用它，并指定参数：X Y坐标和一种颜色，它会在屏幕上生成一个点/
void GC9203_Display::drawPixel(int16_t x, int16_t y, uint16_t color) { 
  // 输入三个参量，包括一组坐标和一个色彩
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;
  // 限定xy的范围，大于0且不小于最大值。但是不知道width 和 height 啥时候设置的。

    LCD_SetPos(x, y, x, y); // 怎么输入一组坐标可以限定起始点？很奇怪啊

    // Extract RGB components from 16-bit color (RGB565 format) 提取色彩数据
    // 为什么是RGB565，可能有很多原因。但其中一个印证，就是Adafruit这个库，色彩深度就是565的配置。
    uint8_t r = (color >> 11) & 0x1F; // Extract the 5 bits for red 0001 1111 （5位）
    uint8_t g = (color >> 5) & 0x3F;  // Extract the 6 bits for green 0011 1111 （6位）
    uint8_t b = color & 0x1F;         // Extract the 5 bits for blue 0001 1111 （5位）
   // 做了个什么操作？举个例子，R的取值=color右移11位后，将高5位变成了低5位，然后与0001 1111取与。
   // 结果：R的值= (000)+(color的高五位）

    // Convert to 6-bit values (scale from 5-bit to 6-bit for red and blue)
    r = (r << 1) | (r >> 4);  // Scale red 5 bits to 6 bits r左移1位和r右移4位后的值进行按位‘或’运算。
   // 注意，左移右移，都是在空缺位置补0，而不是环形数组移动。
    b = (b << 1) | (b >> 4);  // Scale blue 5 bits to 6 bits

    // Format the color data according to the GC9203's requirements
    uint8_t red_byte = (r << 2);   // 6 Red bits + 2 dummy bits
    uint8_t green_byte = (g << 2); // 6 Green bits + 2 dummy bits
    uint8_t blue_byte = (b << 2);  // 6 Blue bits + 2 dummy bits
   // 至此得到了高6位是有效数值的三组色彩数据，而且复用了R最高位和B最高位。
    // Reverse the bits in each color byte if needed，这主要是看选了什么样的BGR顺序和屏幕显示顺序
    // red_byte = reverseBits(red_byte);
    // green_byte = reverseBits(green_byte);
    // blue_byte = reverseBits(blue_byte);

    // Use the new function to write the three color bytes，写入色彩数据。但是写给谁不知道？
    Write_Three_Bytes(red_byte, green_byte, blue_byte);
}

  /**********第四个方法函数：铺满屏幕函数*****************************************/
void GC9203_Display::fillScreen(uint16_t color) { // 什么鬼东西，你写一个函数里面嵌套一个函数，没有任何其他操作？
  // fillscreen本质上就是清理屏幕，他是用一个颜色将整个屏幕覆盖，那效果就是清理。
clearScreen(color);
}
  void GC9203_Display::clearScreen(uint16_t color){ // 清屏函数
    /***第一步，配置色彩数据。这里很奇怪，让人无法理解上一个函数为什么要那样写。
    甚至可以直接将以下三步定义为一个“配置色彩”的操作函数，再调用，不是更方便么？***/ 
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
    
    /***第二步，写色彩数据，进入屏幕坐标。***/ 
  uint8_t i, j;// 针对不同扫描顺序下的写入顺序。 (recall line.154, entry mode.)
  if ((USE_HORIZONTAL == 0) || (USE_HORIZONTAL == 1))  // use_horizontal的值在哪里读？根本没输入。
    LCD_SetPos(0, 0, 128, 220); // 看这里的意思是，前列后行。但是128我还是不理解怎么算出来的。唯一的可能是屏幕分辨率调低了。相较于176列，调低了1.375倍。色彩深度看似24位实则18位。
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
  /**********第五个方法函数：打印*****************************************/
void GC9203_Display::printNew(const long value, const int chCount) // const 常量声明。 chCount是字符数量。
{ getNumberBounds(chCount); // 后面有定义这个函数，获取数字边界
  print(value); // 打印value，定义为长整数
  setCursor(OriginalCursor_x, OriginalCursor_y); // 恢复原始光标位置  
}

void GC9203_Display::printNew(const float value, const unsigned decimals, const int chCount)
{ getNumberBounds(chCount);
  print(value, decimals); // 打印value，定义为浮点数，并指定小数位数
  setCursor(OriginalCursor_x, OriginalCursor_y);  
}

void GC9203_Display::getNumberBounds(const int chCount) 
{ OriginalCursor_x = cursor_x;  //保存当前光标的位置
  OriginalCursor_y = cursor_y;
  getTextBounds("3",  OriginalCursor_x, OriginalCursor_y, &x, &y, &charWidth, &h); // THIS CODE IS UGLY 
  getTextBounds("33", OriginalCursor_x, OriginalCursor_y, &x, &y, &char2Width, &h); // font of 3 is wider than 0 
 // 获取了3 和 33的数字显示范围。
 // getTextBounds是Adafruit的内置函数，这个函数表示了打印内容在整个画面的占比和范围。
  int space = char2Width - 2 * charWidth; // 计算字符之间的间距
  w = chCount * charWidth + (chCount - 1) * space; // 计算总宽度
  // Serial << x, y, w, h; // 0: 22 64 22 34 // 00: 22 64 48 34
  // textbgcolor = ILI9341_GREEN; // testing with green background
  fillRect(x, y, w, h, textbgcolor); // textbgcolor is protected in Adafruit_GFX.h 用背景色填充该区域
}

void GC9203_Display::printNew(const String &newString, const String &oldString) // overloading needs const here 替换旧字符为新字符，在打印前清除旧字符的显示范围。
{ OriginalCursor_x = cursor_x;
  OriginalCursor_y = cursor_y;
  getTextBounds(oldString.c_str(), OriginalCursor_x, OriginalCursor_y, &x, &y, &w, &h);
  fillRect(x, y, w, h, textbgcolor); // textbgcolor is protected in Adafruit_GFX.h
  print(newString.c_str()); 
  setCursor(OriginalCursor_x, OriginalCursor_y);   
}
