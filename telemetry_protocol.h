// telemetry_protocol.h

#ifndef INC_TELEMETRY_PROTOCOL_H_
#define INC_TELEMETRY_PROTOCOL_H_

#include "lora.h"
#include <stdint.h>

// Projenize özel telemetri paketi yapısı
typedef struct __attribute__((packed)) {
    uint16_t packet_number;            // 2 byte
    uint8_t  flight_state;             // 1 byte
    // ... struct'ınızın diğer tüm alanları buraya gelecek ...
    float    latitude;                 // 4 byte
    float    longitude;                // 4 byte
    uint32_t gps_utc_time;             // 4 byte
} TelemetryPacket_t;

// --- PUBLIC FONKSİYON PROTOTİPLERİ ---
void Telemetry_SendPacket(LoRa_HandleTypeDef* lora, TelemetryPacket_t* packet);
uint8_t Telemetry_ReadPacket(LoRa_HandleTypeDef* lora, TelemetryPacket_t* packet);

#endif /* INC_TELEMETRY_PROTOCOL_H_ */