#pragma once
#include <hardware/uart.h>

#define GNSS_CDC_IFACE      0
#define GNSS_UART           uart0
#define GNSS_TX_PIN         0
#define GNSS_RX_PIN         1
#define GNSS_DATA_LED_PIN   27

#define OCXO_CDC_IFACE      1
#define OCXO_UART           uart1
#define OCXO_TX_PIN         24
#define OCXO_RX_PIN         25
#define OCXO_1PPS_PIN       19
#define OCXO_LOCK_PIN       20
#define OCXO_DATA_LED_PIN   26
#define OCXO_1PPS_LED_PIN   28
#define OCXO_LOCK_LED_PIN   29


// Note: this is set to avoid uart writes taking too long
#define DEFAULT_GNSS_BAUDRATE   38400
#define DEFAULT_OCXO_BAUDRATE   115200
#define BUFFER_SIZE             64