#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

#include "leds/led.h"
#include "sensors/ir_sensor.h"

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY_SENSOR 6
#define PRIORITY_LED 7

K_FIFO_DEFINE(led0_fifo);
K_FIFO_DEFINE(led1_fifo);

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
	.num = 0,
};

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
	ir_sensor_read(&ir_sensor0, &led0_fifo, &led1_fifo);
}

/*	First Thread  */
K_THREAD_DEFINE(blink0_id, STACKSIZE, blink0, NULL, NULL, NULL,
		PRIORITY_LED, 0, 0);

/*	Second Thread  */
K_THREAD_DEFINE(blink1_id, STACKSIZE, blink1, NULL, NULL, NULL,
		PRIORITY_LED, 0, 0);

/*  Third Thread  */
K_THREAD_DEFINE(uart_out_id, STACKSIZE, ir_sensor_read0, NULL, NULL, NULL,
		PRIORITY_SENSOR, 0, 0);