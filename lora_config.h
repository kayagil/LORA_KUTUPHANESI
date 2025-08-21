// lora_config.h

#ifndef INC_LORA_CONFIG_H_
#define INC_LORA_CONFIG_H_

// Bu dosya, lora kütüphanesini projenize özel hale getiren tek yerdir.
// Farklı bir karta geçiş yapıldığında sadece bu dosyanın güncellenmesi yeterlidir.

#include "stm32f4xx_hal.h" // Kullandığınız MCU'nun HAL kütüphanesi

// ============================================================================
// ||                      DONANIM AYARLARI (PINOUT)                         ||
// ============================================================================

// Kullanılacak SPI portunun handle'ı (main.c'de tanımlı global değişken)
extern SPI_HandleTypeDef hspi1;
#define LORA_SPI_HANDLE         &hspi1

// LoRa modülünün bağlı olduğu pinler (Bu define'lar CubeMX'te verdiğiniz isimlerle aynı olmalı)
#define LORA_NSS_PORT           LORA_NSS_GPIO_Port
#define LORA_NSS_PIN            LORA_NSS_Pin

#define LORA_RESET_PORT         LORA_RESET_GPIO_Port
#define LORA_RESET_PIN          LORA_RESET_Pin

#define LORA_DIO0_PORT          LORA_DIO0_GPIO_Port
#define LORA_DIO0_PIN           LORA_DIO0_Pin


// ============================================================================
// ||                    VARSAYILAN LoRa PARAMETRELERİ                       ||
// ============================================================================

// Çalışılacak frekans (Hz). Türkiye için yasal ISM bantları 433MHz ve 868MHz'dir.
#define LORA_DEFAULT_FREQUENCY      433000000UL

// Gönderme gücü (dBm). Yasal sınırlara ve pil ömrüne dikkat edin.
// Seçenekler: 2-17 (PA_BOOST pini için)
#define LORA_DEFAULT_POWER          17

// Yayılma Faktörü. Menzil ve veri hızı arasındaki temel denge.
// Düşük SF = Kısa Menzil, Yüksek Hız. Yüksek SF = Uzun Menzil, Düşük Hız.
// Seçenekler: 7, 8, 9, 10, 11, 12
#define LORA_DEFAULT_SF             7

// Bant Genişliği. Menzil ve veri hızı arasındaki ikinci denge.
// Düşük BW = Uzun Menzil, Düşük Hız. Yüksek BW = Kısa Menzil, Yüksek Hız.
// Seçenekler: LORA_BANDWIDTH_7_8_KHZ, LORA_BANDWIDTH_10_4_KHZ, LORA_BANDWIDTH_15_6_KHZ,
//             LORA_BANDWIDTH_20_8_KHZ, LORA_BANDWIDTH_31_25_KHZ, LORA_BANDWIDTH_41_7_KHZ,
//             LORA_BANDWIDTH_62_5_KHZ, LORA_BANDWIDTH_125_KHZ, LORA_BANDWIDTH_250_KHZ,
//             LORA_BANDWIDTH_500_KHZ (125 kHz en yaygın olanıdır)
#define LORA_DEFAULT_BANDWIDTH      LORA_BANDWIDTH_125_KHZ

// Kodlama Oranı. Güvenilirlik ve veri yükü arasındaki denge.
// Yüksek oran = Daha güvenilir, daha yavaş. Düşük oran = Daha az güvenilir, daha hızlı.
// Seçenekler: LORA_CODINGRATE_4_5, LORA_CODINGRATE_4_6, LORA_CODINGRATE_4_7, LORA_CODINGRATE_4_8
//             (4/5 çoğu uygulama için yeterlidir)
#define LORA_DEFAULT_CODINGRATE     LORA_CODINGRATE_4_5


#endif /* INC_LORA_CONFIG_H_ */