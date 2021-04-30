#include <Arduino.h>
#include <driver/i2s.h>
#include <driver/dac.h>
//output sine wave(1.5KHz) on pin 25 & pin26.

#define I2S_FR 48000

//should be 2^n
#define NLEN      32

const uint16_t sine_wave[NLEN] = {
  153,  177,  199,  219,  234,  246,  254,  255,  254,  246,  234,  219,  199,  177,  153,  128,
  103,   79,   57,   37,   22,   10,    2,    0,    2,   10,   22,   37,   57,   79,  103,  128
};

void setup_i2sdac();
void update_i2s_1sample();


void setup()
{
  Serial.begin(115200);
  Serial.println("output 1.5KHz sine wave to I2S DAC- pin 25&26");

  setup_i2sdac();
}

void loop()
{
  update_i2s_1sample();
}


void update_i2s_1sample()
{
  static uint16_t cnt = 0;
  uint32_t samp = sine_wave[cnt & (NLEN - 1)] << 8;
  samp = samp | (samp << 16);
  size_t  szt = 0;
  i2s_write(I2S_NUM_0, (const void*)&samp, 4, (size_t*)&szt, portMAX_DELAY);
  cnt += 1;
}

void setup_i2sdac()
{
   //we can only use I2S0 with internal DAC
  static const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = I2S_FR,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 2,//at least 2
    .dma_buf_len = 8,//at least 8
    .use_apll = false
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);   //install and start i2s driver

  i2s_set_pin(I2S_NUM_0, NULL); //for internal DAC, this will enable both of the internal channels

  //You can call i2s_set_dac_mode to set built-in DAC output mode.
  //i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

  i2s_set_sample_rates(I2S_NUM_0, I2S_FR); //set sample rates
}
