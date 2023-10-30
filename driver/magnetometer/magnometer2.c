#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

int main() {
    stdio_init_all();

    // Initialize I2C
    i2c_init(i2c0, 400000); // 400 kHz
    gpio_set_function(17, GPIO_FUNC_I2C);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_pull_up(17);
    gpio_pull_up(16);

    // LSM303DLHC ACCELERO I2C address is 0x32 (50) for writing, 0x33 (51) for reading
    uint8_t accelero_addr = 0x19;

    // LSM303DLHC MAGNETO I2C address is 0x3C (60) for writing, 0x3D (61) for reading
    uint8_t magneto_addr = 0x1E;

    // Select control register1 (0x20)
    // X, Y, and Z-axis enable, power on mode, output data rate 10 Hz (0x27)
    uint8_t config[2] = {0x20, 0x27};

    // Select control register4 (0x23)
    // Full scale +/- 2g, continuous update (0x00)
    uint8_t config_accel[2] = {0x23, 0x00};

    // Select MR register (0x02) for magnetometer
    // Continuous conversion (0x00)
    uint8_t config_magneto[2] = {0x02, 0x00};

    // Select CRA register (0x00)
    // Data output rate = 15Hz (0x10)
    uint8_t config_cra[2] = {0x00, 0x10};

    // Select CRB register (0x01)
    // Set gain = +/- 1.3g (0x20)
    uint8_t config_crb[2] = {0x01, 0x20};

    while (1) {
        // Initialize error flags
        bool accel_error = false;
        bool magneto_error = false;

        // Write accelerometer configuration
        if (i2c_write_blocking(i2c0, accelero_addr, config, 2, false) != PICO_ERROR_GENERIC) {
            if (i2c_write_blocking(i2c0, accelero_addr, config_accel, 2, false) == PICO_ERROR_GENERIC) {
                printf("Error: Failed to write accelerometer configuration\n");
                accel_error = true;
            }
        } else {
            printf("Error: Failed to communicate with accelerometer\n");
            accel_error = true;
        }
        sleep_ms(10); // Wait for accelerometer data

        // Read accelerometer data
        uint8_t reg_accel = 0x28; // X-axis lsb data register
        if (i2c_write_blocking(i2c0, accelero_addr, &reg_accel, 1, true) != PICO_ERROR_GENERIC) {
            uint8_t accel_data[6] = {0};
            if (i2c_read_blocking(i2c0, accelero_addr, accel_data, 6, false) != PICO_ERROR_GENERIC) {
                int16_t xAccl = (accel_data[1] << 8) | accel_data[0];
                if (xAccl > 32767) {
                    xAccl -= 65536;
                }

                int16_t yAccl = (accel_data[3] << 8) | accel_data[2];
                if (yAccl > 32767) {
                    yAccl -= 65536;
                }

                int16_t zAccl = (accel_data[5] << 8) | accel_data[4];
                if (zAccl > 32767) {
                    zAccl -= 65536;
                }

                // Output accelerometer data
                printf("Acceleration in X-Axis: %d\n", xAccl);
                printf("Acceleration in Y-Axis: %d\n", yAccl);
                printf("Acceleration in Z-Axis: %d\n", zAccl);
            } else {
                printf("Error: Failed to read accelerometer data\n");
                accel_error = true;
            }
        } else {
            printf("Error: Failed to communicate with accelerometer\n");
            accel_error = true;
        }

        // Write magnetometer configuration
        if (i2c_write_blocking(i2c0, magneto_addr, config_magneto, 2, false) != PICO_ERROR_GENERIC) {
            if (i2c_write_blocking(i2c0, magneto_addr, config_cra, 2, false) == PICO_ERROR_GENERIC) {
                printf("Error: Failed to write magnetometer configuration\n");
                magneto_error = true;
            }
        } else {
            printf("Error: Failed to communicate with magnetometer\n");
            magneto_error = true;
        }
        sleep_ms(10); // Wait for magnetometer data
        

        // Read magnetometer data
        uint8_t reg_magneto = 0x03; // X-axis msb data register
        if (i2c_write_blocking(i2c0, magneto_addr, &reg_magneto, 1, true) != PICO_ERROR_GENERIC) {
            uint8_t magneto_data[6] = {0};
            if (i2c_read_blocking(i2c0, magneto_addr, magneto_data, 6, false) != PICO_ERROR_GENERIC) {
                int16_t xMag = (magneto_data[0] << 8) | magneto_data[1];
                if (xMag > 32767) {
                    xMag -= 65536;
                }

                int16_t yMag = (magneto_data[2] << 8) | magneto_data[3];
                if (yMag > 32767) {
                    yMag -= 65536;
                }

                int16_t zMag = (magneto_data[4] << 8) | magneto_data[5];
                if (zMag > 32767) {
                    zMag -= 65536;
                }

                // Output magnetometer data
                printf("Magnetic field in X-Axis: %d\n", xMag);
                printf("Magnetic field in Y-Axis: %d\n", yMag);
                printf("Magnetic field in Z-Axis: %d\n", zMag);
            } else {
                printf("Error: Failed to read magnetometer data\n");
                magneto_error = true;
            }
        } else {
            printf("Error: Failed to communicate with magnetometer\n");
            magneto_error = true;
        }

        // Check for potential errors
        if (accel_error || magneto_error) {
            printf("Potential errors detected.\n");
        }

        sleep_ms(1000); // Wait for the next reading
    }

    return 0;
}
