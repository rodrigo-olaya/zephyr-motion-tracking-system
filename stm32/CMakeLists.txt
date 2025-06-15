cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(stm32)

target_sources(app PRIVATE 
            src/main.c
            src/leds/led.c
            src/sensors/ir_sensor.c
            src/comm/spi.c
            src/sensors/mpu6050.c)