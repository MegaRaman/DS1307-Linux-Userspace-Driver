#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include "userspace_i2c_ops.h"


#define SECONDS_REG	0x0
#define MINUTES_REG	0x1
#define HOURS_REG	0x2
#define WDAYS_REG	0x3
#define MDAYS_REG	0x4
#define MONTHS_REG	0X5
#define YEARS_REG	0x6

/* assume that all time registers are consecutive */
#define ALL_TIME_REG_LEN (YEARS_REG - SECONDS_REG + 1)

// taken from Linux kernel bcd.c
unsigned bcd2bin(unsigned char val) {
	return (val & 0x0f) + (val >> 4) * 10;
}

unsigned char bin2bcd(unsigned val) {
	return ((val / 10) << 4) + val % 10;
}

/* Enable clock and set format to 24-hour */
int ds1307_init() {
	int ret;
	uint8_t tmp[1];
	ret = i2c_write(SECONDS_REG, tmp, 1);
	if (ret < 0)
		goto err;

	ret = i2c_read(HOURS_REG, tmp, 1);
	if (ret < 0)
		goto err;
	tmp[0] &= ~(1 << 6);
	ret = i2c_write(HOURS_REG, tmp, 1);
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("Error initialising DS1307\n");
	return ret;
}

bool time_parse(char *s, char *format, struct tm *tm) {
	char *res = strptime(s, format, tm);

	if (res == NULL || *res != '\0')
		return false;
	return true;
}

int ds1307_set_time(struct tm *tm) {
	uint8_t tmp[ALL_TIME_REG_LEN];

	tmp[SECONDS_REG] = bin2bcd(tm->tm_sec);
	tmp[MINUTES_REG] = bin2bcd(tm->tm_min);
	tmp[HOURS_REG] = bin2bcd(tm->tm_hour);
	tmp[MDAYS_REG] = bin2bcd(tm->tm_mday);
	tmp[MONTHS_REG] = bin2bcd(tm->tm_mon + 1);
	tmp[YEARS_REG] = bin2bcd(tm->tm_year - 100);
	tmp[WDAYS_REG] = bin2bcd(tm->tm_wday + 1);

	int ret = i2c_write(SECONDS_REG, tmp, ALL_TIME_REG_LEN);
	if (ret < 0) {
		printf("Error writing time to DS1307\n");
		return ret;
	}
	return ret;
}

int ds1307_get_time(struct tm *tm) {
	uint8_t tmp[ALL_TIME_REG_LEN];
	uint8_t val;

	int ret = i2c_read(SECONDS_REG, tmp, ALL_TIME_REG_LEN);
	if (ret < 0) {
		printf("Error reading time from DS1307\n");
		return ret;
	}

	tm->tm_sec = bcd2bin(tmp[SECONDS_REG] & 0x7f);
	tm->tm_min = bcd2bin(tmp[MINUTES_REG] & 0x7f);
	val = tmp[HOURS_REG] & 0x3f;
	tm->tm_hour = bcd2bin(val);
	tm->tm_wday = bcd2bin(tmp[WDAYS_REG] & 0x07) - 1;
	tm->tm_mday = bcd2bin(tmp[MDAYS_REG] & 0x3f);
	val = tmp[MONTHS_REG] & 0x1f;
	tm->tm_mon = bcd2bin(val) - 1;
	tm->tm_year = bcd2bin(tmp[YEARS_REG]) + 100;

	return ret;
}

int main(int argc, char **argv) {
	if (argc < 3 || argc > 4)
		goto usage;

	if (i2c_init(atoi(argv[2]), 0x68) != 0) {
		printf("Failed to acquire device\n");
		return -1;
	}

	int ret;
	struct tm tm;
	struct tm read_tm;
	ret = ds1307_init();
	if (ret < 0)
		return ret;

	if (argv[1][0] == 'w') {
		bool res = time_parse(argv[3], "%Y-%m-%d %H:%M:%S", &tm);
		if (!res) {
			printf("Wrong input time format\n");
			return -1;
		}
		ret = ds1307_set_time(&tm);
		if (ret < 0)
			return ret;
	}
	else if (argv[1][0] == 'r') {
		ret = ds1307_get_time(&read_tm);
		if (ret < 0)
			return ret;

		puts(asctime(&read_tm));
	}
	else {
		goto usage;
	}

	return 0;

usage:
	printf("Usage: %s w adapter_nr \"YYYY-MM-DD hh:mm:ss\" OR %s r adapter_nr\n",
			argv[0], argv[0]);
	return -1;
}

