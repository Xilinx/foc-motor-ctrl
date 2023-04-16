/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _SENSOR_QEI_HPP_
#define _SENSOR_QEI_HPP_

class QeiSensor : public Sensor {
public:
	QeiSensor(/* constructor parameters */);
	virtual int getSpeed() override;
	virtual int getPosition() override;
	~QeiSensor();

private:
	// private member variables and functions for QEI sensor
	IIO_Driver *mQei_IIO_Handle;
	static const string kQeiDriveName;
};

#endif // _SENSOR_QEI_HPP_
