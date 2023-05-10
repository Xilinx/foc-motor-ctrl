/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#include "iio_drv.h"
#include <iostream>
#include <fstream>
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

double IIO_Driver::readChannel(const unsigned int index, const std::string &attrName)
{
	char buf[32];
	char *endptr;
	double value;

	iio_channel_attr_read(channels[index], attrName.c_str(), buf, sizeof(buf));
	value = strtod(buf, &endptr);
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

double IIO_Driver::readDeviceattr(const std::string &attrName)
{
	char buf[32];
	char *endptr;
	double value;
	iio_device_attr_read(dev, attrName.c_str(), buf, sizeof(buf));
	value = strtod(buf, &endptr);
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

int IIO_Driver::writeeventattr(const unsigned int index, const std::string &attrName, const std::string &value)
{
	const char* id = NULL;
	const char* cid = NULL;

	id = iio_device_get_id(dev);
	cid = iio_channel_get_id(channels[index]);

	std::string eventPath = "/sys/bus/iio/devices/" + static_cast<std::string>(id) + "/events/in_" + static_cast<std::string>(cid) + "_" + attrName;
	std::fstream eventStream(eventPath.c_str());
	if (!eventStream.is_open())
			throw std::runtime_error("Unable to find attribute " + attrName);
	eventStream << value;
	eventStream.close();
	return 0;
}
