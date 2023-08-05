/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <cassert>
#include "mc_driver.h"
#include "fcntl.h"
#include "unistd.h"

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

MC_Uio::MC_Uio() : EventControl(/* List of supported Faults */
		 {FaultId::kPhaseImbalance})
{
	std::string uioPath;
	fd = -1;

	mUioHandle = new UioDrv(kUioDriverName);
	mUioHandle->findUioDevicenode(uioPath);
	fd = open(uioPath.c_str(), O_RDWR);
	if (fd < 0) {
		perror("open");
	}
}

MC_Uio::~MC_Uio()
{
	close(fd);
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
	assert(isSupportedEvent(event));
	return fd;
}

void MC_Uio::enableEvent(FaultId event)
{
	assert(isSupportedEvent(event));
	mUioHandle->regWrite(MOTOR_CONTORL_UIO_IRQ_DISABLE, 0x0);
	mUioHandle->regWrite(PHASE_CURRENT_BALANCE_FAULT_ENABLE, 0x1);
}

void MC_Uio::disableEvent(FaultId event)
{
	assert(isSupportedEvent(event));
	mUioHandle->regWrite(MOTOR_CONTORL_UIO_IRQ_DISABLE, 0x1);
	mUioHandle->regWrite(PHASE_CURRENT_BALANCE_FAULT_ENABLE, 0x0);
}

bool MC_Uio::getEventStatus(FaultId event)
{
	assert(isSupportedEvent(event));
	return static_cast<bool>(mUioHandle->regRead(PHASE_CURRENT_BALANCE_FAULT_STATUS));
}

void MC_Uio::clearEvent(FaultId event)
{
	uint32_t info = 1; /* unmask uio irq */

	assert(isSupportedEvent(event));
	mUioHandle->regWrite(PHASE_CURRENT_BALANCE_FAULT_CLEAR, 0xbad00ff);
	write(fd, &info, sizeof(info));
}

void MC_Uio::setUpperThreshold(FaultId event, double val)
{
	assert(isSupportedEvent(event));
	int ival = val * 65536;
	mUioHandle->regWrite(PHASE_CURRENT_BALANCE_FAULT_VALUE, ival);
}

double MC_Uio::getUpperThreshold(FaultId event)
{
	assert(isSupportedEvent(event));
	return mUioHandle->regRead(PHASE_CURRENT_BALANCE_FAULT_VALUE) / 65536.0;
}
