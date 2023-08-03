/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <cassert>
#include "mc_driver.h"

/*
 * Hardware Offsets
 */
#define GATE_DRIVE_EN 0x00
#define PHASE_CURRENT_BALANCE_FAULT_VALUE 0x04
#define PHASE_CURRENT_BALANCE_FAULT_ENABLE 0x08
#define PHASE_CURRENT_BALANCE_FAULT_CLEAR 0x0C
#define PHASE_CURRENT_BALANCE_FAULT_STATUS 0x10 // read only
#define MOTOR_CONTORL_UIO_IRQ_DISABLE 0x14
#define MUX_SEL 0x18

const std::string MC_Uio::kUioDriverName = "motor_control";

MC_Uio::MC_Uio(): EventControl( /* List of supported Faults */
		{ FaultId::kPhaseImbalance })
{
	mUioHandle = new UioDrv(kUioDriverName);
}

MC_Uio::~MC_Uio()
{
	delete mUioHandle;
}

int MC_Uio::setGateDrive(bool value)
{
    if (value)
            mUioHandle->regWrite(GATE_DRIVE_EN, 0x1);
        else
            mUioHandle->regWrite(GATE_DRIVE_EN, 0x0);
    return 0;
}

uint32_t MC_Uio::getGateDrive()
{
    return mUioHandle->regRead(GATE_DRIVE_EN);
}

int MC_Uio::getEventFd(FaultId event)
{
	// Verify if the event is supported by the driver
	assert(isSupportedEvent(event));
	// Determine the device that needs to be opened for the blocking read
	// open the device and return the FD.
	return -1; //TODO: return file descriptor to /dev/adchub
}

void MC_Uio::enableEvent(FaultId event)
{
	// Verify if the event is supported by the driver
	assert(isSupportedEvent(event));
	// Enable the Fault
}

void MC_Uio::disableEvent(FaultId event)
{
	// Verify if the event is supported by the driver
	assert(isSupportedEvent(event));
	// Disable the Fault
}

bool MC_Uio::getEventStatus(FaultId event)
{
	// Verify if the event is supported by the driver
	assert(isSupportedEvent(event));
	// Return the current status of the fault
	return false;
}
