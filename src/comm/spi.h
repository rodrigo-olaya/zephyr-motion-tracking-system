#ifndef SPI_COMM_H
#define SPI_COMM_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/spi.h>

// #define SPI_NODE DT_NODELABEL(arduino_spi_dev)
#define SPI_NODE DT_ALIAS(arduinospi)

#if !DT_NODE_HAS_STATUS_OKAY(SPI_NODE)
#error "Unsupported board: spi2 devicetree alias is not defined"
#endif

struct spi_fifo_t {
    void *fifo_reserved; /* 1st word reserved for use by fifo */
	uint8_t message[128];
    uint8_t len;
};

void spi_comm();

#endif // SPI_COMM_H