/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "../interface/iio_drv.h"
#include "qei.h"

const string QeiSensor::kQeiDriveName = "hls-qei-axi";

QeiSensor::QeiSensor(/* init config data*/)
{
	mQei_IIO_Handle = new IIO_Driver(QeiSensor::kQeiDriverName);
}

QeiSensor::~QeiSensor()
{
	delete mQei_IIO_Handle;
}

int QeiSensor::getSpeed()
{
	// read and return the speed from iio handle channel
	return 0;
}

int QeiSensor::getPosition()
{
	// read and return the theta from iio handle channel
	return 0;
}

