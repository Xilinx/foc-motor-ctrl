/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _FOC_HPP_
#define _FOC_HPP_

#include "motor-control/motor-control.hpp"
#include "interface/iio_drv.h"

struct GainData
{
	double kp;
	double ki;
};

class Foc
{
public:
	Foc(/* args */);
	int setSpeed(int speedSp);
	int setTorque(double torqueSp);
	int setGain(GainType gainController, double kp, double ki);
	GainData getGain(GainType gainController);
	int startFoc();
	int setAngleOffset(int angleSh);
	int setFixedSpeed(int fixedSpeed);
	int setVfParam(double vq, double vd, int fixedSpeed);
	int stopMotor();
	double getTorque();
	FocData getChanData();
	~Foc();

private:
	IIO_Driver *mFoc_IIO_Handle;
	static const std::string kFocDriverName;
};

#endif // _FOC_HPP_
