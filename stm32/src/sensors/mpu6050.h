#ifndef MPU6050_H
#define MPU6050_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include "../comm/spi.h"

#include <zephyr/drivers/uart.h>

#define MPU6050_NODE DT_ALIAS(mpu6050)

#define UART0_NODE DT_ALIAS(raspi4)

int IMU_read(const struct device *const mpu6050, struct k_fifo *spi_fifo, const struct device *const pi4);

#endif // MPU6050_H