/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _IIO_DRV_H_
#define _IIO_DRV_H_

#include <string>
#include <iio.h>

class IIO_Driver {
public:
	IIO_Driver(const std::string& deviceName);
	~IIO_Driver();
	int readChannel(const std::string channelName);
	void writeChannel(const std::string channelName, int value);
private:
	struct iio_context *ctx;
	struct iio_device *dev;
}


#endif //_IIO_DRV_H_
