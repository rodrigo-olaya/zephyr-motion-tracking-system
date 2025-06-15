/* Start of temp IMU code before thread refactoring */
#include "mpu6050.h"

int IMU_read(const struct device *const mpu6050, struct k_fifo *spi_fifo){

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
		k_fifo_put(spi_fifo, spi_mem_ptr);

        k_msleep(500);
	}

	return 0;
}

/* End of temp code before thread refactoring */