/*********
  Complete project details at https://randomnerdtutorials.com
  
  This is an example for our Monochrome OLEDs based on SSD1306 drivers. Pick one up today in the adafruit shop! ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 pixel display using I2C to communicate 3 pins are required to interface (two I2C and one reset).
  Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. BSD license, check license.txt for more information All text above, and the splash screen below must be included in any redistribution. 
*********/

#include <Log.h>
#include <SerialLogger.h>
#include "U8g2lib.h"
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "Eye.h"
const int DISPLAY_WIDTH = 128;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
Eye eyeLeft(DISPLAY_WIDTH / 2 - 18 , 40, 3);
Eye eyeRight(DISPLAY_WIDTH / 2 + 18, 40, -3);

void setup(void)
{
  u8g2.begin();
  Serial.begin(115200);
  Log::init(new SerialLogger());
}

void drawEye(int x, int y, int pupilOffset, int directionOffset, float open)
{
  if (open == 0.)
  {
    return;
  }
  const float DIRECTION_PUPIL_FACTOR = 0.2;
  int width = 30;
  int height = open * 45;
  int roundness = 10;
  u8g2.setColorIndex(1);
  if (roundness > height / 2.)
  {
    roundness = height / 2.;
  }
  u8g2.drawRBox(x - width / 2 + directionOffset, y - height / 2, width, height, roundness);
  u8g2.setColorIndex(0);
  int pupilDirectionOffset = DIRECTION_PUPIL_FACTOR * directionOffset;
  u8g2.drawFilledEllipse(x + pupilOffset + directionOffset + pupilDirectionOffset, y, 5, 5);
  Log::infof("Height: %d, open: %.2f", height, open);
}

void loop(void)
{
  int range = 30;
  for (int directionOffset = -range; directionOffset <= range; directionOffset += 5)
  {

    for (int i = 100; i >= 0; i -= 25)
    {
      u8g2.clearBuffer();
      eyeLeft.draw(u8g2, i / 100., directionOffset);
      eyeRight.draw(u8g2, i / 100., directionOffset);
      u8g2.sendBuffer();
      delay(60);
    }

    for (int i = 0; i <= 100; i += 25)
    {
      u8g2.clearBuffer();
      eyeLeft.draw(u8g2, i / 100., directionOffset);
      eyeRight.draw(u8g2, i / 100., directionOffset);
      u8g2.sendBuffer();
      delay(60);
    }

    delay(2000);
  }
}
