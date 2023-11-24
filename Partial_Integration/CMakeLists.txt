add_executable(Partial_Integration
        Partial_Integration.c
        )

target_link_libraries(Partial_Integration pico_stdlib hardware_pwm)

pico_add_extra_outputs(Partial_Integration)
pico_enable_stdio_usb(Partial_Integration 1)

# add url via pico_set_program_url
example_auto_set_url(Partial_Integration)

