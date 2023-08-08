/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <unistd.h>
#include <cassert>
#include <sys/eventfd.h>
#include "event_manager.h"

#define MAX_EVENTS	10

EventManager::EventManager(std::initializer_list<EventControl *> drivers) :
		mMonitorRunning(true), mExit_fd(eventfd(0,0))
{
	/*
	 * Initialize driver map for each event
	 */
	for (int i = 0; i < static_cast<int>(FaultId::kFaultIdMax); i++) {
		FaultId event = static_cast<FaultId>(i);
		for (auto drv: drivers) {
			if (drv->isSupportedEvent(event)) {
				mEventController[event] = drv;
				mEventStatus[event] = false; //just for sanity
				break;
			}
		}
	}

	mEpoll_fd = epoll_create1(0);
	if (mEpoll_fd < 0) {
		perror("Failed to create epoll file descriptor");
		assert(false);
		return;
	}

	// Add descriptor for the Exit Event
	if (addDescriptor(mExit_fd) != 0) {
		assert(false);
	}

	// Start the monitoring thread
	mThread = std::thread(&EventManager::monitorThread, this);
}

bool EventManager::getStatus(FaultId event) const
{
	bool status = false;
	// No need to lock just for status reading
	if (auto fault = mEventStatus.find(event); fault != mEventStatus.end())
		status = fault->second;
	return status;
}

int EventManager::activateAllEvents(EventCallback cb /* = nullptr */)
{
	int status = 0;
	for (auto const& e : mEventController) {
		if (e.second)
			status += activateEvent(e.first, cb);
	}
	return status;
}

int EventManager::activateEvent(FaultId event, EventCallback cb /* = nullptr */)
{
	auto driver = mEventController[event];

	if (!driver) {
		assert(false);
		return -1;
	}

	int fd = driver->getEventFd(event);

	if (fd < 0) {
		assert(false);
		return -1;
	}

	mLock.lock(); // Acquire the lock before modifying the data structures

	// Add the file descriptor to the epoll set
	if (addDescriptor(fd) != 0) {
		mLock.unlock(); // Release the lock before returning on error
		return -1;
	}

	/*
	 * Register the event and callback in the maps.
	 * Make sure to have unique entry for the RegisteredEvent
	 * Below find logic is equivalent to
	 * mRegisteredEvents[fd].push_back(event); but each entry is unique
	 */
	auto &eventList = mRegisteredEvents[fd];
	if (std::find(eventList.begin(), eventList.end(), event) ==
							eventList.end()) {
		eventList.push_back(event);
	}
	mCallBacks[event] = cb;

	driver->enableEvent(event);

	mLock.unlock(); // Release the lock after modifying the data structures
	return 0;
}

int EventManager::resetAllEvents(void)
{
	int status = 0;
	for (auto const& e : mEventController) {
		if(e.second) {
			status += resetEvent(e.first);
		}
	}
	return status;
}
int EventManager::resetEvent(FaultId e)
{
	int status = 0;
	mStatusLock.lock();
	auto driver = mEventController[e];
	if (driver)
	{
		status += deactivateEvent(e);
		driver->clearEvent(e);
	}
	mEventStatus[e]=false;
	mStatusLock.unlock();
	return status;
}

int EventManager::deactivateAllEvents(void)
{
	int status = 0;
	for (auto const& e : mEventController) {
		if(e.second) {
			status += deactivateEvent(e.first);
		}
	}
	return status;
}

int EventManager::deactivateEvent(FaultId event)
{
	auto driver = mEventController[event];
	assert(driver);

	mLock.lock();
	auto it = mCallBacks.find(event);
	if (it != mCallBacks.end()) {
		// Remove the event from the callback map
		mCallBacks.erase(it);
	}

	// Find and remove the event from the registered events
	for (auto& entry : mRegisteredEvents) {
		auto& events = entry.second;
		auto events_it = std::find(events.begin(), events.end(), event);
		if (events_it != events.end()) {
			events.erase(events_it);
			break; // Assuming unique entry for the event
		}

		if(events.empty()) {
			// if FD has no event, removeDescriptor
			removeDescriptor(entry.first);
		}
	}

	if (driver) {
		driver->disableEvent(event);
	}

	mLock.unlock();
	return 0;
}

EventManager::~EventManager()
{
	const uint64_t exit_signal = 1;

	// Stop the monitoring thread
	mMonitorRunning = false;

	// Send exit signal to break the epoll wait
	eventfd_write(mExit_fd, exit_signal);

	// Wait for the thread to complete and join
	if (mThread.joinable()) {
		mThread.join();
	}

	// Cleanup: Remove all descriptors from the epoll set
	for (int fd : mDescList) {
		removeDescriptor(fd);
	}

	// Close the file descriptors
	close(mExit_fd);
	close(mEpoll_fd);
}

