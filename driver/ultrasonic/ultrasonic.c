#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

int timeout = 30000; // Increase the timeout to handle longer distances

void setupUltrasonicPins(uint trigPin, uint echoPin)
{
    gpio_init(trigPin);
    gpio_init(echoPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

uint64_t getPulse(uint trigPin, uint echoPin)
{
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);

    uint64_t width = 0;

    while (gpio_get(echoPin) == 0) tight_loop_contents();
    absolute_time_t startTime = get_absolute_time();
    while (gpio_get(echoPin) == 1)
    {
        width++;
        sleep_us(1);
        if (width > timeout)
            return 0;
    }
    absolute_time_t endTime = get_absolute_time();

    return absolute_time_diff_us(startTime, endTime);
}

float getCm(uint trigPin, uint echoPin)
{
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    if (pulseLength == 0)
        return -1.0; // Error condition

    // Calculate the distance in centimeters
    float distance_cm = (float)pulseLength * 0.01715; // Speed of sound in air = 343 m/s

    return distance_cm;
}

int main()
{
    const uint trigPin = 12; 
    const uint echoPin = 13; 

    stdio_init_all();

    setupUltrasonicPins(trigPin, echoPin);

    while (1)
    {
        float distance_cm = getCm(trigPin, echoPin);
        if (distance_cm >= 0)
        {
            printf("Distance: %.2f cm\n", distance_cm);
        }
        else
        {
            printf("Error reading distance\n");
        }
        sleep_ms(1000); // Update the distance every second
    }

    return 0;
}
