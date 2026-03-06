#include <tusb.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <hardware/irq.h>
#include "tusb_config.h"
#include "class/cdc/cdc_device.h"
#include "class/cdc/cdc.h"
#include "config.h"
#include "tasks/serial_relay_task.h"
#include "tasks/blinking_led_task.h"

blinking_led_task_t gnss_data_led_task;
blinking_led_task_t ocxo_data_led_task;
blinking_led_task_t ocxo_1pps_led_task;

volatile bool irq_fired = false;
void pps_irq() {
    // Set the IRQ fired flag
    irq_fired = true;
}

void core1() {
    // Register the PPS interrupt handler
    gpio_set_irq_enabled_with_callback(OCXO_1PPS_PIN, GPIO_IRQ_EDGE_RISE, true, pps_irq);

    // Run the other tasks
    while (true) {
        // Check if the IRQ has fired
        if (irq_fired) {
            blinking_led_task_trigger(&ocxo_1pps_led_task);
            irq_fired = false;
        }

        // Run the LED blinking tasks
        blinking_led_task_run(&gnss_data_led_task);
        blinking_led_task_run(&ocxo_data_led_task);
        blinking_led_task_run(&ocxo_1pps_led_task);
        
        // Update the lock LED
        gpio_put(OCXO_LOCK_LED_PIN, gpio_get(OCXO_LOCK_PIN));
    }
}

int main() {
    // Initialize the two UARTs
    uart_init(GNSS_UART, DEFAULT_GNSS_BAUDRATE);
    uart_init(OCXO_UART, DEFAULT_OCXO_BAUDRATE);
    
    // Configure the uart pins
    gpio_set_function(GNSS_TX_PIN, UART_FUNCSEL_NUM(GNSS_UART, GNSS_TX_PIN));
    gpio_set_function(GNSS_RX_PIN, UART_FUNCSEL_NUM(GNSS_UART, GNSS_RX_PIN));
    gpio_set_function(OCXO_TX_PIN, UART_FUNCSEL_NUM(OCXO_UART, OCXO_TX_PIN));
    gpio_set_function(OCXO_RX_PIN, UART_FUNCSEL_NUM(OCXO_UART, OCXO_RX_PIN));

    // Initialize the 1PPS and lock inputs
    gpio_init(OCXO_1PPS_PIN);
    gpio_set_dir(OCXO_1PPS_PIN, false);
    gpio_set_dir(OCXO_LOCK_PIN, false);

    // Initialize the status LEDs
    gpio_init(GNSS_DATA_LED_PIN);
    gpio_init(OCXO_DATA_LED_PIN);
    gpio_init(OCXO_1PPS_LED_PIN);
    gpio_init(OCXO_LOCK_LED_PIN);
    gpio_set_dir(GNSS_DATA_LED_PIN, true);
    gpio_set_dir(OCXO_DATA_LED_PIN, true);
    gpio_set_dir(OCXO_1PPS_LED_PIN, true);
    gpio_set_dir(OCXO_LOCK_LED_PIN, true);
    gpio_put(GNSS_DATA_LED_PIN, 0);
    gpio_put(OCXO_DATA_LED_PIN, 0);
    gpio_put(OCXO_1PPS_LED_PIN, 0);
    gpio_put(OCXO_LOCK_LED_PIN, 0);

    // Initialize the LED blinking tasks
    blinking_led_task_init(&gnss_data_led_task, GNSS_DATA_LED_PIN, 100);
    blinking_led_task_init(&ocxo_data_led_task, OCXO_DATA_LED_PIN, 100);
    blinking_led_task_init(&ocxo_1pps_led_task, OCXO_1PPS_LED_PIN, 100);

    // Initialize TinyUSB
    tusb_init();

    // Start the second core
    multicore_launch_core1(core1);

    // Run USB task
    while (true) {
        // TinyUSB task
        tud_task();

        // Relay the serial ports
        serial_relay_task(GNSS_UART, GNSS_CDC_IFACE, &gnss_data_led_task);
        serial_relay_task(OCXO_UART, OCXO_CDC_IFACE, &ocxo_data_led_task);
    }

    return 0;
}

// uart_parity_t PARITY_MAP[] = {
//     UART_PARITY_NONE,
//     UART_PARITY_ODD,
//     UART_PARITY_EVEN,
//     UART_PARITY_NONE, // NOTE: These will not work
//     UART_PARITY_NONE  // NOTE: These will not work
// };

// // Handler called by TinyUSB when the interface
// void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* line_coding) {
//     // Handle depending on the interface
//     switch (itf) {
//     case GNSS_CDC_IFACE:
//         uart_set_baudrate(GNSS_UART, line_coding->bit_rate);
//         uart_set_format(GNSS_UART, line_coding->data_bits, line_coding->stop_bits, PARITY_MAP[line_coding->parity]);
//         break;
//     case OCXO_CDC_IFACE:
//         uart_set_baudrate(OCXO_UART, line_coding->bit_rate);
//         uart_set_format(OCXO_UART, line_coding->data_bits, line_coding->stop_bits, PARITY_MAP[line_coding->parity]);
//         break;
//     }
// }