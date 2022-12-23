#ifndef AD5932_H
#define AD5932_H

#include <Arduino.h>

/**@brief 4-Bit value of control bits register. */
#define AD5932_REG_CREG         0x0

/**@brief 4-Bit value of number of increments register. */
#define AD5932_REG_NINCR        0x1

/**@brief 4-Bit value of lower 12 bits of delta frequency register. */
#define AD5932_REG_DELTAF_L     0x2

/**@brief 4-Bit value of higher 12 bits of delta frequency register. */
#define AD5932_REG_DELTAF_H     0x3

/**@brief 4-Bit value of increment interval register. */
#define AD5932_REG_TINT         0x4

/**@brief 4-Bit value of lower 12 bits of start frequency register. */
#define AD5932_REG_FSTART_L     0xC

/**@brief 4-Bit value of higher 12 bits of start frequency register. */
#define AD5932_REG_FSTART_H     0xD

/**@brief Type of the increment interval. It will yield incrementing fixed number of output waveform cycles. */
#define AD5932_TINT_TYPE_WC     0

/**@brief Type of the increment interval. It will yield incrementing fixed number of clock periods. */
#define AD5932_TINT_TYPE_CP     1

/**
  @class AD5932
  @brief Class for the managing AD5932 IC.
*/
class AD5932
{
  public:
    /**
        @brief Constructor of the class. It sets general variables.

        @param[in] fmclk Oscillator frequency of AD5932.

        @param[in] sclk Clock pin of the AD5932.

        @param[in] sdata Data pin of the AD5932.

        @param[in] fsync Fsync pin of the AD5932.

        @param[in] ctrl Ctrl pin of the AD5932.
    */
    AD5932(uint32_t fmclk, int8_t sclk, int8_t sdata, int8_t fsync, int8_t ctrl);

    /**
      @brief Initializes pins of ATtny85 and make AD5932 to go known state.
    */
    void init();

    /**
      @brief Resets the AD5932.

      To reset the AD5932, Data should be written to config register.
      See @ref AD5932_REG_CREG. When AD5932 is reset, output of the AD5932 will be at midscale and there will be no generated signal.
      To start the signal, required registers should be configured and ctrl pin should be triggered. See @ref trig_ctrl.
    */
    void reset();

    /**
       @brief Sets the start frequency.

       @param[in] start_freq Start frequency in Hz.
    */
    void set_start_freq(uint32_t start_freq);

    /**
       @brief Sets the delta frequency.

       When AD5932 outputs current frequency for a specified of time then delta frequency is added to the current frequency then this frequency will be output
       for a specified of time.

       @param[in] delta_freq Delta frequency in Hz. This value can be positive or negative.
    */
    void set_delta_freq(int32_t delta_freq);

    /**
       @brief Sets number of the increments.

       AD5932 will add delta frequency to the start frequency nincr times.

       @param[in] nincr Number of the frequency increments. Minimum is 2 and maximum is 4095.

       @retval      Returns actual nincr written to AD5932.
       @retval 2    If nincr is less than 2.
       @retval 4095 If nincr is higher than 4095.
    */
    uint16_t set_nincr(uint16_t nincr);

    /**
       @brief Sets duration of each frequency steps.

       This duration can be multiple of output waveform cycle or multiple of clock period.

       @param[in] type Type of the interval. See @ref AD5932_TINT_TYPE_WC and @ref AD5932_TINT_TYPE_CP.

       @param[in] n Count of the output waveform cycle or clock period. Minimum is 2 and maximum is 1023500.

       @retval          Returns Actual n written to AD5932.
       @retval 2        If n is less than 2.
       @retval 1023500  If n is higher than 1023500.
    */
    unsigned int set_tint(unsigned int type, unsigned int n);

    /**
       @brief Starts the scan by changing ctrl pin as low-high-low.
    */
    void trig_ctrl();
  private:
    /**
      @brief Pin number of ATtiny85 connected to SCLK pin of AD5932.

      AD5932 receives data from SDATA pin on falling edge of SCLK pin.
      This pin should be stay on high when idle.
    */
    const int8_t sclk;

    /**
       @brief Pin number of ATtiny85 connected to SDATA pin of AD5932.

       AD5932 receives data from this pin when SCLK pin changed from high to low.
    */
    const int8_t sdata;

    /**
      @brief Pin number of ATtiny85 connected to FSYNC pin of AD5932.

      When data will be written to AD5932, this pin should go low first.
      This pin should be high when idle.
    */
    const int8_t fsync;

    /**
      @brief Pin number of ATtiny85 connected to CTRL pin of AD5932.

      When AD5932 configured, we should make this pin trigger to start the scan. Should transit as low-high-low.
      This transition starts the scan.
    */
    const int8_t ctrl;

    /**
       @brief This is the oscillator frequency value in Hz conntected to MCLK pin of AD5932.
    */
    uint32_t fmclk;

    /**
      @brief 12 LSB, 12 MSB, sine, out enable.
    */
    uint16_t control_reg = 0b111011010011;

    /**
      @brief Writes data to the specified register.

      @param[in] reg Address of the register.

      @param[in] data 12-Bit data.
    */
    void write_reg(uint8_t reg, uint16_t data);
};

#endif /*AD5932_H*/
