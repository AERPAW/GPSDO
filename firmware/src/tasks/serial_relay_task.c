#include "serial_relay_task.h"
#include "tusb_config.h"
#include "class/cdc/cdc_device.h"
#include "class/cdc/cdc.h"
#include "../config.h"

void serial_relay_task(uart_inst_t* uart, int cdc, blinking_led_task_t* led_task) {
    // If the CDC device is not connected, do nothing
    if (!tud_cdc_n_connected(cdc)) { return; }

    // Check if uart data is available
    if (uart_is_readable(uart)) {
        // Blink the LED
        blinking_led_task_trigger(led_task);

        // Read available data
        char buf[BUFFER_SIZE];
        int len = 0;
        while (uart_is_readable(uart) && len < BUFFER_SIZE) {
            uart_read_blocking(uart, &buf[len++], 1);
        }

        // Send to the CDC device
        tud_cdc_n_write(cdc, buf, len);
        tud_cdc_n_write_flush(cdc);
    }

    // Check if CDC data is available
    if (tud_cdc_n_available(cdc)) {
        // Blink the LED
        blinking_led_task_trigger(led_task);

        // Read available data
        char buf[BUFFER_SIZE];
        int len = tud_cdc_n_read(cdc, buf, BUFFER_SIZE);

        // Send to the uart
        uart_write_blocking(uart, buf, len);
    }
}