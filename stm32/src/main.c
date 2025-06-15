#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>
// #include <zephyr/drivers/spi.h>

#include "leds/led.h"
#include "sensors/ir_sensor.h"
#include "comm/spi.h"
#include "sensors/mpu6050.h"

#include <zephyr/drivers/sensor.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY_SENSOR 5
#define PRIORITY_LED 7
#define PRIORITY_MPU 6

K_FIFO_DEFINE(led0_fifo);
K_FIFO_DEFINE(led1_fifo);
K_FIFO_DEFINE(spi_fifo);

static const struct led led0 = {
	.spec = GPIO_DT_SPEC_GET(LED0_NODE, gpios),
	.num = 0,
};

static const struct led led1 = {
	.spec = GPIO_DT_SPEC_GET(LED1_NODE, gpios),
	.num = 1,
};

static const struct ir_sensor ir_sensor0 = {
	.spec = GPIO_DT_SPEC_GET(IR_SENSOR_NODE, gpios),
	// .num = 0,
};

static const struct device *spi_dev = DEVICE_DT_GET(SPI_NODE);

const struct device *const mpu6050 = DEVICE_DT_GET(MPU6050_NODE);

void blink0(void)
{
	blink(&led0, &led0_fifo, 0);
}

void blink1(void)
{
	blink(&led1, &led1_fifo, 1);
}

void ir_sensor_read0(void)
{
	ir_sensor_read(&ir_sensor0, &led0_fifo, &led1_fifo, &spi_fifo);
}

void spi_comm0(void)
{
    spi_comm(spi_dev, &spi_fifo);
}

void IMU_read0(void)
{
	IMU_read(mpu6050, &spi_fifo);
}

/*	First Thread  */
K_THREAD_DEFINE(blink0_id, STACKSIZE, blink0, NULL, NULL, NULL,
		PRIORITY_LED, 0, 0);

/*	Second Thread  */
K_THREAD_DEFINE(blink1_id, STACKSIZE, blink1, NULL, NULL, NULL,
		PRIORITY_LED, 0, 0);

/*  Third Thread  */
K_THREAD_DEFINE(ir_sensor_read0_id, STACKSIZE, ir_sensor_read0, NULL, NULL, NULL,
		PRIORITY_SENSOR, 0, 0);

/* Fourth Thread for SPI communication */
K_THREAD_DEFINE(spi_comm_id, STACKSIZE, spi_comm0, NULL, NULL, NULL,
        PRIORITY_SENSOR, 0, 0);

/* Fifth Thread for MPU6050 sensor reading */
K_THREAD_DEFINE(mpu6050_id, STACKSIZE, IMU_read0, NULL, NULL, NULL,
		PRIORITY_SENSOR, 0, 0);