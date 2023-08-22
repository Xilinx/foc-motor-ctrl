/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _EVENT_CONTROL_H_
#define _EVENT_CONTROL_H_

#include <unordered_set>
#include <cassert>
#include "motor-control/motor-control.hpp"

class EventControl {
public:
	EventControl(std::initializer_list<FaultId> events = {}):
		mSupportedEvents(events)
	{
	}

	/*
	 * Mandatory APIs to be overridden
	 */
	virtual bool getEventStatus(FaultId event) = 0;
	virtual int getEventFd(FaultId event) = 0;
	virtual void enableEvent(FaultId event) = 0;
	virtual void disableEvent(FaultId event) = 0;
	virtual void clearEvent(FaultId event) = 0;

	/*
	 * MUST override these APIs if hardware Supports it.
	 */
	virtual void setUpperThreshold(FaultId event, double val)
	{
		//No Effect if not implemented
		static_cast<void>(val);
		assert(isSupportedEvent(event));
	}
	virtual void setLowerThreshold(FaultId event, double val)
	{
		//No Effect if not implemented
		static_cast<void>(val);
		assert(isSupportedEvent(event));
	}
	virtual double getUpperThreshold(FaultId event)
	{
		//No Effect if not implemented
		assert(isSupportedEvent(event));
		return 0.0;
	}
	virtual double getLowerThreshold(FaultId event)
	{
		//No Effect if not implemented
		assert(isSupportedEvent(event));
		return 0.0;
	}

	bool isSupportedEvent(FaultId e) const
	{
		return (mSupportedEvents.find(e) != mSupportedEvents.end());
	}
private:
	const std::unordered_set<FaultId> mSupportedEvents;
};

#endif // _EVENT_CONTROL_H_
