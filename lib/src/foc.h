/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _FOC_HPP_
#define _FOC_HPP_

#include "motor-control/motor-control.hpp"
#include "interface/iio_drv.h"

/*
 * TODO: May be useful to have dedicated OpMode for
 * FOC
 */

class Foc
{
public:
	Foc();
	int setSpeed(double speedSp);
	int setTorque(double torqueSp);
	int setGain(GainType gainController, double kp, double ki);
	GainData getGain(GainType gainController);
	int startFoc();
	int setAngleOffset(int angleSh);
	int setFixedSpeed(int fixedSpeed);
	int setVfParam(double vq, double vd, int fixedSpeed);
	int stopMotor();
	double getTorqueSetValue();
	int setOperationMode(MotorOpMode mode);
	int getSpeedSetValue();
	FocData getChanData();
	~Foc();

private:
	IIO_Driver *mFoc_IIO_Handle;
	static const std::string kFocDriverName;
	int mTargetSpeed;
	int mTargetTorque;
};

#endif // _FOC_HPP_
