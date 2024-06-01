# DS1307 Linux Userspace Driver

## Introduction

This project provides a Linux userspace driver for the DS1307 real-time clock (RTC) module. The driver allows users to read from and write to the time registers of the DS1307, enabling interaction with the device for timekeeping purposes.

## Features

- Read current time from DS1307
- Write new time to DS1307
- Simple interface using I2C communication
- Error handling for I2C operations

## Prerequisites

- `i2c-dev` module installed or compiled into kernel(Default in `Ubuntu 22.04`)

## Build

- Just use `make` to build `us_ds1307_driver` executable

## Usage

- Be aware that module uses `arm-none-linux-gnueabihf-gcc` by default, you can use `gcc` for example as following
```sh
make CC=gcc
```

### Reading time from the clock
```sh
./us_ds1307_driver r i2c_adapter_number
```

### Writing time to the clock
```sh
./us_ds1307_driver w i2c_adapter_number "YYYY-MM-DD hh:mm:ss"
```
