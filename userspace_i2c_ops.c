#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

static int file = 0;
static uint8_t dev_addr = 0;

#define BUF_LEN 32

int i2c_init(int adapter_nr, int addr) {
	char filename[20];
	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
		perror("Error opening I2C device file: ");
		return -1;
	}

	if (ioctl(file, I2C_SLAVE_FORCE, addr) < 0) {
		perror("Error calling ioctl on slave: ");
		return -1;
	}
	dev_addr = addr;
	return 0;
}

int i2c_read(uint8_t reg, uint8_t *buf, uint8_t count) {
	if (file == 0 || count == 0)
		return -1;

	struct i2c_msg msgs[2];
	msgs[0].addr = dev_addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	msgs[1].addr = dev_addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = count;
	msgs[1].buf = buf;

	struct i2c_rdwr_ioctl_data msgset;
	msgset.msgs = msgs;
	msgset.nmsgs = 2;
	if (ioctl(file, I2C_RDWR, &msgset) < 0) {
		perror("Error reading message in rdwr mode: ");
		return -1;
	}

	return 0;
}

int i2c_write(uint8_t reg, uint8_t *buf, uint8_t count) {
	if (file == 0 || count == 0)
		return -1;
	uint8_t reg_buf[BUF_LEN];
	reg_buf[0] = reg;
	memcpy(reg_buf + 1, buf, count);

	if (write(file, reg_buf, count + 1) != count + 1) {
		perror("Error writing message in rdwr mode: ");
		return -1;
	}
	return 0;
}

