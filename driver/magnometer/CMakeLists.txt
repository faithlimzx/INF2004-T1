add_executable(magnometer2 magnometer2.c)

target_link_libraries(magnometer2 pico_stdlib hardware_i2c hardware_pwm)

pico_enable_stdio_usb(magnometer2 1) # Enable USB serial
pico_enable_stdio_uart(magnometer2 0) # Disable uart

# create map/bin/hex file etc.
pico_add_extra_outputs(magnometer2)

# add url via pico_set_program_url
example_auto_set_url(magnometer2)