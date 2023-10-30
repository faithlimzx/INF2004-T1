#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define ENCODER_PIN 2  // GPIO pin connected to the wheel encoder //

// Define the properties of your wheel and encoder
#define PPR 360       // Example: 360 pulses per wheel revolution
#define WHEEL_CIRCUMFERENCE 21.0  // Example: 21 cm

volatile uint32_t pulse_count = 0;
volatile uint32_t last_pulse_time = 0;

void encoder_isr() {
    pulse_count++;
    uint32_t current_time = time_us_32();
    uint32_t time_since_last_pulse = current_time - last_pulse_time;
    
    if (time_since_last_pulse > 0) {
        float speed_hz = 1.0 / (time_since_last_pulse * 1e-6);  // Speed in Hz
        float speed_cm_per_sec = speed_hz * (WHEEL_CIRCUMFERENCE / PPR);  // Speed in cm/s
        float distance_cm = (float)pulse_count * (WHEEL_CIRCUMFERENCE / PPR);  // Distance in cm
        printf("Distance: %.2f cm, Speed: %.2f cm/s\n", distance_cm, speed_cm_per_sec);
    }
    
    last_pulse_time = current_time;
}

int main() {
    stdio_init_all();
    gpio_init(ENCODER_PIN);
    gpio_set_dir(ENCODER_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &encoder_isr);
    
    while (1) {
        tight_loop_contents();
    }
    
    return 0;
}
