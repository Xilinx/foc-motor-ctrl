/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _EVENT_CONTROL_H_
#define _EVENT_CONTROL_H_

#include "../include/motor-control/motor-control.hpp"

class EventControl {
public:
	virtual bool getEventStatus(FaultType event) = 0;
	virtual int getEventFd(FaultType event) = 0;
	virtual void enableEvent(FaultType event) = 0;
	virtual void disableEvent(FaultType event) = 0;
	/**
	 * TODO: Add threshold settings also here
	 */
};

#endif // _EVENT_CONTROL_H_
