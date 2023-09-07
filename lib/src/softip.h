/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _SOFT_IP_H_
#define _SOFT_IP_H_

class SoftIP
{
public:
	virtual void setThresholdValue(double value) = 0;
	virtual double getThresholdValue() = 0;
	virtual int getFd() = 0;
	virtual void enableFault(bool value) = 0;
	virtual void clearFault() = 0;
	virtual bool getFaultStatus() = 0;
	struct Register
	{
		bool faultEnable;
		bool faultStatus;
		double faultThreshold;
	};
};

#endif
