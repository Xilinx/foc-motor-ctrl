/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "softip.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include "adchub.h"
#include "mc_driver.h"
#include <thread>
#include <iostream>

#ifndef _AVG_POWER_H
#define _AVG_POWER_H

class AVGPower : public SoftIP
{
public:
	AVGPower(Adchub* adchub, MC_Uio* mcuio);
	~AVGPower();
	void setThresholdValue(double value) override;
	double getThresholdValue() override;
	int getFd() override;
	void enableFault(bool value) override;
	void clearFault() override;
	bool getFaultStatus() override;
	void generateFault();

private:
	double calculateAvgPower();

	int mAvgFd;
	Adchub *madc;
	MC_Uio *muio;
	Register AVGPowerRegister;
};

#endif
