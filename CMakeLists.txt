cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(zephyr-alarm-system)

target_sources(app PRIVATE 
            src/main.c
            src/leds/led.c
            src/sensors/ir_sensor.c
            src/comm/spi.c)