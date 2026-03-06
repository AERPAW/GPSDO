#pragma once
#include <pico/stdlib.h>
#include <pico/multicore.h>

struct blinking_led_task {
    int led_pin;
    uint64_t blink_len_us;
    uint64_t trig_time;
    mutex_t trig_time_mtx;
};
typedef struct blinking_led_task blinking_led_task_t;

/**
 * Initialize a led blining task.
 * @param task Task to initialize.
 * @param led_pin Pin of the LED to blink.
 * @param blink_len_ms Duration of a blink in milliseconds.
*/
void blinking_led_task_init(blinking_led_task_t* task, int led_pin, int blink_len_ms);

/**
 * Run the led blinking task. This function does not block.
 * @param task Task to run.
*/
void blinking_led_task_run(blinking_led_task_t* task);

/**
 * Trigger the LED to blink. If the LED is already on, it will simply extend the duration of the blink.
 * @param task Task of the LED to blink.
*/
void blinking_led_task_trigger(blinking_led_task_t* task);