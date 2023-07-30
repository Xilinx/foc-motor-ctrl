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
	EventManager();
	int registerEvent(FaultType event, EventControl *driver,
			  std::function<void(FaultType)> cb);
	int deRegisterEvent(FaultType event);
	~EventManager();
private:
	void monitorThread(void);
	int addDescriptor(int fd);
	int removeDescriptor(int fd);

	bool mMonitorRunning;
	int epoll_fd;
	std::vector<int> desc_list;
	std::map<int,std::vector <FaultType>> mEvent_registered;
	std::map<int,EventControl *> mDrivers;
	std::map<FaultType,std::function<void(FaultType)>> mCallBacks;

	std::thread mThread;
	std::mutex mLock; //TODO: check if this is required.
};

#endif // _EVENT_MANAGER_H_
