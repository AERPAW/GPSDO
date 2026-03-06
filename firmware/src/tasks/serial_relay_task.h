#pragma once
#include <hardware/uart.h>
#include "blinking_led_task.h"

void serial_relay_task(uart_inst_t* uart, int cdc, blinking_led_task_t* led_task);