#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

#define IR_SENSOR_NODE DT_ALIAS(irsensor)

#if !DT_NODE_HAS_STATUS_OKAY(IR_SENSOR_NODE)
#error "Unsupported board: irsensor devicetree alias is not defined"
#endif

struct sensor_read_t {
	void *fifo_reserved; /* 1st word reserved for use by fifo */
	uint32_t status;
};

struct ir_sensor {
	struct gpio_dt_spec spec;
	uint8_t num;
};

void ir_sensor_read(const struct ir_sensor *ir_sensor, struct k_fifo *led0_fifo, struct k_fifo *led1_fifo, struct k_fifo *spi_fifo);

#endif // IR_SENSOR_H