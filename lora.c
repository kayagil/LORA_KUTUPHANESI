// lora.c

#include "lora.h"

// --- Register Adresleri ---
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_FIFO_ADDR_PTR        0x0D
#define REG_FIFO_TX_BASE_ADDR    0x0E
#define REG_FIFO_RX_BASE_ADDR    0x0F
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_MODEM_CONFIG_1       0x1D
#define REG_MODEM_CONFIG_2       0x1E
#define REG_PAYLOAD_LENGTH       0x22
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42

// --- Modlar ---
#define MODE_LORA                0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05

// --- IRQ Maskeleri ---
#define IRQ_TX_DONE_MASK         0x08
#define IRQ_RX_DONE_MASK         0x40

// --- Kütüphane İçi (Private) Değişkenler ---
static LoRa_HandleTypeDef* _lora_handle;
static volatile uint8_t _tx_done_flag = 0;
volatile uint8_t rx_done_flag = 0; // Alıcıda main.c'den erişim için public (volatile)

// Kütüphane İçi (Private) Fonksiyonlar 
static void RFM98_Write(uint8_t address, uint8_t value) {
    uint8_t write_address = address | 0x80;
    HAL_GPIO_WritePin(_lora_handle->NSS_Port, _lora_handle->NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_lora_handle->hspi, &write_address, 1, 100);
    HAL_SPI_Transmit(_lora_handle->hspi, &value, 1, 100);
    HAL_GPIO_WritePin(_lora_handle->NSS_Port, _lora_handle->NSS_Pin, GPIO_PIN_SET);
}

static uint8_t RFM98_Read(uint8_t address) {
    uint8_t read_address = address & 0x7F;
    uint8_t received_data;
    HAL_GPIO_WritePin(_lora_handle->NSS_Port, _lora_handle->NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(_lora_handle->hspi, &read_address, 1, 100);
    HAL_SPI_Receive(_lora_handle->hspi, &received_data, 1, 100);
    HAL_GPIO_WritePin(_lora_handle->NSS_Port, _lora_handle->NSS_Pin, GPIO_PIN_SET);
    return received_data;
}

static void RFM98_SetMode(uint8_t mode) {
    RFM98_Write(REG_OP_MODE, MODE_LORA | mode);
}

// --- Genel (Public) Fonksiyonlar ---
uint8_t LoRa_InitDefault(LoRa_HandleTypeDef* lora) {
    lora->hspi = LORA_SPI_HANDLE;
    lora->NSS_Port = LORA_NSS_PORT;
    lora->NSS_Pin = LORA_NSS_PIN;
    lora->RESET_Port = LORA_RESET_PORT;
    lora->RESET_Pin = LORA_RESET_PIN;
    lora->frequency = LORA_DEFAULT_FREQUENCY;
    lora->power = LORA_DEFAULT_POWER;
    lora->spreadingFactor = LORA_DEFAULT_SF;
    lora->bandwidth = LORA_DEFAULT_BANDWIDTH;
    lora->codingRate = LORA_DEFAULT_CODINGRATE;
    return LoRa_Init(lora);
}

uint8_t LoRa_Init(LoRa_HandleTypeDef* lora) {
    _lora_handle = lora;
    HAL_GPIO_WritePin(_lora_handle->RESET_Port, _lora_handle->RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(_lora_handle->RESET_Port, _lora_handle->RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
    if (RFM98_Read(REG_VERSION) != 0x12) return 0;
    RFM98_SetMode(MODE_SLEEP);
    uint64_t frf = ((uint64_t)lora->frequency << 19) / 32000000;
    RFM98_Write(REG_FRF_MSB, (uint8_t)(frf >> 16));
    RFM98_Write(REG_FRF_MID, (uint8_t)(frf >> 8));
    RFM98_Write(REG_FRF_LSB, (uint8_t)(frf >> 0));
    RFM98_Write(REG_PA_CONFIG, 0x80 | (lora->power & 0x0F));
    RFM98_Write(REG_MODEM_CONFIG_1, lora->bandwidth | lora->codingRate | 0x01); // 0x01 = Explicit Header
    RFM98_Write(REG_MODEM_CONFIG_2, (lora->spreadingFactor << 4) | 0x04); // 0x04 = CRC On
    RFM98_SetMode(MODE_STDBY);
    return 1;
}

void LoRa_SendMessage(LoRa_HandleTypeDef* lora, uint8_t* data, uint8_t length) {
    _tx_done_flag = 0;
    RFM98_SetMode(MODE_STDBY);
    RFM98_Write(REG_DIO_MAPPING_1, 0x40); // DIO0 -> TxDone
    RFM98_Write(REG_FIFO_TX_BASE_ADDR, 0x00);
    RFM98_Write(REG_FIFO_ADDR_PTR, 0x00);
    for (int i = 0; i < length; i++) RFM98_Write(REG_FIFO, data[i]);
    RFM98_Write(REG_PAYLOAD_LENGTH, length);
    RFM98_SetMode(MODE_TX);
    while (_tx_done_flag == 0); // Interrupt'ı bekle
}

void LoRa_ReceiveMode(LoRa_HandleTypeDef* lora) {
    rx_done_flag = 0;
    RFM98_Write(REG_DIO_MAPPING_1, 0x00); // DIO0 -> RxDone
    RFM98_Write(REG_FIFO_RX_BASE_ADDR, 0x00);
    RFM98_Write(REG_FIFO_ADDR_PTR, 0x00);
    RFM98_SetMode(MODE_RX_CONTINUOUS);
}

uint8_t LoRa_ReadPacket(LoRa_HandleTypeDef* lora, uint8_t* buffer, uint8_t maxLength) {
    uint8_t len = RFM98_Read(REG_RX_NB_BYTES);
    if(len == 0 || len > maxLength) return 0;
    uint8_t start_addr = RFM98_Read(REG_FIFO_RX_CURRENT_ADDR);
    RFM98_Write(REG_FIFO_ADDR_PTR, start_addr);
    for (int i = 0; i < len; i++) buffer[i] = RFM98_Read(REG_FIFO);
    return len;
}

void LoRa_Handle_DIO0(LoRa_HandleTypeDef* lora) {
    uint8_t flags = RFM98_Read(REG_IRQ_FLAGS);
    RFM98_Write(REG_IRQ_FLAGS, 0xFF); // Tüm falgları temizle
    if (flags & IRQ_TX_DONE_MASK) _tx_done_flag = 1;
    if (flags & IRQ_RX_DONE_MASK) rx_done_flag = 1;
}