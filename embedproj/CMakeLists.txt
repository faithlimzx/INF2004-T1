add_executable(embedproj
        irline.c
        )

target_link_libraries(embedproj pico_stdlib hardware_pwm)

pico_add_extra_outputs(embedproj)
pico_enable_stdio_usb(embedproj 1)

# add url via pico_set_program_url
example_auto_set_url(embedproj)

