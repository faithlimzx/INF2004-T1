#include <stdio.h>
#include "pico/stdlib.h" 
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

// Define GPIO pins for ULTRASONIC SENSOR  
#define TRIGGER_PIN 12
#define ECHO_PIN 13

// Define GPIO pins for PWM SIGNALS and MOTOR DIRECTIONS
#define motorPWMR 0
#define motorPWML 1

#define motorDirR01 3
#define motorDirR02 4
#define motorDirL01 6
#define motorDirL02 5

// Define GPIO pins for IR SENSORS
#define IR_SENSOR_LEFT 26
#define IR_SENSOR_RIGHT 27
#define IR_SENSOR_BOTTOM 28

// Define GPIO pin for ENCODER OUTPUT 
#define ENCODER_OUT_PIN 2 

// PWM Slices
uint slice_num01;
uint slice_num02;

// Variable to count the ENCODER PULSES 
volatile int pulseCount = 0; 

float get_distance() 
{
    // Triggering the ULTRASONIC SENSOR 
    gpio_put(TRIGGER_PIN, 1);
    sleep_us(10);
    gpio_put(TRIGGER_PIN, 0);

    // Measuring the pulse duration from the ECHO PIN
    while (gpio_get(ECHO_PIN) == 0) {}
    uint32_t start_time = time_us_32();
    while (gpio_get(ECHO_PIN) == 1) {}
    uint32_t end_time = time_us_32();

    // Calculate distance in CM
    float pulse_duration = (float)(end_time - start_time);
    float distance = (pulse_duration / 2) * 0.000343 * 100;

    return distance;
}

void gpio_ultrasonic_initialization() 
{
    // Initialize GPIO pins for ULTRASONIC SENSOR 
    gpio_init(TRIGGER_PIN);
    gpio_init(ECHO_PIN);

    // Set GPIO pins to output(trigger) and input(echo) for ULTRASONIC SENSOR 
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    // Set the ULTRASONIC SENSOR to logic low 
    gpio_put(TRIGGER_PIN, 0);

    sleep_ms(500);  // Allow the ULTRASONIC SENSOR to settle
}

void encoder_pulse_handler() 
{
    // Increment pulse count on each edge transition 
    pulseCount++;
}

void setup_encoder_interrupt() 
{
    // Configuring interrupt on the GPIO pin ENCODER_OUT_PIN to trigger when a falling edge is detected
    gpio_set_irq_enabled_with_callback(ENCODER_OUT_PIN, GPIO_IRQ_EDGE_FALL, true, &encoder_pulse_handler);
}

void gpio_encoder_initialization()
{
    // Initialize GPIO pins for ENCODER 
    gpio_init(ENCODER_OUT_PIN);

    // Set GPIO directions to input for ENCODER 
    gpio_set_dir(ENCODER_OUT_PIN, GPIO_IN);
}
   
void gpio_ir_sensor_initialization()
{
    // Initialize GPIO pins for IR SENSORS
    gpio_init(IR_SENSOR_LEFT);
    gpio_init(IR_SENSOR_RIGHT);
    gpio_init(IR_SENSOR_BOTTOM);

    // Set GPIO directions to input for IR SENSORS
    gpio_set_dir(IR_SENSOR_LEFT, GPIO_IN);
    gpio_set_dir(IR_SENSOR_RIGHT, GPIO_IN);
    gpio_set_dir(IR_SENSOR_BOTTOM, GPIO_IN);
}

void gpio_motor_initialization()
{
    // GPIO initialization for MOTOR CONTROL
    gpio_init(motorDirR01);
    gpio_init(motorDirR02);
    gpio_init(motorDirL01);
    gpio_init(motorDirL02);

    // Set GPIO directions to output for MOTOR CONTROL
    gpio_set_dir(motorDirR01, GPIO_OUT);
    gpio_set_dir(motorDirR02, GPIO_OUT);
    gpio_set_dir(motorDirL01, GPIO_OUT);
    gpio_set_dir(motorDirL02, GPIO_OUT);

    // Set GPIO functions for PWM
    gpio_set_function(motorPWMR, GPIO_FUNC_PWM);
    gpio_set_function(motorPWML, GPIO_FUNC_PWM);

    // Assigning variables for PWM slices 
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

// Function to print statements when IR SENSORS detect a black line
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

// Function to STOP the robot car 
void move_stop()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 0);
    gpio_put(motorDirR01, 0);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 0);
    gpio_put(motorDirL02, 0);
}

// Function to drive the robot car FORWARDS
void move_forward()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 1.5);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 1.5);
    gpio_put(motorDirR01, 1);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 1);
    gpio_put(motorDirL02, 0);
}

// Function to drive the robot car BACKWARDS 
void move_backward()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 1.5);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 1.5);
    gpio_put(motorDirR01, 0);
    gpio_put(motorDirR02, 1);
    gpio_put(motorDirL01, 0);
    gpio_put(motorDirL02, 1);
}

// Function to turn the robot car RIGHT
void move_forward_right()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 1.5);
    gpio_put(motorDirR01, 1);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 1);
    gpio_put(motorDirL02, 0);
}

// Function to turn the robot car LEFT
void move_forward_left()
{
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 1.5);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 0);
    gpio_put(motorDirR01, 1);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 1);
    gpio_put(motorDirL02, 0);
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
        printf("Pulse Count: %d\n", pulseCount);

        // Print statements for IR sensors
        printIRSensorStatus();

        // Print statements for ULTRASONIC SENSOR
        float distance = get_distance();
        printf("Distance: %.2f cm\n", distance);

        // Set ULTRASONIC SENSOR threshold distance to REVERSE robot car  
        float threshold_distance = 10;  

        // Robot logic 
        if (distance < threshold_distance) 
        {
            // ULTRASONIC SENSOR threshold reached: REVERSE robot car for 1.2 seconds
            move_backward(); 
            sleep_ms(1200);    
            move_stop();
        }

        else if (!gpio_get(IR_SENSOR_LEFT) && !gpio_get(IR_SENSOR_RIGHT))
        {
            // Neither IR SENSOR on the line: robot car moves FORWARDS
            move_forward();
        }
        else if (!gpio_get(IR_SENSOR_LEFT) && gpio_get(IR_SENSOR_RIGHT))
        {
            // Right IR SENSOR on the line: robot car turns LEFT
            move_forward_left(); 
            sleep_ms(400);
        }
        else if (gpio_get(IR_SENSOR_LEFT) && !gpio_get(IR_SENSOR_RIGHT))
        {
            // Left IR SENSOR on the line: robot car turns RIGHT
            move_forward_right(); 
            sleep_ms(400);
        }
        else if (gpio_get(IR_SENSOR_LEFT) && gpio_get(IR_SENSOR_RIGHT))
        {
            // Both IR SENSORS on the line: robot car moves BACKWARDS for 1.2 seconds
            move_backward(); 
            sleep_ms(1200);      
        }
    }

    return 0;
}