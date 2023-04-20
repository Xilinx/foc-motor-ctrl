/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _SENSOR_SMO_HPP_
#define _SENSOR_SMO_HPP_

class SmoSensor : public Sensor {
public:
	SmoSensor(/* constructor parameters */);
	virtual int getSpeed() override;
	virtual int getPosition() override;
	~SmoSensor();

private:
	// private member variables and functions for SMO sensor
	IIO_Driver *mSmo_IIO_Handle;
	static const string kSmoDriveName;
};

#endif // _SENSOR_SMO_HPP_
