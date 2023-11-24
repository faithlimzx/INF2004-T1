#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

// Define GPIO pins for Ultrasonic sensor 
#define TRIGGER_PIN 12
#define ECHO_PIN 13

// Define GPIO pins for PWM signals, motor directions, etc.
#define motorPWMR 0
#define motorPWML 1

#define motorDirR01 3
#define motorDirR02 4
#define motorDirL01 6
#define motorDirL02 5

// Define GPIO pins for IR sensors
#define IR_SENSOR_LEFT 26
#define IR_SENSOR_RIGHT 27
#define IR_SENSOR_BOTTOM 28

// Define GPIO pin for the encoder output
#define ENCODER_OUT_PIN 2 

uint slice_num01;
uint slice_num02;

volatile int pulseCount = 0; // Variable to count the encoder pulses

float get_distance() 
{
    // Triggering the ultrasonic sensor
    gpio_put(TRIGGER_PIN, 1);
    sleep_us(10);
    gpio_put(TRIGGER_PIN, 0);

    // Measuring the pulse duration from the echo pin
    while (gpio_get(ECHO_PIN) == 0) {}
    uint32_t start_time = time_us_32();
    while (gpio_get(ECHO_PIN) == 1) {}
    uint32_t end_time = time_us_32();

    // Calculate distance in meters
    float pulse_duration = (float)(end_time - start_time);
    float distance = (pulse_duration / 2) * 0.000343 * 100;

    return distance;
}

void gpio_ultrasonic_initialization() 
{
    gpio_init(TRIGGER_PIN);
    gpio_init(ECHO_PIN);

    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    gpio_put(TRIGGER_PIN, 0);

    sleep_ms(500);  // Allow the sensor to settle
}

void encoder_pulse_handler() 
{
    // Increment pulse count on each edge transition (e.g., rising or falling edge)
    pulseCount++;
}

void setup_encoder_interrupt() 
{
    gpio_set_irq_enabled_with_callback(ENCODER_OUT_PIN, GPIO_IRQ_EDGE_FALL, true, &encoder_pulse_handler);
    // irq_set_exclusive_handler(GPIO_IRQ_0, encoder_pulse_handler);
    // irq_set_enabled(ENCODER_OUT_PIN, true);
}

// Configure GPIO pin for the encoder output
void gpio_encoder_initialization()
{
    // Initialize GPIO pins for encoder
    gpio_init(ENCODER_OUT_PIN);

    // Set GPIO directions to input
    gpio_set_dir(ENCODER_OUT_PIN, GPIO_IN);
}
   
void gpio_ir_sensor_initialization()
{
    // Initialize GPIO pins for IR sensors
    gpio_init(IR_SENSOR_LEFT);
    gpio_init(IR_SENSOR_RIGHT);

    // Set GPIO directions to input
    gpio_set_dir(IR_SENSOR_LEFT, GPIO_IN);
    gpio_set_dir(IR_SENSOR_RIGHT, GPIO_IN);
}

// Function to initialize motor control
void gpio_motor_initialization()
{

    // GPIO initialization
    gpio_init(motorDirR01);
    gpio_init(motorDirR02);
    gpio_init(motorDirL01);
    gpio_init(motorDirL02);

    // Set GPIO directions to output
    gpio_set_dir(motorDirR01, GPIO_OUT);
    gpio_set_dir(motorDirR02, GPIO_OUT);
    gpio_set_dir(motorDirL01, GPIO_OUT);
    gpio_set_dir(motorDirL02, GPIO_OUT);

    // Set GPIO functions for PWM
    gpio_set_function(motorPWMR, GPIO_FUNC_PWM);
    gpio_set_function(motorPWML, GPIO_FUNC_PWM);

    slice_num01 = pwm_gpio_to_slice_num(motorPWMR);
    slice_num02 = pwm_gpio_to_slice_num(motorPWML);

    // Configure PWM settings (clock divider and wrap value)
    pwm_set_clkdiv(slice_num01, 100);
    pwm_set_wrap(slice_num01, 12500);
    pwm_set_clkdiv(slice_num02, 100);
    pwm_set_wrap(slice_num02, 12500);

    // Enable PWM slices
    pwm_set_enabled(slice_num01, true);
    pwm_set_enabled(slice_num02, true);
}

