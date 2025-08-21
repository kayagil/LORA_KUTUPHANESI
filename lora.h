// lora.h

#ifndef INC_LORA_H_
#define INC_LORA_H_

#include "lora_config.h" // Proje ayarlarını dahil et
#include <stdint.h>

// burdaki değerleri datasheetten kontrol et
typedef enum {
    LORA_BANDWIDTH_7_8_KHZ   = 0x00,
    LORA_BANDWIDTH_10_4_KHZ  = 0x10,
    LORA_BANDWIDTH_15_6_KHZ  = 0x20,
    LORA_BANDWIDTH_20_8_KHZ  = 0x30,
    LORA_BANDWIDTH_31_25_KHZ = 0x40,
    LORA_BANDWIDTH_41_7_KHZ  = 0x50,
    LORA_BANDWIDTH_62_5_KHZ  = 0x60,
    LORA_BANDWIDTH_125_KHZ   = 0x70,
    LORA_BANDWIDTH_250_KHZ   = 0x80,
    LORA_BANDWIDTH_500_KHZ   = 0x90
} LoRa_Bandwidth_t;

typedef enum {
    LORA_CODINGRATE_4_5 = 0x02,
    LORA_CODINGRATE_4_6 = 0x04,
    LORA_CODINGRATE_4_7 = 0x06,
    LORA_CODINGRATE_4_8 = 0x08
} LoRa_CodingRate_t;

typedef struct {
    SPI_HandleTypeDef* hspi;
    GPIO_TypeDef* NSS_Port;
    uint16_t              NSS_Pin;
    GPIO_TypeDef* RESET_Port;
    uint16_t              RESET_Pin;

    long                  frequency;
    uint8_t               spreadingFactor;
    LoRa_Bandwidth_t      bandwidth;
    LoRa_CodingRate_t     codingRate;
    int8_t                power;
} LoRa_HandleTypeDef;



// Başlatma Fonksiyonları
uint8_t LoRa_Init(LoRa_HandleTypeDef* lora);
uint8_t LoRa_InitDefault(LoRa_HandleTypeDef* lora);

// Gönderme (TX) Fonksiyonları
void LoRa_SendMessage(LoRa_HandleTypeDef* lora, uint8_t* data, uint8_t length);

// Alma (RX) Fonksiyonları
void LoRa_ReceiveMode(LoRa_HandleTypeDef* lora);
uint8_t LoRa_ReadPacket(LoRa_HandleTypeDef* lora, uint8_t* buffer, uint8_t maxLength);

// Interrupt fon ksiyonu
void LoRa_Handle_DIO0(LoRa_HandleTypeDef* lora);

#endif /* INC_LORA_H_ */