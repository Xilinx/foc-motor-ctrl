/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _MCUIO_H_
#define _MCUIO_H_

#include "../include/motor-control/motor-control.hpp"
#include "event_control.h"
#include "interface/uio_drv.h"

class MC_Uio : public EventControl
{
private:
	static const std::string kUioDriverName;
	UioDrv* mUioHandle;
public:
	MC_Uio(/* args */);
	~MC_Uio();
	int setGateDrive(bool value);
	uint32_t getGateDrive();

	// Fault Handling
	bool getEventStatus(FaultId event) override;
	int getEventFd(FaultId event) override;
	void enableEvent(FaultId event) override;
	void disableEvent(FaultId event) override;
};

#endif
