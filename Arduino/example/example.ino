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
#define SCLK_PIN    0

/**
   @brief Pin number of ATtiny85 connected to SDATA pin of AD5932.

   AD5932 receives data from this pin when SCLK pin changed from high to low.
*/
#define SDATA_PIN   1

/**
  @brief Pin number of ATtiny85 connected to FSYNC pin of AD5932.

  When data will be written to AD5932, this pin should go low first.
  This pin should be high when idle.
*/
#define FSYNC_PIN   2

/**
  @brief Pin number of ATtiny85 connected to CTRL pin of AD5932.

  When AD5932 configured, we should make this pin trigger to start the scan. Should transit as low-high-low.
  This transition starts the scan.
*/
#define CTRL_PIN    3

/**
   @brief Pin number of ATtiny85 connected to ADC pin of AD5932.
   This pin is where we will read envelope data coming from pcb.
*/
#define ADC_PIN     4

AD5932 ad5932(OSC_FREQ, SCLK_PIN, SDATA_PIN, FSYNC_PIN, CTRL_PIN);

/**
   @brief Current time in ms to keep starting time of the scan.

   We should wait for @ref SWEEP_DURATION_MS and stop the AD5932.
*/
int start;

/**
   @brief This is the ADC value read from the ADC pin.
*/
int val;

/**
   @brief This function starts new scan.
*/
void start_new_scan()
{
  //reset the AD5932
  ad5932.reset();

  //set start frequency to 1kHz
  ad5932.set_start_freq(1e3);

  //increment frequency by 17.5kHz for each step
  ad5932.set_delta_freq(17500);

  //increment frequency 200 times
  ad5932.set_nincr(200);

  //each freqnecy will be out for 165us.
  ad5932.set_tint(AD5932_TINT_TYPE_CP, 8250);

  //start scan
  ad5932.trig_ctrl();
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

  //save starting time of scan
  start = millis();

  //wait for a @ref SWEEP_DURATION_MS
  while (millis() - start < SWEEP_DURATION_MS)
  {
    //read adc
    val = analogRead(ADC_PIN);
  }
}
