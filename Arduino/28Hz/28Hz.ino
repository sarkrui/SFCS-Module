#include "ad5932.h"

/**
   @brief Sweep duration in ms.

   This value is determining sweep duration in ms. For example, if it is 33, the output signal will go from 1kHz to 3.5MHz in 33ms.
   When scan is completed, we should start another scan programatically by reseting and configuring AD5932.
*/
#define SWEEP_DURATION_MS 33

/**
   @brief This is the oscillator frequency value in Hz conntected to MCLK pin of AD5932.
*/
#define OSC_FREQ 50e6

/**
   @brief Pin number of ATtiny85 connected to SCLK pin of AD5932.

   AD5932 receives data from SDATA pin on falling edge of SCLK pin.
   This pin should be stay on high when idle.
*/
#define SCLK_PIN    10

/**
   @brief Pin number of ATtiny85 connected to SDATA pin of AD5932.

   AD5932 receives data from this pin when SCLK pin changed from high to low.
*/
#define SDATA_PIN   11

/**
  @brief Pin number of ATtiny85 connected to FSYNC pin of AD5932.

  When data will be written to AD5932, this pin should go low first.
  This pin should be high when idle.
*/
#define FSYNC_PIN   12

/**
  @brief Pin number of ATtiny85 connected to CTRL pin of AD5932.

  When AD5932 configured, we should make this pin trigger to start the scan. Should transit as low-high-low.
  This transition starts the scan.
*/
#define CTRL_PIN    13

/**
   @brief Pin number of ATtiny85 connected to ADC pin of AD5932.
   This pin is where we will read envelope data coming from pcb.
*/
#define ADC_PIN     A0

AD5932 ad5932(OSC_FREQ, SCLK_PIN, SDATA_PIN, FSYNC_PIN, CTRL_PIN);

/**
   @brief Current time in ms to keep starting time of the scan.

   We should wait for @ref SWEEP_DURATION_MS and stop the AD5932.
*/
uint32_t start;


/**
   @brief This is the ADC value read from the ADC pin.
*/
int val;


/**
   @brief This is the number of increment steps
*/
uint32_t nincr = 200;

/**
   @brief This is the clock period
*/
uint32_t cp = 8000;
uint32_t sweepDurMs = (nincr * cp * 20) / 1000000;
/**
   @brief This is the clock period
*/



void start_new_scan()
{
  //reset the AD5932
  ad5932.reset();

  //set start frequency to 4kHz
  ad5932.set_start_freq(40000);

  //increment frequency by 17.5kHz for each step
  ad5932.set_delta_freq(17500);

  //increment frequency times
  ad5932.set_nincr(nincr);

  //each freqnecy will be out for 165us.
  ad5932.set_tint(AD5932_TINT_TYPE_CP, cp);

  //start scan
  ad5932.trig_ctrl();

  //  while(true);
}

void setup()
{
  //initialize the AD5932
  ad5932.init();
}

void loop()
{
  //start new scan
  start_new_scan();

  delay((nincr * cp * 20) / 1000000);
  return;

  //save starting time of scan
  start = millis();
  sweepDurMs = (nincr * cp * 20) / 1000000;

  //wait for a @ref sweepDurMs
  while (millis() - start < sweepDurMs)
  {
    //read adc
    //val = analogRead(ADC_PIN);
  }
}
