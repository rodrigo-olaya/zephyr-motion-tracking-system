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

#include <zephyr/drivers/sensor.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY_SENSOR 6
#define PRIORITY_LED 7

/* Temp defs for MPU6050 */

#define MPU6050_NODE DT_ALIAS(mpu6050)

const struct device *const mpu6050 = DEVICE_DT_GET(MPU6050_NODE);

/* End of temp defs for MPU6050 */

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

/* Start of temp IMU code before thread refactoring */

int main(void){

	struct spi_fifo_t spi_fifo_data;

	if (!device_is_ready(mpu6050)) {
		printf("Device %s is not ready\n", mpu6050->name);
		return 0;
	}

	struct sensor_value acc_x_val;

	while (1){
		// first need to do sensor sample fetch
		int rc = sensor_sample_fetch(mpu6050);

		// then need to do sensor channel get
		rc = sensor_channel_get(mpu6050, SENSOR_CHAN_ACCEL_X, &acc_x_val);

		// use sensor_value_to_double to give reasonable output
		double acc_x = sensor_value_to_double(&acc_x_val);

		char buffer[19];
		size_t buffer_size = sizeof(buffer);

		int int_part = (int)acc_x;
		int frac_part = (int)((acc_x - int_part) * 100);
		snprintf(buffer, buffer_size, "The value is: %d.%02d", int_part, frac_part);

		memcpy(spi_fifo_data.message, buffer, buffer_size);
		spi_fifo_data.len = buffer_size;

		size_t size_spi = sizeof(struct spi_fifo_t);
		char *spi_mem_ptr = k_malloc(size_spi);
		
		while (spi_mem_ptr == NULL) {
			k_msleep(10);
			spi_mem_ptr = k_malloc(size_spi);
			if (spi_mem_ptr != NULL) {
				break;
			}
			continue;
		}

		memcpy(spi_mem_ptr, &spi_fifo_data, size_spi);
		k_fifo_put(&spi_fifo, spi_mem_ptr);
	}

	return 0;
}

/* End of temp code before thread refactoring */

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