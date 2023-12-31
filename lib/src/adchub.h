/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _ADC_HUB_H
#define _ADC_HUB_H

#include "event_control.h"
#include "interface/iio_drv.h"

// TODO: move AdcChannels Enum within the class namespace
enum AdcChannels
{
	voltage0_ac = 0,
	current1_ac,
	voltage2_ac,
	current3_ac,
	voltage4_ac,
	current5_ac,
	voltage6_dc,
	current7_dc,
	channelMax
};

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

	/*
	 * Buffer handling APIs
	 */
	std::map<AdcChannels, std::vector<double>> fillBuffer(int samples, std::vector<AdcChannels> channels);

private:
	static const std::string kAdcHubDriverName;
	IIO_Driver *mAdchub_IIO_Handle;

	int getChannelId(FaultId event);
	void eventEnableDisable(FaultId event, bool enable);
};

#endif /* _ADC_HUB_H */
