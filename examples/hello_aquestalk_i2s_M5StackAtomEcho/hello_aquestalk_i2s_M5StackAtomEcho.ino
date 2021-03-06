// hello_aquestalk.ino - AquesTalk pico for ESP32 サンプルプログラム
#include "driver/i2s.h"
#include "aquestalk.h"

#include "M5Atom.h"

#define LEN_FRAME 32
uint32_t workbuf[AQ_SIZE_WORKBUF];
const int i2s_num = 0; // i2s port number

void setup() {
  M5.begin();

  Serial.println("Initialize AquesTalk");
  int iret = CAqTkPicoF_Init(workbuf, LEN_FRAME, "XXX-XXX-XXX");
  if (iret) {
    Serial.println("ERR:CAqTkPicoF_Init");
  }

  // Set Speed(8000:Yukkuri, 12000:Normal)
  DAC_Create(8000);

  Serial.println("D/A start");

  playAquesTalk("akue_suto'-_ku/kido-shima'_shita.");
  playAquesTalk("botanno/o_shitekudasa'i.");
}

void loop() {
  static int8_t voice = 0;

  M5.update();
  if (M5.Btn.wasReleased()) {
    if (voice == 0) {
      playAquesTalk("konnnichiwa.");
    } else if (voice == 1) {
      playAquesTalk("korewa;te'_sutode_su.");
    } else {
      playAquesTalk("yukkuri_siteittene?");
    }
    voice++;
    voice = voice % 3;
  }

  delay(1);
}

// 一文の音声出力（同期型）
void playAquesTalk(const char *koe)
{
  Serial.print("Play:");
  Serial.println(koe);

  int iret = CAqTkPicoF_SetKoe((const uint8_t*)koe, 100, 0xffffU);
  if (iret) {
    Serial.println("ERR:CAqTkPicoF_SetKoe");
  }

  for (;;) {
    int16_t wav[LEN_FRAME];
    uint16_t len;
    iret = CAqTkPicoF_SyntheFrame(wav, &len);
    if (iret) {
      break; // EOD
    }

    DAC_Write((int)len, wav);
  }

  i2s_zero_dma_buffer((i2s_port_t)i2s_num);
}

////////////////////////////////
//i2s configuration
i2s_config_t i2s_config = {
  .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
  .sample_rate          = 8000,
  .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,
  .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S,
  .intr_alloc_flags     = 0,
  .dma_buf_count        = 4,
  .dma_buf_len          = 384,
  .use_apll             = 0,
  .tx_desc_auto_clear   = true,
  .fixed_mclk           = 0,
};

static const i2s_pin_config_t pin_config = {
  .bck_io_num           = 19,
  .ws_io_num            = 33,
  .data_out_num         = 22,
  .data_in_num          = 23
};

void DAC_Create(int speed)
{
  AqResample_Reset();

  i2s_config.sample_rate = speed;

  i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)i2s_num, &pin_config);
  i2s_zero_dma_buffer((i2s_port_t)i2s_num);
}

void DAC_Release()
{
  i2s_driver_uninstall((i2s_port_t)i2s_num); //stop & destroy i2s driver
}

// upsampling & write to I2S
int DAC_Write(int len, int16_t *wav)
{
  int i;
  for (i = 0; i < len; i++) {
    // upsampling x3
    int16_t wav3[3];
    AqResample_Conv(wav[i], wav3);

    // write to I2S DMA buffer
    for (int k = 0; k < 3; k++) {
      size_t transBytes;
      uint16_t sample[2];
      uint16_t us = ((uint16_t)wav3[k]);
      sample[0] = sample[1] = us; // mono -> stereo
      int iret = i2s_write((i2s_port_t)i2s_num, (const char*)sample, 4, &transBytes, portMAX_DELAY);
      if (iret < 0) {
        return iret; // -1:ESP_FAIL
      }
      if (iret == 0) {
        break;	//	0:TIMEOUT
      }
    }
  }
  return i;
}
