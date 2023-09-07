/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _SOFT_FAULT_H
#define _SOFT_FAULT_H

#include "event_control.h"
#include "avgpower.h"
#include <iostream>

class SoftwareFaults : public EventControl
{
public:
	SoftwareFaults(Adchub *adchub, MC_Uio *uio);
	~SoftwareFaults();

	bool getEventStatus(FaultId event) override;
	int getEventFd(FaultId event) override;
	void enableEvent(FaultId event) override;
	void disableEvent(FaultId event) override;
	void clearEvent(FaultId event) override;
	void setUpperThreshold(FaultId event, double val) override;
	void setLowerThreshold(FaultId event, double val) override;
	double getUpperThreshold(FaultId event) override;
	double getLowerThreshold(FaultId event) override;

private:
	AVGPower mAvgP;
	std::thread mAvgPThread;
};

#endif
