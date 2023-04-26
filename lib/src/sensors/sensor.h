/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _SENSOR_HPP_
#define _SENSOR_HPP_

class Sensor {
public:
	virtual ~Sensor();
	virtual int getSpeed() = 0;
	virtual int getPosition() = 0;
	static Sensor *getSensorInstance();
private:
	static bool isQeiPresent();
};

#endif /*_SENSOR_HPP_*/
