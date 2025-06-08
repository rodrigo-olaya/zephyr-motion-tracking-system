#include "led.h"
#include "../sensors/ir_sensor.h"


void blink(const struct led *led, struct k_fifo *led_fifo, uint32_t id)
{
	const struct gpio_dt_spec *spec = &led->spec;
	int cnt = 0;
	int ret;

	if (!gpio_is_ready_dt(spec)) {
		return;
	}

	ret = gpio_pin_configure_dt(spec, GPIO_OUTPUT);
	if (ret != 0) {
		return;
	}

	while (1) {
		struct sensor_read_t *sensor_status;

		if (led->num == 0){
			sensor_status = k_fifo_get(led_fifo, K_MSEC(100));
		}
		else if (led->num == 1){
			sensor_status = k_fifo_get(led_fifo, K_MSEC(100));
		}

		if (sensor_status == NULL) {
			continue;
		}

		if (led->num == 0 && sensor_status->status == 1) {
			gpio_pin_set_dt(spec, 0);
		} else if (led->num == 0 && sensor_status->status == 0) {
			gpio_pin_set_dt(spec, 1);
		}

		if (led->num == 1 && sensor_status->status == 1) {
			gpio_pin_set_dt(spec, 1);
		} else if (led->num == 1 && sensor_status->status == 0) {
			gpio_pin_set_dt(spec, 0);
		}

		k_free(sensor_status);
		k_yield();
	}
}