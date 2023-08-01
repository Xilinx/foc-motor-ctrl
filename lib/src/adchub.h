/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _ADC_HUB_H
#define _ADC_HUB_H

#include "../include/motor-control/motor-control.hpp"
#include "event_control.h"
#include "interface/iio_drv.h"

enum class ElectricalData;
enum class FaultType;

class Adchub : public EventControl
{
public:
	Adchub();
	double getCurrent(ElectricalData phase);
	double getVoltage(ElectricalData phase);
	int clearFaults();
	int setVoltageScale(ElectricalData phase, double scale);
	int setCurrentScale(ElectricalData phase, double scale);
	int setCurrentFiltertap(ElectricalData phase, int filtertap);
	int setVoltageFiltertap(ElectricalData phase, int filtertap);
	int set_voltage_threshold_falling_limit(ElectricalData phase, double threshold);
	int set_voltage_threshold_rising_limit(ElectricalData phase, double threshold);
	int set_current_threshold_falling_limit(ElectricalData phase, double threshold);
	int set_current_threshold_rising_limit(ElectricalData phase, double threshold);
	int calibrateCurrentChannel(ElectricalData phase);
	int calibrateVoltageChannel(ElectricalData phase);
	int disable_undervoltage_protection(ElectricalData phase);
	~Adchub();

	// Fault Handling
	bool getEventStatus(FaultType event) override;
	int getEventFd(FaultType event) override;
	void enableEvent(FaultType event) override;
	void disableEvent(FaultType event) override;

private:
	static const std::string kAdcHubDriverName;
	IIO_Driver *mAdchub_IIO_Handle;
};

#endif /* _ADC_HUB_H */
