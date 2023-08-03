/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _ADC_HUB_H
#define _ADC_HUB_H

#include "../include/motor-control/motor-control.hpp"
#include "event_control.h"
#include "interface/iio_drv.h"

class Adchub : public EventControl
{
public:
	Adchub();
	~Adchub();
	double getCurrent(ElectricalData phase);
	double getVoltage(ElectricalData phase);
	int setVoltageScale(ElectricalData phase, double scale);
	int setCurrentScale(ElectricalData phase, double scale);
	int setCurrentFiltertap(ElectricalData phase, int filtertap);
	int setVoltageFiltertap(ElectricalData phase, int filtertap);
	int calibrateCurrentChannel(ElectricalData phase);
	int calibrateVoltageChannel(ElectricalData phase);

	/*
	 * Following APIs are deprecated in favor of Fault Handling
	 */
	int clearFaults();
	int set_voltage_threshold_falling_limit(ElectricalData phase, double threshold);
	int set_voltage_threshold_rising_limit(ElectricalData phase, double threshold);
	int set_current_threshold_falling_limit(ElectricalData phase, double threshold);
	int set_current_threshold_rising_limit(ElectricalData phase, double threshold);
	int disable_undervoltage_protection(ElectricalData phase);

	/*
	 * Fault handling Event control APIs
	 */
	bool getEventStatus(FaultId event) override;
	int getEventFd(FaultId event) override;
	void enableEvent(FaultId event) override;
	void disableEvent(FaultId event) override;
	void clearEvent(FaultId event) override;
	void setUpperThreshold(FaultId event, double val) override;
	void setLowerThreshold(FaultId event, double val) override;
	double getUpperThreshold(FaultId event) override;
	double getLowerThreshold(FaultId event) override;

private:
	static const std::string kAdcHubDriverName;
	IIO_Driver *mAdchub_IIO_Handle;

	int fd;
	int getChannelId(FaultId event);
	void eventEnableDisable(FaultId event, bool enable);
};

#endif /* _ADC_HUB_H */
