#include "spi.h"

void spi_comm(struct device *spi_dev, struct k_fifo *spi_fifo){

    struct spi_fifo_t *spi_data;
 
    while (!device_is_ready(spi_dev)) {
        k_msleep(100);
        continue;
    }

	static struct spi_cs_control cs_ctrl = (struct spi_cs_control){
		.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(arduino_spi_dev)),
		.delay = 0
	};

	struct spi_config config = {
		.frequency = 200000,
		.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_MODE_GET(0),
		.slave = 0,
		.cs = cs_ctrl
	};

    while (1){
        spi_data = k_fifo_get(spi_fifo, K_MSEC(100));
        if (spi_data == NULL) {
            continue; // No data available, skip to next iteration
        }
        
        uint8_t header[1];
        header[0] = spi_data->len & 0xFF;

        uint8_t tx_buf[spi_data->len + 1];
        memcpy(&tx_buf, spi_data->message, spi_data->len);

        /* Set up the buffer descriptors */
        const struct spi_buf tx_bufs[] = {
            {
                .buf = header,
                .len = sizeof(header)
            },
            {
                .buf = tx_buf,
                .len = sizeof(tx_buf)
            }
        };

        const struct spi_buf_set tx = {
            .buffers = tx_bufs,
            ARRAY_SIZE(tx_bufs)
        };
        
        int ret = spi_write(spi_dev, &config, &tx);
        if (ret != 0) {
            printk("SPI write failed: %d\n", ret);
        }
        
        k_sleep(K_MSEC(2000));
        k_free(spi_data);
    }
};
	