/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/iio/events.h>
#include <sys/ioctl.h>
#include "iio_drv.h"

#define BASE 10

IIO_Driver::IIO_Driver(const std::string &name):
	event_fd(-1)
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

	devId = static_cast<std::string>(iio_device_get_id(dev));
}

IIO_Driver::~IIO_Driver()
{
	if (ctx != NULL)
	{
		if (rxBuf)
			IIO_Driver::closeBufferObj();

		iio_context_destroy(ctx);
	}
	if (event_fd != -1) {
		close(event_fd);
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
	const char *cid = NULL;

	cid = iio_channel_get_id(channels[index]);
	std::string eventPath = "/sys/bus/iio/devices/" + devId + "/events/in_" + static_cast<std::string>(cid) + "_" + attrName;
	std::fstream eventStream(eventPath.c_str());
	if (!eventStream.is_open())
		throw std::runtime_error("Unable to find attribute " + attrName);
	eventStream << value;
	eventStream.close();
	return 0;
}

double IIO_Driver::readeventattr(const unsigned int index, const std::string &attrName)
{
	const char *cid = NULL;
	double data;

	cid = iio_channel_get_id(channels[index]);
	std::string eventPath = "/sys/bus/iio/devices/" + devId + "/events/in_" + static_cast<std::string>(cid) + "_" + attrName;
	std::fstream eventStream(eventPath.c_str());
	if (!eventStream.is_open())
		throw std::runtime_error("Unable to find attribute " + attrName);
	eventStream >> data;
	eventStream.close();
	return data;
}

int IIO_Driver::getEventFd()
{
	if(event_fd == -1) {
		std::string filepath = "/dev/" + devId;
		int fd = open(filepath.c_str(), O_RDONLY);
		if (fd < 0) {
			perror("Failed to open iio device");
			return -1;
		}

		if(ioctl(fd, IIO_GET_EVENT_FD_IOCTL, &event_fd) == -1 ||
			event_fd == -1) {
			perror("Failed to get iio event fd");
			return -1;
		}

		if (close(fd) == -1)
			perror("Failed to close character device file");
	}
	return event_fd;
}

ssize_t IIO_Driver::populateMap(const struct iio_channel *chn, void *src, size_t bytes, void *data)
{
	int cIndex = iio_channel_get_index(chn);
	std::map<int, std::vector<double>> *mapPtr = static_cast<std::map<int, std::vector<double>> *>(data);

	if (bytes == sizeof(int64_t))
	{
		double tmp = ((int64_t *)src)[0];
		(*mapPtr)[cIndex].push_back(tmp);
	}
	else if (bytes == sizeof(int32_t))
	{
		double tmp = ((int32_t *)src)[0] / 65536.0;
		(*mapPtr)[cIndex].push_back(tmp);
	}

	return bytes;
}

void IIO_Driver::closeBufferObj()
{
	// delete buffer object
	if (rxBuf) {
		iio_buffer_cancel(rxBuf);
		iio_buffer_destroy(rxBuf);
		rxBuf = NULL;
	}
	// disable active channels
	for (auto it : this->activeChannels)
	{
		if (iio_channel_is_enabled(channels[it]))
			iio_channel_disable(channels[it]);
	}
}

bool IIO_Driver::compareActiveChannels(std::vector<int> &requestedChannels)
{
	bool isChanged = false;

	for (auto it : requestedChannels)
	{
		if (!iio_channel_is_enabled(channels[it]))
			iio_channel_enable(channels[it]);
		if (std::find(this->activeChannels.begin(), this->activeChannels.end(), it) == this->activeChannels.end())
		{
			this->activeChannels.push_back(it);
			isChanged = true;
		}
	}

	for (auto it : this->activeChannels)
	{
		if (std::find(requestedChannels.begin(), requestedChannels.end(), it) == requestedChannels.end())
		{
			if (iio_channel_is_enabled(channels[it]))
				iio_channel_disable(channels[it]);
			this->activeChannels.erase(std::find(activeChannels.begin(), activeChannels.end(), it));
			isChanged = true;
		}
	}

	return isChanged;
}

// api to create buffer
int IIO_Driver::createBuffer(int numSamples, std::vector<int> &channelList)
{
	int ret = 0;

	IIO_Driver::compareActiveChannels(channelList);
	if (rxBuf)
	{
		iio_buffer_cancel(rxBuf);
		iio_buffer_destroy(rxBuf);
		rxBuf = NULL;
	}
	// create buffer object
	rxBuf = iio_device_create_buffer(dev, numSamples, false);
	if (rxBuf == NULL)
	{
		std::cout << "failed to create buffer" << std::endl;
		ret = -1;
	}
	return ret;
}

std::map<int, std::vector<double>> IIO_Driver::getBufferdata(int numSamples, std::vector<int> channelList)
{
	std::map<int, std::vector<double>> channelDataMap;
	char errmessage[1024];
	int ret;

	if (channelList.size() == 0) {
		closeBufferObj();
		this->activeChannels.clear();
		return channelDataMap;
	}

	// creates buffer only on a new or modified request
	ret = IIO_Driver::createBuffer(numSamples, channelList);
	if (ret < 0) {
		return channelDataMap;
	}

	ssize_t nbytes_rx = iio_buffer_refill(rxBuf);
	if (nbytes_rx < 0)
	{
		iio_strerror(-(int)nbytes_rx, errmessage, sizeof(errmessage));
		std::cout << "Error refilling buf: " << errmessage << std::endl;
		closeBufferObj();
		return channelDataMap;
	}

	nbytes_rx /= iio_buffer_step(rxBuf);
	if (nbytes_rx >= numSamples)
		iio_buffer_foreach_sample(rxBuf, populateMap, &channelDataMap);
	else
		std::cout << "not enough buffer to read sample data" << std::endl;

	closeBufferObj();
	return channelDataMap;
}