// Function to print statements when IR sensors detect a black line
void printIRSensorStatus()
{
    if (gpio_get(IR_SENSOR_LEFT))
    {
        printf("Left IR Sensor: Black line detected!\n");
    }
    else
    {
        printf("Left IR Sensor: No black line detected.\n");
    }

    if (gpio_get(IR_SENSOR_RIGHT))
    {
        printf("Right IR Sensor: Black line detected!\n");
    }
    else
    {
        printf("Right IR Sensor: No black line detected.\n");
    }
}

void move_stop()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 0);
    gpio_put(motorDirR01, 0);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 0);
    gpio_put(motorDirL02, 0);
}

void move_forward()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 1.5);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 1.5);
    gpio_put(motorDirR01, 1);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 1);
    gpio_put(motorDirL02, 0);
}

void move_backward()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 1.5);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 1.5);
    gpio_put(motorDirR01, 0);
    gpio_put(motorDirR02, 1);
    gpio_put(motorDirL01, 0);
    gpio_put(motorDirL02, 1);
}

void move_forward_right()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 1.5);
    gpio_put(motorDirR01, 1);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 1);
    gpio_put(motorDirL02, 0);
}

void move_forward_left()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 1.5);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 0);
    gpio_put(motorDirR01, 1);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 1);
    gpio_put(motorDirL02, 0);
}

void move_backward_right()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 1);
    gpio_put(motorDirR01, 0);
    gpio_put(motorDirR02, 1);
    gpio_put(motorDirL01, 0);
    gpio_put(motorDirL02, 1);
}

int main()
{
    stdio_init_all();

    setup_encoder_interrupt();

    gpio_encoder_initialization();

    gpio_ir_sensor_initialization();

    gpio_motor_initialization();

    gpio_ultrasonic_initialization();

    while (1)
    {
        // Print statements for encoder
        // printf("Pulse Count: %d\n", pulseCount);

        // Print statements for IR sensors
        // printIRSensorStatus();

        // Print statements for Ultrasonic sensor 
        float distance = get_distance();
        printf("Distance: %.2f cm\n", distance);

        // Set a threshold distance to stop and reverse for Ultrasonic sensor 
        float threshold_distance = 10;  // Adjust this value based on your needs

        // Robot logic 
        if (distance < threshold_distance) 
        {
            // Ultrasonic threshold reached: reverse for 1.2 seconds
            move_backward(); 
            sleep_ms(1200);   
            move_stop(); 
        }

        else if (!gpio_get(IR_SENSOR_LEFT) && !gpio_get(IR_SENSOR_RIGHT))
        {
            // Neither sensor on the line: travel straight
            move_forward();
        }
        else if (!gpio_get(IR_SENSOR_LEFT) && gpio_get(IR_SENSOR_RIGHT))
        {
            // Right sensor on the line: turn left
            move_forward_left(); 
            sleep_ms(400);
        }
        else if (gpio_get(IR_SENSOR_LEFT) && !gpio_get(IR_SENSOR_RIGHT))
        {
            // Left sensor on the line: turn right
            move_forward_right(); 
            sleep_ms(400);
        }
        else if (gpio_get(IR_SENSOR_LEFT) && gpio_get(IR_SENSOR_RIGHT))
        {
            // Both sensors on the black line: reverse for 1.2 second
            move_stop();
            move_backward(); 
            sleep_ms(1200);   
            move_stop();    

            // Sweep motion to detect black lines after reversing
            // for (int i = 0; i < 1; ++i)
            // {
            //     // Perform a sweeping motion
            //     // For example, you can implement a left-to-right sweeping motion here
            //     move_forward_left(); // Example: Turn left while moving forward
            //     sleep_ms(200);       // Adjust the duration as needed for the sweep
            //     move_forward_right();
            //     sleep_ms(200);
            //     move_stop(); // Stop the movement briefly before next step

            //     // Check the line-following logic conditions
            //     if (gpio_get(IR_SENSOR_LEFT) || gpio_get(IR_SENSOR_RIGHT))
            //     {
            //         // If the line is detected while sweeping, break the loop
            //         break;
            //     }
            // }
        }
    }

    return 0;
}