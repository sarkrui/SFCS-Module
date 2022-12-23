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
#define SMOOTH_DEG  5

AD5932 ad5932(OSC_FREQ, SCLK_PIN, SDATA_PIN, FSYNC_PIN, CTRL_PIN);

/**
   @brief Current time in ms to keep starting time of the scan.

   We should wait for @ref SWEEP_DURATION_MS and stop the AD5932.
*/
uint32_t start;
uint64_t startMicros;
/**
   @brief This is the ADC value read from the ADC pin.
*/
int val;

/**
   @brief This is to create a data buffer.
*/


/**
   @brief This is the number of increment steps
*/

uint32_t sdur = 1500000;
uint32_t nincr = 1000; 
uint32_t cp = sdur / nincr;
uint32_t sweepDurUs = (nincr * cp * 20) / 1000;

// 
uint32_t sfreq = 160000; 

//
uint32_t efreq = 230000;
uint32_t dfreq = efreq / nincr;


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

void setup()
{
  //initialize the AD5932
  ad5932.init();
  Serial.begin(9600);

  //wait until serial monitor/plotter is ready
  //  while (!Serial);

  unsigned long start = millis();

  Serial.println("Waiting for 2 seconds.");

  while (millis() - start < 2000 ) {
    Serial.print(".");
    delay(500);
  }
}

void loop()
{

  char serial;
  serial = Serial.read();

  // start new scan
  start_new_scan();

  // save starting time of scan in us
  startMicros = micros();

  // save reading index
  uint32_t index = 0;

  //wait for a @ref sweepDurMs
  while (micros() - startMicros < sweepDurUs)
  {
    if (index < ad5932.bufferSize(SMOOTH_DEG)) {

      Serial.print(ad5932.capRead(ADC_PIN, SMOOTH_DEG));
      Serial.println();
      //      Serial.print(",");
      index++;
    }
  }// end-of-sweep

  // return line at the last line of data

}
