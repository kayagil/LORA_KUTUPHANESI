// telemetry_protocol.c

#include "telemetry_protocol.h"

void Telemetry_SendPacket(LoRa_HandleTypeDef* lora, TelemetryPacket_t* packet)
{
    // Bu fonksiyon, alt katmandaki LoRa sürücüsünü çağırarak telemetri paketinin byte'a çevrilip gönderilmesini sağlar.
    LoRa_SendMessage(lora, (uint8_t*)packet, sizeof(TelemetryPacket_t));
}

uint8_t Telemetry_ReadPacket(LoRa_HandleTypeDef* lora, TelemetryPacket_t* packet)
{
    // Alt katmandan gelen byte'ları okur ve telemetri paketi boyutunda olup olmadığını kontrol eder.
    uint8_t read_len = LoRa_ReadPacket(lora, (uint8_t*)packet, sizeof(TelemetryPacket_t));
    
    if (read_len == sizeof(TelemetryPacket_t)) {
        return 1; // Başarılı, paket boyutu doğru
    }
    
    return 0; // Hata veya eksik/fazla boyutta paket
}