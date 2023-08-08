/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_

/*
 * Event Manager class to handle all the faults in the system.
 */

#include <unistd.h>
#include <sys/epoll.h>
#include <functional>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include "event_control.h"

class EventManager {
public:
	using EventCallback = std::function<void(FaultId)>;

	EventManager(std::initializer_list<EventControl *> drivers);

	/*
	 * Get event status
	 */
	bool getStatus(FaultId) const;

	/*
	 * Change event monitoring state
	 */
	int activateAllEvents(EventCallback cb = nullptr);
	int activateEvent(FaultId event, EventCallback cb = nullptr);
	int deactivateAllEvents(void);
	int deactivateEvent(FaultId event);
	int resetAllEvents(void);	// deactivate and clear events
	int resetEvent(FaultId event);

	/*
	 * Configure the event parameters
	 */
	void setUpperThreshold(FaultId event, double val);
	void setLowerThreshold(FaultId event, double val);
	double getUpperThreshold(FaultId event);
	double getLowerThreshold(FaultId event);

	~EventManager();

private:
	/*
	 * Utilities
	 */
	int addDescriptor(int fd);
	int removeDescriptor(int fd);

	/*
	 * Monitoring Thread
	 */
	void monitorThread(void);
	std::thread mThread;
	bool mMonitorRunning;
	int mEpoll_fd;
	int mExit_fd;

	/*
	 * Data structures for holding status of events
	 */

	/*
	 * Using Unordered_map vs std::map in favor of better lookup performance'
	 * Using Array is also an alternate. But will restrict to assume the FaultId is
	 * is used as index & sequential starting from 0.
	 */
	std::unordered_map<FaultId,bool> mEventStatus;

	/*
	 * Other Data strucutures for the state maintainance
	 * and Event operations and storage.
	 */
	std::mutex mLock;
	std::map<FaultId, EventControl *> mEventController;
	std::map<int,std::vector <FaultId>> mRegisteredEvents;
	std::vector<int> mDescList;
	std::map<FaultId,EventCallback> mCallBacks;
};

#endif // _EVENT_MANAGER_H_
