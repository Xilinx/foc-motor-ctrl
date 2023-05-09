/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _PWM_H_
#define _PWM_H_

#include "interface/iio_drv.h"

class Pwm
{
public:
	Pwm(/* args */);
	int setFrequency(int frequency);
	int startPwm();
	int setDeadCycle(int deadCycle);
	int setPhaseShift(int phaseShift);
	int setSampleII(int sample);
	~Pwm();

private:
	IIO_Driver *m_Pwm_IIO_Handle;
	static const std::string kPwmDriverName;
};

#endif // _PWM_H_
