/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _SENSOR_HPP_
#define _SENSOR_HPP_
#include <map>
#include <vector>

// TODO: move Qeichannel Enum within the class namespace
enum Qeichannel
{
	RPM = 0,
	THETA,
};

class Sensor {
public:
	virtual ~Sensor();
	virtual int getSpeed() = 0;
	virtual int getPosition() = 0;
	virtual void start() = 0;
	virtual	std::map<Qeichannel, std::vector<double>> fillBuffer( int samples, std::vector<Qeichannel> channels);
	static Sensor *getSensorInstance();
private:
	static bool isQeiPresent();
};

#endif /*_SENSOR_HPP_*/
