/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "qei.h"

const std::string QeiSensor::kQeiDriverName = "hls_qei_axi";
enum Qeichannel
{
	RPM = 0,
	THETA,
};

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
	return mQei_IIO_Handle->readChannel(RPM, "raw");
}

int QeiSensor::getPosition()
{
	return mQei_IIO_Handle->readChannel(THETA, "raw");
}

int QeiSensor::startQei()
{
	return mQei_IIO_Handle->writeDeviceattr("ap_ctrl", "1");
}