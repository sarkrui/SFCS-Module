#include "ad5932.h"

AD5932::AD5932(uint32_t fmclk, int8_t sclk, int8_t sdata, int8_t fsync, int8_t ctrl):
  fmclk(fmclk), sclk(sclk), sdata(sdata), fsync(fsync), ctrl(ctrl)
{

}

void AD5932::init()
{
  pinMode(sclk, OUTPUT);
  pinMode(sdata, OUTPUT);
  pinMode(fsync, OUTPUT);
  pinMode(ctrl, OUTPUT);

  digitalWrite(sclk, HIGH);
  digitalWrite(fsync, HIGH);
  digitalWrite(ctrl, LOW);

  reset();
}

int AD5932::capRead(uint8_t pin_num, uint8_t smooth_degree)
{

  // change the resolution to 12 bits and read ADC_PIN
  analogReadResolution(12);

  int numIndex;
  int readVal = 0;

  for (numIndex = 0; numIndex < smooth_degree;) {

    // Read sensor data.
    readVal = readVal + analogRead(pin_num);
    numIndex++;

  }

  // Take an average of all the readings.
  readVal = readVal / smooth_degree;

  return readVal;
}

/*
* |Num |Smooth|
* |----|------|
* |59  | 20   |
* |93  | 12   |
* |101 | 11   |
* |109 | 10   |
* |120 | 9    |
* |133 | 8    |
* |148 | 7    |
* |168 | 6    |
* |192 | 5    |
* |228 | 4    |
* |275 | 3    |
* |354 | 2    |
* |487 | 1    |
*/

const int AD5932::bufferSize(uint8_t smooth_degree)
{
  switch (smooth_degree) {
    case 1:
      return 490;
      break;

    case 2:
      return 355;
      break;

    case 3:
      return 275;
      break;

    case 4:
      return 230;
      break;

    case 5:
      return 192;
      break;

    case 6:
      return 168;
      break;

    case 7:
      return 148;
      break;

    case 8:
      return 133;
      break;

    case 9:
      return 120;
      break;

    case 10:
      return 109;
      break;

    case 11:
      return 101;
      break;

    case 12:
      return 93;
      break;

    default:
      return 500;
      break;
  }
}

void AD5932::reset()
{
  //writing to control register will reset the ic and stays the midscale
  write_reg(AD5932_REG_CREG, control_reg);
}

void AD5932::set_start_freq(uint32_t start_freq)
{
  uint32_t freq = ((uint64_t)start_freq << 24) / fmclk;

  write_reg(AD5932_REG_FSTART_L, freq);
  write_reg(AD5932_REG_FSTART_H, freq >> 12);
}

void AD5932::set_delta_freq(int32_t delta_freq)
{
  uint32_t s;
  if (delta_freq > 0)
  {
    s = 0;
  }
  else
  {
    s = 1;
  }

  uint32_t freq = ((uint64_t)abs(delta_freq) << 24) / fmclk;

  write_reg(AD5932_REG_DELTAF_L, freq);
  write_reg(AD5932_REG_DELTAF_H, (s << 11) | (0x07FF & (freq >> 12)));
}

uint16_t AD5932::set_nincr(uint16_t nincr)
{
  if (nincr > 4095)
  {
    nincr = 4095;
  }
  else if (nincr < 2)
  {
    nincr = 2;
  }
  write_reg(AD5932_REG_NINCR, nincr);

  return nincr;
}

unsigned int AD5932::set_tint(unsigned int type, unsigned int n)
{
  unsigned int act_n;
  uint8_t reg = AD5932_REG_TINT;

  if (n > 500 * 2047)
  {
    n = 2047;
    act_n = n * 500;
    bitWrite(reg, 0, 1);
    bitWrite(n, 11, 1);
  }
  else if (n > 100 * 2047)
  {
    n /= 500;
    act_n = n * 500;
    bitWrite(reg, 0, 1);
    bitWrite(n, 11, 1);
  }
  else if (n > 5 * 2047)
  {
    n /= 100;
    act_n = n * 100;
    bitWrite(reg, 0, 1);
    bitWrite(n, 11, 0);
  }
  else if (n > 1 * 2047)
  {
    n /= 5;
    act_n = n * 5;
    bitWrite(reg, 0, 0);
    bitWrite(n, 11, 1);
  }
  else if (n < 2)
  {
    n = 2;
    act_n = n;
    bitWrite(reg, 0, 0);
    bitWrite(n, 11, 0);
  }
  else
  {
    act_n = n;
    bitWrite(reg, 0, 0);
    bitWrite(n, 11, 0);
  }

  if (type == AD5932_TINT_TYPE_WC)
  {
    bitWrite(reg, 1, 0);
    write_reg(reg, n);
  }
  else if (type == AD5932_TINT_TYPE_CP)
  {
    bitWrite(reg, 1, 1);
    write_reg(reg, n);
  }

  return act_n;
}

void AD5932::trig_ctrl()
{
  digitalWrite(ctrl, HIGH);
  delayMicroseconds(1);
  digitalWrite(ctrl, LOW);
}

void AD5932::write_reg(uint8_t reg, uint16_t data)
{
  data = (uint16_t)reg << 12 | (data & 0x0FFF);

  digitalWrite(sclk, HIGH);
  digitalWrite(fsync, LOW);

  shiftOut(sdata, sclk, MSBFIRST, data >> 8);
  shiftOut(sdata, sclk, MSBFIRST, data & 0xFF);

  digitalWrite(fsync, HIGH);
}
