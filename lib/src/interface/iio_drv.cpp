/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#include "iio_drv.h"
#include <iostream>
#define BASE 10

IIO_Driver::IIO_Driver(const std::string &name)
{

	ctx = iio_create_local_context();
	if (!ctx)
	{
		throw std::runtime_error("Unable to create Context");
	}

	dev = iio_context_find_device(ctx, name.c_str());
	if (!dev)
	{
		throw std::runtime_error("Unable to find " + name + " device on platform");
	}

	channelCount = iio_device_get_channels_count(dev);

	for (unsigned int i = 0; i < channelCount; i++)
	{
		channels.push_back(iio_device_get_channel(dev, i));
	}
}

IIO_Driver::~IIO_Driver()
{
	if (ctx != NULL)
	{
		iio_context_destroy(ctx);
	}
}

int IIO_Driver::readChannel(const unsigned int index, const std::string &attrName)
{
	char buf[32];
	char *endptr;
	int value;

	iio_channel_attr_read(channels[index], attrName.c_str(), buf, sizeof(buf));
	value = strtol(buf, &endptr, BASE);
	if (endptr == buf)
	{
		std::cerr << "No data obtained from attribute " << attrName << std::endl;
		return -1;
	}
	return value;
}

int IIO_Driver::writeChannel(const unsigned int index, const std::string &attrName, const std::string &value)
{
	int ret = iio_channel_attr_write(channels[index], attrName.c_str(), value.c_str());
	if (ret < 0)
	{
		std::cerr << "Unable to write to attribute " << attrName << std::endl;
	}
	return ret;
}

int IIO_Driver::readDeviceattr(const std::string &attrName)
{
	char buf[32];
	char *endptr;
	int value;
	iio_device_attr_read(dev, attrName.c_str(), buf, sizeof(buf));
	value = strtol(buf, &endptr, BASE);
	if (endptr == buf)
	{
		std::cerr << "No data obtained from attribute " << attrName << std::endl;
		return -1;
	}
	return value;
}

int IIO_Driver::writeDeviceattr(const std::string &attrName, const std::string &value)
{
	int ret = iio_device_attr_write(dev, attrName.c_str(), value.c_str());
	if (ret < 0)
	{
		std::cerr << "Unable to write to attribute " << attrName << std::endl;
	}
	return ret;
}
