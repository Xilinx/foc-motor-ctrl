/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "qei.h"
#include <iostream>

const std::string QeiSensor::kQeiDriverName = "hls_qei_axi";

QeiSensor::QeiSensor(/* init config data*/)
{
	mQei_IIO_Handle = new IIO_Driver(QeiSensor::kQeiDriverName);
	// sample buffer data at 100us intervals
	mQei_IIO_Handle->writeDeviceattr("sample_interval_us", "100");
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

void QeiSensor::start()
{
	mQei_IIO_Handle->writeDeviceattr("ap_ctrl", "1");
}


std::map<Qeichannel, std::vector<double>> QeiSensor::fillBuffer(int samples, std::vector<Qeichannel> channels)
{
	std::map<Qeichannel, std::vector<double>> qeiData;
	std::vector<int> qeiVector;

	for (auto it : channels)
	{
		qeiVector.push_back(static_cast<int>(it));
	}

	std::map<int, std::vector<double>> rawDataMap = mQei_IIO_Handle->getBufferdata(samples, qeiVector);
	if (rawDataMap.size() == 0 && qeiVector.size() != 0)
	{
		std::cout << " No data samples obtained from QEI " << std::endl;
		return qeiData;
	}

	for (auto &it : rawDataMap)
	{
		for (auto &element : it.second)
			element *= 65536.0;
		Qeichannel newKey = static_cast<Qeichannel>(it.first);
		qeiData[newKey] = it.second;
	}

	return qeiData;
}
