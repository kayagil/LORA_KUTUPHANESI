
Bu kütüphane, STM32 mikrodenetleyicileri ile RFM98/SX127x LoRa modüllerini kullanarak telemetri verisi gönderme ve alma işlemini basitleştirmek için tasarlanmıştır.

Mimari Anlayışı 
Kütüphane 3 katmandan oluşur. Kullanıcı olarak sadece 1. ve 3. katmanlarla ilgilenmeniz yeterlidir.

Yapılandırma Katmanı (..._config.h, ..._protocol.h): Donanım pinleriniz, LoRa ayarlarınız ve göndereceğiniz veri paketinin yapısı. burayı projenize göre düzenleyin

Sürücü Katmanı (lora.c, lora.h): LoRa modülüyle haberleşen sihirli kodlar. buraya dokunmanıza gerek yok.

Uygulama Katmanı (main.c): ana kod.

Kullanım Adımları (Bir Bakışta)
Adım 1: Donanım ve LoRa Ayarlarını Yapılandırın (lora_config.h)
Kütüphaneyi kullanmaya başlamak için değiştirmeniz gereken TEK dosya budur. Core/Inc/lora_config.h dosyasını açın ve projenize göre aşağıdaki alanları güncelelyin.

----------------------------------------------------------------------------------------------------

 Core/Inc/lora_config.h

         DONANIM AYARLARI 
 CubeMX'te SPI1'i aktif edin veya aşağıyı seçtiğiniz spiya göre değiştirin.

extern SPI_HandleTypeDef hspi1;
#define LORA_SPI_HANDLE         &hspi1

CubeMX'te pinlerele aşağıdaki isimlerin aynı olduğundan emin olun

#define LORA_NSS_PORT           LORA_NSS_GPIO_Port
#define LORA_NSS_PIN            LORA_NSS_Pin

#define LORA_RESET_PORT         LORA_RESET_GPIO_Port
#define LORA_RESET_PIN          LORA_RESET_Pin

#define LORA_DIO0_PORT          LORA_DIO0_GPIO_Port
#define LORA_DIO0_PIN           LORA_DIO0_Pin


 VARSAYILAN LORA PARAMETRELERİni ihtiycınıza göre değiştirin.

// Hangi frekansta çalışacaksınız (Türkiye için 433MHz veya 868MHz yasal)
#define LORA_DEFAULT_FREQUENCY      433000000UL

// Gönderme gücü (dBm, 2-17 arası önerilir)
#define LORA_DEFAULT_POWER          17

// Menzil ve hız ayarı (SF7 en hızlı, SF12 en uzun menzilli)
#define LORA_DEFAULT_SF             7

// Bant genişliği (Genellikle 125 kHz kullanılır)
#define LORA_DEFAULT_BANDWIDTH      LORA_BANDWIDTH_125_KHZ

// Hata düzeltme oranı (Genellikle 4/5 yeterlidir)
#define LORA_DEFAULT_CODINGRATE     LORA_CODINGRATE_4_5

----------------------------------------------------------------------------------------------
Adım 2: Gönderilecek Veri Paketini Tanımlayın (telemetry_protocol.h)
Core/Inc/telemetry_protocol.h dosyasını açın ve TelemetryPacket_t struct'ının içini göndermek istediğiniz verilerle doldurun.

----------------------------------------------------------------------------------------------

// Core/Inc/telemetry_protocol.h

typedef struct __attribute__((packed)) {
    // BU ALANLARI KENDİ PROJENİZE GÖRE  DEĞİŞTİRİN
    uint16_t packet_number;
    float    temperature;
    float    pressure;
    uint32_t system_time;
    // ... istediğiniz kadar veri ekleyebilirsiniz .
} TelemetryPacket_t;

Adım 3: main.c 


--------------------------------------------------------------------------------------------------
Örnek 1: Gönderici (Transmitter) main.c
--------------------------------------------------------------------------------------------------

// main.c - GÖNDERİCİ

#include "main.h"
#include "telemetry_protocol.h" //  bu .h dosyasını eklemeyin

//  Global Değişkenler 
SPI_HandleTypeDef hspi1; // CubeMX tarafından oluşturulur
LoRa_HandleTypeDef MyLoRa;
TelemetryPacket_t telemetry_data;

// --- INTERRUPT YÖNLENDİRİCİSİ ---
// Bu fonksiyonu projenize kopyalayın. DIO0 pininden sinyal gelince küyüphaneye haber veren interrupt kodu.
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == LORA_DIO0_PIN) {
        LoRa_Handle_DIO0(&MyLoRa);
    }
}

int main(void) {
    
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();

    //  LoRa'yı varsayılan ayarlarla başlat
    if (LoRa_InitDefault(&MyLoRa) == 0) {
        // Hata! Modül bulunamadı. Burada bir hata LED'i yakabilirsiniz.
        while(1);
    }

    telemetry_data.packet_number = 0;

    
    while (1) {
        //  Gönderilecek verileri struct'a doldur
        telemetry_data.packet_number++;
        telemetry_data.temperature = 25.5f; // Sensörden okunan değer
        telemetry_data.pressure = 1013.2f;   // Sensörden okunan değer
        telemetry_data.system_time = HAL_GetTick();

        //  Paketi gönder
        Telemetry_SendPacket(&MyLoRa, &telemetry_data);

        //  Bir sonraki gönderim için bekleme süresi
        HAL_Delay(2000); // 2 saniye bekle
    }
}
-----------------------------------------------------------------------------------------------
Örnek 2: Alıcı (Receiver) main.c
-----------------------------------------------------------------------------------------------

// main.c - ALICI

#include "main.h"
#include "telemetry_protocol.h" // bu .h dosyasını ekleyin
#include <stdio.h> // printf için (opsiyonel)

// --- Global Değişkenler ---
SPI_HandleTypeDef hspi1;
LoRa_HandleTypeDef MyLoRa;
TelemetryPacket_t received_data;
extern volatile uint8_t rx_done_flag; // flage erişim

// interrupt fonksiyonu
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == LORA_DIO0_PIN) {
        LoRa_Handle_DIO0(&MyLoRa);
    }
}

int main(void) {
    
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();
    // printf için uart veya benzeri bişeyi başlatın, sprintf için cube ide ayarını yapın

    //  LoRa'yı varsayılan ayarlarla başlat
    if (LoRa_InitDefault(&MyLoRa) == 0) {
        // Hata! Modül bulunamadı.
        while(1);
    }

    // LoRa'yı sürekli dinleme moduna al
    LoRa_ReceiveMode(&MyLoRa);

   
    while (1) {
        // Yeni bir paket gelip gelmediğini kontrol et
        if (rx_done_flag) {
            
            // Paket geldiyse, veriyi kütüphaneden oku
            uint8_t success = Telemetry_ReadPacket(&MyLoRa, &received_data);

            if (success) {
                // Veri başarıyla okundu, şimdi kullanabiliriz.
                // Örnek: UART üzerinden terminale yazdır
                printf("Paket No: %u, Sıcaklık: %.2f C\r\n", 
                       received_data.packet_number, 
                       received_data.temperature);
            }
            
            // flagi sıfırla
            rx_done_flag = 0; 
        }
        
        // İşlemciyi uyutarak güç tasarrufu yapılabilir.
        // __WFI(); 
    }
}