add_executable(irline irline.c)

# pull in common dependencies
target_link_libraries(irline pico_stdlib 
hardware_pwm 
hardware_adc 
hardware_timer
pico_time)

# enable usb output, disable uart output
pico_enable_stdio_usb(irline 1)
pico_enable_stdio_uart(irline 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(irline)

# add url via pico_set_program_url
example_auto_set_url(irline)
