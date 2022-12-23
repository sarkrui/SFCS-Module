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

// SS
#define SCLK_PIN    D9

/**
   @brief Pin number of ATtiny85 connected to SDATA pin of AD5932.

   AD5932 receives data from this pin when SCLK pin changed from high to low.
*/
// UNO MOSI
#define SDATA_PIN   D10

/**
  @brief Pin number of ATtiny85 connected to FSYNC pin of AD5932.

  When data will be written to AD5932, this pin should go low first.
  This pin should be high when idle.
*/
// UNO MISO
#define FSYNC_PIN   D8

/**
  @brief Pin number of ATtiny85 connected to CTRL pin of AD5932.

  When AD5932 configured, we should make this pin trigger to start the scan. Should transit as low-high-low.
  This transition starts the scan.
*/
// UNO SCK
#define CTRL_PIN    D7

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
uint16_t val;


/**
   @brief This is the number of increment steps
*/
// sfreq = 1e4
// efreq = 16e5
// nincr = 500
//
uint32_t nincr = 300;
uint32_t cp = 1300;
uint32_t sfreq = 1e3;
uint32_t dfreq = 5000;
uint32_t sweepDurMs = (nincr * cp * 20) / 1000000;


// Variables will change:
uint32_t previousMillis = 0;
const long interval = 33;           // interval at which to print (milliseconds)

/**
   @brief This is the clock period
*/

void start_new_scan()
{
  //reset the AD5932
  ad5932.reset();

  //set start frequency for the first step
  ad5932.set_start_freq(sfreq);

  //increment frequency for each step
  ad5932.set_delta_freq(dfreq);

  //increment frequency times
  ad5932.set_nincr(nincr);

  //each freqnecy will be out for cp of time.
  ad5932.set_tint(AD5932_TINT_TYPE_CP, cp);

  //start a scan
  ad5932.trig_ctrl();

}

int smooth(int data) {
  int i;
  int value = 0;
  int numReadings = 5;

  for (i = 0; i < numReadings;) {

    // 1ms pause adds more stability between reads.
    static unsigned long last;
    if (millis() - last >= 1) {

      // Read light sensor data.
      value = value + data;
      i++;
    }
  }

  // Take an average of all the readings.
  value = value / numReadings;

  return value;
}

void setup()
{
  //initialize the AD5932
  ad5932.init();
//  Serial.begin(9600);
}

void loop()
{
  // start new scan
  start_new_scan();

  //save starting time of scan
  start = millis();

  //wait for a @ref sweepDurMs
  while (millis() - start < sweepDurMs)
  {
  }
}
