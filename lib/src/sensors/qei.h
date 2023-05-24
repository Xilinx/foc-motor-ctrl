/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _SENSOR_QEI_HPP_
#define _SENSOR_QEI_HPP_

#include <string>
#include "sensor.h"
#include "../interface/iio_drv.h"

class QeiSensor : public Sensor
{
public:
	QeiSensor(/* constructor parameters */);
	int getSpeed() override;
	int getPosition() override;
	void start() override;
	~QeiSensor();

private:
	// private member variables and functions for QEI sensor
	IIO_Driver *mQei_IIO_Handle;
	static const std::string kQeiDriverName;
};

#endif // _SENSOR_QEI_HPP_
