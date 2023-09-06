/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _IIO_DRV_H_
#define _IIO_DRV_H_

#include <algorithm>
#include <string>
#include <vector>
#include <mutex>
#include <iio.h>
#include <map>

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
	std::map<int, std::vector<double>> getBufferdata(int numSamples, std::vector<int> channelList);

private:
	std::string devId;
	struct iio_context *ctx;
	struct iio_device *dev;
	unsigned int channelCount;

	std::vector<iio_channel *> channels;
	std::vector<int> activeChannels;
	int event_fd;
	struct iio_buffer *rxBuf = NULL;

	static ssize_t populateMap(const struct iio_channel *chn, void *src, size_t bytes, void *d);
	int createBuffer(int numSamples, std::vector<int> &channelList);
	void closeBufferObj();
	bool compareActiveChannels(std::vector<int> &requestedChannels);
};

#endif //_IIO_DRV_H_
