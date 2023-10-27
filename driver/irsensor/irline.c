#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/time.h"

#define IR_SENSOR_PIN 26
#define ADC_SAMPLE_INTERVAL_MS 25  // 25 ms

volatile uint16_t ir_sensor_value;

void adc_setup() {
    adc_init();
    adc_gpio_init(IR_SENSOR_PIN);
}

bool timer_callback(struct repeating_timer *t) {
    // Perform ADC reading from the IR sensor
    adc_select_input(0); // Use ADC channel 0, you can change this if needed
    ir_sensor_value = adc_read();

    // Print IR sensor value with a 25 ms interval
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    uint32_t milliseconds = current_time / 1000;

    printf("%02d:%02d:%02d.%03d -> IR Sensor Value: %d\n",
           (int)(milliseconds / 3600000) % 24,
           (int)((milliseconds / 60000) % 60),
           (int)((milliseconds / 1000) % 60),
           (int)(milliseconds % 1000),
           ir_sensor_value);

    return true;
}

int main() {
    stdio_init_all();
    adc_setup();

    // Set up a timer interrupt for IR sensor sampling every 25 ms
    struct repeating_timer timer;
    add_repeating_timer_ms(ADC_SAMPLE_INTERVAL_MS, timer_callback, NULL, &timer);

    while (true) {
    }

    return 0;
}
