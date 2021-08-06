#pragma once
#include <U8g2lib.h>

class Eye
{
public:
    Eye();
    Eye(int x, int y, int pupilOffset);
    void draw(U8G2 &u8g2, float open, int directionOffset);

private:
    int m_centerX;
    int m_centerY;
    int m_pupilOffset;
};

Eye::Eye() : Eye(0, 0, 0)
{
}

Eye::Eye(int x, int y, int pupilOffset)
    : m_centerX(x),
      m_centerY(y),
      m_pupilOffset(pupilOffset)
{
}

void Eye::draw(U8G2 &u8g2, float open, int directionOffset)
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
    u8g2.drawRBox(m_centerX - width / 2 + directionOffset, m_centerY - height / 2, width, height, roundness);
    u8g2.setColorIndex(0);
    int pupilDirectionOffset = DIRECTION_PUPIL_FACTOR * directionOffset;
    u8g2.drawFilledEllipse(m_centerX + m_pupilOffset + directionOffset + pupilDirectionOffset, m_centerY, 5, 5);
}