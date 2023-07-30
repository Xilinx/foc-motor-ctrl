/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <unistd.h>
#include "event_manager.h"

EventManager::EventManager() : mMonitorRunning(true)
{
	epoll_fd = epoll_create1(0);
	if (epoll_fd < 0) {
		perror("Failed to create epoll file descriptor");
		//TODO: Handle the error
		return;
	}

	// Start the monitoring thread
	mThread = std::thread(&EventManager::monitorThread, this);
}

int EventManager::registerEvent(FaultType event, EventControl* driver,
                                std::function<void(FaultType)> cb)
{
	int fd = driver->getEventFd(event);
	if (fd < 0) {
		// Failed to get the event file descriptor
		// TODO:
		return -1;
	}

	mLock.lock(); // Acquire the lock before modifying the data structures

	// Add the file descriptor to the epoll set
	if (addDescriptor(fd) != 0) {
		mLock.unlock(); // Release the lock before returning on error
		return -1;
	}

	// Register the event and callback in the maps
	mEvent_registered[fd].push_back(event);
	mDrivers[fd] = driver;
	mCallBacks[event] = cb;

	driver->enableEvent(event);

	mLock.unlock(); // Release the lock after modifying the data structures
	return 0;
}

int EventManager::deRegisterEvent(FaultType event)
{
	mLock.lock(); // Acquire the lock before modifying the data structures

	// Find the event in the callback map
	auto it = mCallBacks.find(event);
	if (it == mCallBacks.end()) {
		// Event not found in the callback map
		mLock.unlock(); // Release the lock before returning
		return -1;
	}

	// Remove the event from the callback map
	mCallBacks.erase(it);

	// Find and remove the event from the registered events
	for (auto& entry : mEvent_registered) {
		auto& events = entry.second;
		auto events_it = std::find(events.begin(), events.end(), event);
		if (events_it != events.end()) {
			events.erase(events_it);
			break; // We assume each event can only be registered once for each driver
		}
	}

	// TODO: if FD has not event, removeDescriptor

	mLock.unlock(); // Release the lock after modifying the data structures
	return 0;
}

EventManager::~EventManager()
{
	// Stop the monitoring thread
	mMonitorRunning = false;

	// Cleanup: Remove all descriptors from the epoll set and close them
	for (int fd : desc_list) {
		removeDescriptor(fd);
		close(fd);
	}

	//TODO: Check for joinable.
	mThread.join();

	// Close the epoll file descriptor
	close(epoll_fd);
}

void EventManager::monitorThread()
{
	const int MAX_EVENTS = 10; // TODO: move to more sutiable
				   // location and adjust the number

	while (mMonitorRunning) {
		std::array<struct epoll_event, MAX_EVENTS> events;
		int num_events = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);
		if (num_events < 0) {
			// epoll_wait error
			// TODO: Handle the epoll_wait error
			continue; // Continue to the next iteration of the loop
		}

		for (int i = 0; i < num_events; ++i) {
			int fd = events[i].data.fd;
			mLock.lock(); // Acquire the lock before accessing data structures

			// Find the events associated with this file descriptor
			auto it = mEvent_registered.find(fd);
			if (it != mEvent_registered.end()) {
				const std::vector<FaultType>& events = it->second;

				// Call the registered callbacks for each event associated with this FD
				for (FaultType event : events) {
					auto callback_it = mCallBacks.find(event);
					if (callback_it != mCallBacks.end()) {
						// Call the corresponding callback function
						callback_it->second(event);
					}
				}
			}

			mLock.unlock(); // Release the lock after accessing data structures
		}
	}
}

int EventManager::addDescriptor(int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
		perror("Failed to add file descriptor to epoll");
		return -1;
	}

	desc_list.push_back(fd);
	return 0;
}

int EventManager::removeDescriptor(int fd)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr) < 0) {
		perror("Failed to remove file descriptor from epoll");
		return -1;
	}

	auto it = std::find(desc_list.begin(), desc_list.end(), fd);
	if (it != desc_list.end()) {
		desc_list.erase(it);
	}

	return 0;
}

