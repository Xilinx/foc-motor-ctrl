/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "smo.h"

const std::string SmoSensor::kSmoDriverName = "hls-smo-axi";

SmoSensor::SmoSensor(/* init config data*/)
{
	mSmo_IIO_Handle = new IIO_Driver(SmoSensor::kSmoDriverName);
}

SmoSensor::~SmoSensor()
{
	delete mSmo_IIO_Handle;
}

int SmoSensor::getSpeed()
{
	// read and return the speed from iio handle channel
	return 0;
}

int SmoSensor::getPosition()
{
	// read and return the theta from iio handle channel
	return 0;
}

void SmoSensor::start()
{

}
