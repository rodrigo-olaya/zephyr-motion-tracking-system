#include "ir_sensor.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

void ir_sensor_read(const struct ir_sensor *ir_sensor, struct k_fifo *led0_fifo, struct k_fifo *led1_fifo)
{
	const struct gpio_dt_spec *spec = &ir_sensor->spec;
	struct sensor_read_t sensor_read;

	int ret;
	if (!gpio_is_ready_dt(spec)){
		return;
	}

	ret = gpio_pin_configure_dt(spec, GPIO_INPUT);
	if (ret < 0) {
		return;
	}

	while (1) {
		ret = gpio_pin_get_dt(spec);
		if (ret < 0) {
			k_msleep(10); // Small delay before retrying
            continue;
		}
		else if (ret == 1) {
			sensor_read.status = 1; // Sensor detected
		}
		else if (ret == 0)
		{
			sensor_read.status = 0; // Sensor not detected
		}
		size_t size = sizeof(struct sensor_read_t);
		char *mem_ptr0 = k_malloc(size);
		char *mem_ptr1 = k_malloc(size);
		
		if (mem_ptr0 == NULL || mem_ptr1 == NULL) {
			k_msleep(10);
            continue;
		}
		memcpy(mem_ptr0, &sensor_read, size);
		memcpy(mem_ptr1, &sensor_read, size);

		k_fifo_put(led0_fifo, mem_ptr0);
		k_fifo_put(led1_fifo, mem_ptr1);
	}
}