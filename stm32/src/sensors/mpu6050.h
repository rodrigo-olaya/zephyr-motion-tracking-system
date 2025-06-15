#ifndef MPU6050_H
#define MPU6050_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include "../comm/spi.h"

/* Temp defs for MPU6050 */

#define MPU6050_NODE DT_ALIAS(mpu6050)

/* End of temp defs for MPU6050 */

int IMU_read(const struct device *const mpu6050, struct k_fifo *spi_fifo);

#endif // MPU6050_H