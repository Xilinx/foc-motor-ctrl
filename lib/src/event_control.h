/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _EVENT_CONTROL_H_
#define _EVENT_CONTROL_H_

#include <unordered_set>
#include "motor-control/motor-control.hpp"

class EventControl {
public:
	EventControl(std::initializer_list<FaultId> events = {}):
		mSupportedEvents(events)
	{
	}

	virtual bool getEventStatus(FaultId event) = 0;
	virtual int getEventFd(FaultId event) = 0;
	virtual void enableEvent(FaultId event) = 0;
	virtual void disableEvent(FaultId event) = 0;

	virtual void clearEvent(FaultId event) = 0;
	virtual void setUpperThreshold(FaultId event, double val) = 0;
	virtual void setLowerThreshold(FaultId event, double val) = 0;
	virtual double getUpperThreshold(FaultId event) = 0;
	virtual double getLowerThreshold(FaultId event) = 0;
	/**
	 * TODO: Add threshold settings also here
	 */

	bool isSupportedEvent(FaultId e) const
	{
		return (mSupportedEvents.find(e) != mSupportedEvents.end());
	}
private:
	const std::unordered_set<FaultId> mSupportedEvents;
};

#endif // _EVENT_CONTROL_H_
