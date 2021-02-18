#include "fonts.h"

//-------------------------------------------------------------- Установка яркости
uint16_t auto_br(uint16_t lt)
{
  uint16_t c_br = 0;

  int a = constrain(conf_data.br_level[2] - conf_data.br_level[1], 1, conf_data.br_level[2]);
  int b = constrain(conf_data.br_level[3] - conf_data.br_level[0], 1, conf_data.br_level[3]);

  c_br = constrain((float)(lt - conf_data.br_level[0]) * a / b + conf_data.br_level[1], conf_data.br_level[1], conf_data.br_level[2]);
  return c_br;
}

uint16_t ft_read(bool snr_pres)
{
  uint16_t ft = 7;
  if (snr_pres) ft = lightMeter.readLightLevel();
  else
  {
#if defined(ESP8266)
    ft = analogRead(A0);
#endif
  }
  return ft;
}


void  time_m32_8()
{
  //----------------------------------------------------------------- заполнение массива
  uint8_t h = hour();
  // Do 24 hour to 12 hour format conversion when required.
  if (conf_data.use_pm && hour() > 12) h = hour() - 12;

  if (h > 9) printCharacter_m32_8(h / 10 + '0',  0, screen);
  printCharacter_m32_8 (h        % 10 + '0',     6, screen);
  printCharacter_m32_8 (minute() / 10 + '0',    13, screen);
  printCharacter_m32_8 (minute() % 10 + '0',    19, screen);
  printCharacter_m32_8 (second() / 10 + '\x80', 25, screen);
  printCharacter_m32_8 (second() % 10 + '\x80', 29, screen);
}



//-------------------------------------------------------------- Отображение бегущей строки
bool mov_str(uint8_t dtype, uint8_t dsp_wdt, String tape, uint8_t nline, int cur_sym_pos)
{
  if (dtype > 19)f_dsp.utf8rus(tape);

  if (dtype == 19) f_dsp.lcd_rus(tape);
#if defined(ESP32)
  if (dtype == 24)
  {
    m3264->setTextWrap(false);
    m3264->setTextSize(TextSize);
  }
#endif

  /*  DBG_OUT_PORT.println(tape);
    DBG_OUT_PORT.print("input string length..");
    DBG_OUT_PORT.println(tape.length());
  */
  int colors[3];
  uint8_t sym_wdt = 5 + spacer; // Ширина занимаемая символом в пикселях (5 ширина шрифта + 1 линия разделитель = 6)
  if (dtype == 19)
  {
    sym_wdt = 1;
    spacer = 0;
  }

#if defined(ESP32)
  if (dtype == 24)
  {
    sym_wdt *= TextSize;
    spacer = TextSize;
  }
#endif

  if (cur_sym_pos < sym_wdt * tape.length() + dsp_wdt - spacer)  //текущая позиция < (длина строки + ширина дисплея)
  {
    if (dtype > 19)
    {
      int16_t letter = cur_sym_pos / sym_wdt;           //        номер крайнего правого отображаемого символа
      int8_t x = (dsp_wdt - 1) - cur_sym_pos % sym_wdt; // координата х крайнего правого отображаемого символа
      uint8_t y = nline * 8;                            // координата y строки
      uint8_t bg = 0;                                   // цвет фона
      if (dtype == 22) bg = 3;

      while (x + sym_wdt - spacer >= 0 && letter >= 0)
      {
        if (letter < tape.length())
        {
          if (dtype == 20) printCharacter_m32_8 (tape[letter], x, screen); //вывод части строки посимвольно, справа налево
          if (dtype == 22) m1632 -> drawChar(x, y, tape[letter], 1, bg, 1); //вывод части строки посимвольно, справа налево
#if defined(ESP32)
          if (dtype == 24)
          {
            getRGB(abs(cur_sym_pos / 4) % 255, 255, 255, colors);
            m3264 -> drawChar(x, y, tape[letter], m3264->AdafruitColor(colors[0], colors[1], colors[2]), 0, TextSize);
          }
#endif
        }
        letter--;     // смещение на символ влево по строке
        x -= sym_wdt; // смещение на ширину символа влево по х
      }
      if (dtype == 22) m1632 -> render(); // Send bitmap to display
      //if (dtype == 24) m3216 -> update(); // Send bitmap to display
    }

    if (dtype == 19)
    {
      int16_t letter = cur_sym_pos; //        номер крайнего правого отображаемого символа
      int8_t x = dsp_wdt - 1;       // координата х крайнего правого отображаемого символа

      while (x >= 0 && letter >= 0)
      {

        if (letter < tape.length())
        {
          lcd->setCursor(x, nline);
          lcd->print(tape[letter]); //Draw char in lcd
        }
        letter--;   // смещение на символ влево по строке
        x--;
      }
    }
  }
  else return true; //end of scrolling
  return false;
}

//-------------------------------------------------------------- Отображение бегущей строки
bool scroll_String(int8_t x1, int8_t x2, char *in,  uint8_t size_in, int &ch_pos, int &ch_byte_pos, byte *buff, uint8_t font)
{
  byte inbyte; //источник байтов

  unsigned char character = in[0];

  uint8_t font_wdt = 1; // "ширина" шрифта в байтах

  if (font == 1) // шрифт 14 seg
  {
    font_wdt = 2;
    ch_byte_pos++;

    if (ch_byte_pos > font_wdt - 1)
    {
      ch_byte_pos = 0;
      if (ch_pos < size_in)
      {
        ch_pos ++;
        character = in[ch_pos]; // дергаем входящую сроку по символам

        buff[x2] = FourteenSegmentASCII[character - 32] & 0xFF;
        buff[x2 - 1] = FourteenSegmentASCII[character - 32] >> 8;

        for (uint8_t x = x2; x > x1; x -= 2)
        {
          buff[x - 2] = buff[x]; // сдвиг в буфере экрана на одну позицию (разряд) (на два байта) влево
        }
      }
      else return true; //end of scrolling
    }
  }

  if (font == 2) // шрифт 5x7
  {
    font_wdt = 5;

    if (ch_byte_pos > font_wdt - 1)
    {
      if (ch_byte_pos == font_wdt) inbyte = 0; // символ "закончился" - вставляем пустой столбик
      if (ch_byte_pos > font_wdt)
      {
        ch_byte_pos = 0;
        if (ch_pos < size_in)
        {
          ch_pos ++;
          character = in[ch_pos]; // дергаем входящую сроку по символам
        }
        else return true; //end of scrolling
      }
    }
    if (ch_byte_pos < font_wdt) inbyte = font5x7[character * font_wdt + ch_byte_pos];

    ch_byte_pos++;

    buff[x2] = inbyte; // запись крайнего правого байта (координата х2) в буфер экрана

    for (uint8_t x = x1; x < x2; x++)
    {
      buff[x] = buff[x + 1]; // сдвиг в буфере экрана на одну позицию (колонку) (на один байт) влево
    }
  }
  return false;
}
