/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _SVPWM_H_
#define _SVPWM_H_

#include "interface/iio_drv.h"
#include <string>

class Svpwm
{

public:
	Svpwm();
	int setSampleII(int sample);
	int setDcLink(int volt);
	int setMode(int mode);
	int startSvpwm();
	~Svpwm();

private:
	IIO_Driver *m_Svpwm_IIO_Handle;
	static const std::string kSvpwmDriverName;
};

#endif /* _SVPWM_H_ */
