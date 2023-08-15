/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _IIO_DRV_H_
#define _IIO_DRV_H_

#include <string>
#include <iio.h>
#include <vector>

class IIO_Driver
{
public:
	IIO_Driver(const std::string &deviceName);
	~IIO_Driver();
	double readChannel(const unsigned int index, const std::string &attrName);
	int writeChannel(const unsigned int index, const std::string &attrName, const std::string &value);
	double readDeviceattr(const std::string &attrName);
	int writeDeviceattr(const std::string &attrName, const std::string &value);
	int writeeventattr(const unsigned int index, const std::string &attrName, const std::string &value);
	double readeventattr(const unsigned int index, const std::string &attrName);
	int getEventFd(void);

private:
	std::string devId;
	struct iio_context *ctx;
	struct iio_device *dev;
	unsigned int channelCount;
	std::vector<iio_channel *> channels;
	int event_fd;
};

#endif //_IIO_DRV_H_