void EventManager::monitorThread()
{
	while (mMonitorRunning) {
		std::array<struct epoll_event, MAX_EVENTS> events;
		int num_events = epoll_wait(mEpoll_fd, events.data(),
								MAX_EVENTS, -1);
		if (num_events < 0) {
			// epoll_wait error. could be exception
			assert(false);
			continue; // Continue to the next iteration of the loop
		}

		for (int i = 0; i < num_events; ++i) {
			int fd = events[i].data.fd;
			uint64_t dump_it;

			if (fd == mExit_fd) {
				eventfd_read(fd, &dump_it);
				/*
				 * To exit assume the event was triggered after setting
				   mMonitorRunning = false;
				 */
				assert (mMonitorRunning == false);
				break; // Skipping rest of the events, if they also happened.
			}

			// Read the fd to ack the interrupt
			read(fd, &dump_it, sizeof(dump_it));

			//Get list of callbacks for active and hot events
			auto callback_list = popOccurredEvents(fd);

			for (auto cb : callback_list) {
				cb.first(cb.second);
			}
		}
	}
}

int EventManager::addDescriptor(int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;
	int status = 0;

	/*
	 * Add the descriptor to the epoll interest list.
	 * Even if it was already added, attempt to add it again as oppose to
	 * check the vector (mDescList) for the entry.
	 * The error condition of already exist is ignored.
	 * This approach allows to verify that the FD exists in the
	 * interest list (as a sanity) and during the error check,
	 * it can be verified if it is in the desc_list as well.
	 */

	if (epoll_ctl(mEpoll_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
		if (errno == EEXIST) {
			// The descriptor is already monitored.
			assert(std::find(mDescList.begin(), mDescList.end(), fd) !=
									mDescList.end());
		} else {
			perror("Failed to add file descriptor to epoll");
			status = -1;
		}
	} else {
		mDescList.push_back(fd);
	}

	return status;
}

int EventManager::removeDescriptor(int fd)
{
	if (epoll_ctl(mEpoll_fd, EPOLL_CTL_DEL, fd, nullptr) < 0) {
		perror("Failed to remove file descriptor from epoll");
		return -1;
	}

	auto it = std::find(mDescList.begin(), mDescList.end(), fd);
	if (it != mDescList.end()) {
		mDescList.erase(it);
	}

	return 0;
}

/*
 * ONLY CALLED from the monitorThread context.
 * Record and remove the events occurred for the fd from the data structures.
 * Returns list of callback for the occurred events
 */
std::vector<std::pair<EventManager::EventCallback, FaultId>>
EventManager::popOccurredEvents(int fd)
{
	std::vector<std::pair<EventCallback, FaultId>> cb_ready_list;

	/*
	 * Make sure the events are not being activated, reset or
	 * deactivated during the event processing.
	 * Take both mLock and mStatusLock to ensure it.
	 */
	mLock.lock();
	mStatusLock.lock();
	// Find the events associated with this file descriptor
	auto it = mRegisteredEvents.find(fd);
	if (it != mRegisteredEvents.end()) {
		auto &events = it->second;
		/*
		 * parse the event vector and if the event
		 * has occurred:
		 * - Disable the event
		 * - Update the status
		 * - record its callback
		 * - Remove the event from the Registered database
		 */
		for (auto event_it = events.begin(); event_it != events.end();) {
			auto event = *event_it;
			auto driver = mEventController[event];
			if (driver && driver->getEventStatus(event)) {

				// disable the enable
				driver->disableEvent(event);

				// Update the status
				mEventStatus[event] = true;

				// Record & remove the callback if any
				auto cb_it = mCallBacks.find(event);
				if (cb_it != mCallBacks.end()) {
					if(cb_it->second) {
						//record the callback
						cb_ready_list.push_back(
							{cb_it->second, event}
							);
					}
					// erase callback entry
					mCallBacks.erase(cb_it);
				}
				events.erase(event_it);
				if(events.empty()) {
					removeDescriptor(fd);
				}
			} else {
				event_it++;
			}
		}
	}
	mStatusLock.unlock();
	mLock.unlock();

	return cb_ready_list;
}

void EventManager::setUpperThreshold(FaultId event, double val)
{
	if(mEventController[event]) {
		mEventController[event]->setUpperThreshold(event, val);
	}
}

void EventManager::setLowerThreshold(FaultId event, double val)
{
	if(mEventController[event]) {
		mEventController[event]->setLowerThreshold(event, val);
	}
}

double EventManager::getUpperThreshold(FaultId event)
{
	double threshold = 0.0;
	if(mEventController[event]) {
		threshold = mEventController[event]->getUpperThreshold(event);
	}
	return threshold;
}

double EventManager::getLowerThreshold(FaultId event)
{
	double threshold = 0.0;
	if(mEventController[event]) {
		threshold = mEventController[event]->getLowerThreshold(event);
	}
	return threshold;
}
