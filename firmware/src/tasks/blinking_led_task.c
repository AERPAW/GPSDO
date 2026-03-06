#include "blinking_led_task.h"

void blinking_led_task_init(blinking_led_task_t* task, int led_pin, int blink_len_ms) {
    // Copy over the settings
    task->led_pin = led_pin;
    task->blink_len_us = (uint64_t)blink_len_ms * 1000;

    // Reset the trigger time
    task->trig_time = 0;

    // Initialize the mutex
    mutex_init(&task->trig_time_mtx);
}

void blinking_led_task_run(blinking_led_task_t* task) {
    // Try to aquire the mutex or give up
    if (!mutex_try_enter(&task->trig_time_mtx, NULL)) { return; }

    // Get the trigger time
    uint64_t trig_time = task->trig_time;

    // Release the mutex
    mutex_exit(&task->trig_time_mtx);

    // Set the LED state depending on the trigger time
    gpio_put(task->led_pin, (time_us_64() - trig_time) < task->blink_len_us);
}

void blinking_led_task_trigger(blinking_led_task_t* task) {
    // Acquire the mutex
    mutex_enter_blocking(&task->trig_time_mtx);

    // Set the trigger time
    task->trig_time = time_us_64();

    // Release the mutex
    mutex_exit(&task->trig_time_mtx);
}