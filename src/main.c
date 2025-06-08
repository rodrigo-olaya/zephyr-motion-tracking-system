#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY_SENSOR 6
#define PRIORITY_LED 7

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define IR_SENSOR_NODE DT_ALIAS(irsensor)

#if !DT_NODE_HAS_STATUS_OKAY(LED0_NODE)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS_OKAY(LED1_NODE)
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS_OKAY(IR_SENSOR_NODE)
#error "Unsupported board: irsensor devicetree alias is not defined"
#endif

struct sensor_read_t {
	void *fifo_reserved; /* 1st word reserved for use by fifo */
	uint32_t status;
};

K_FIFO_DEFINE(led0_fifo);
K_FIFO_DEFINE(led1_fifo);

struct led {
	struct gpio_dt_spec spec;
	uint8_t num;
};

struct ir_sensor {
	struct gpio_dt_spec spec;
	uint8_t num;
};

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

void blink(const struct led *led, uint32_t sleep_ms, uint32_t id)
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
			sensor_status = k_fifo_get(&led0_fifo, K_MSEC(100));
		}
		else if (led->num == 1){
			sensor_status = k_fifo_get(&led1_fifo, K_MSEC(100));
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

void ir_sensor_read(const struct ir_sensor *ir_sensor)
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

		k_fifo_put(&led0_fifo, mem_ptr0);
		k_fifo_put(&led1_fifo, mem_ptr1);
	}
}

void blink0(void)
{
	blink(&led0, 100, 0);
}

void blink1(void)
{
	blink(&led1, 1000, 1);
}

void ir_sensor_read0(void)
{
	ir_sensor_read(&ir_sensor0);
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