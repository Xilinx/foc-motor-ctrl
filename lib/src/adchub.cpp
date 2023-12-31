/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <cassert>
#include "adchub.h"
#include <iostream>

const std::string Adchub::kAdcHubDriverName = "xilinx_adc_hub";

Adchub::Adchub(): EventControl( /* List of supported Faults */
		{ FaultId::kPhaseA_OC,
		  FaultId::kPhaseB_OC,
		  FaultId::kPhaseC_OC,
		  FaultId::kDCLink_OC,
		  FaultId::kDCLink_OV,
		  FaultId::kDCLink_UV,
		})
{
	mAdchub_IIO_Handle = new IIO_Driver(kAdcHubDriverName);

	/*
	 * Disable and clear all the events. Also reset the fault
	 * thresholds.
	 * Note: Although it is simple to use event APIs to clear this,
	 * we are not using them and using channels directly. This is
	 * to enforce reset of the channels which not be exposed through
	 * the event control (e.g voltage{0,2,4}_ac)
	 */

	for (int channel_id=voltage0_ac; channel_id < channelMax; channel_id++) {

		/*
		 * Disable any enabled events/faults
		 */
		mAdchub_IIO_Handle->writeeventattr(channel_id,
						   "thresh_rising_en", "0");
		mAdchub_IIO_Handle->writeeventattr(channel_id,
						   "thresh_falling_en", "0");

		/*
		 * Clear status of any previously triggered events/faults
		 * These also remove the gate latch in the hw
		 */
		mAdchub_IIO_Handle->writeChannel(channel_id,
						 "fault_clear", "1");

		/*
		 * Reset thresholds for all events/faults
		 */
		mAdchub_IIO_Handle->writeeventattr(channel_id,
						   "thresh_rising_value", "0");
		mAdchub_IIO_Handle->writeeventattr(channel_id,
						   "thresh_falling_value", "0");
	}
	// sample buffer data at 100us intervals
	mAdchub_IIO_Handle->writeDeviceattr("sample_interval_us", "100");
}

double Adchub::getCurrent(ElectricalData phase)
{
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->readChannel(current1_ac, "input");
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->readChannel(current3_ac, "input");
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->readChannel(current5_ac, "input");
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->readChannel(current7_dc, "input");
	default:
		return -1;
	}
	return 0;
}

double Adchub::getVoltage(ElectricalData phase)
{
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->readChannel(voltage0_ac, "input");
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->readChannel(voltage2_ac, "input");
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->readChannel(voltage4_ac, "input");
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->readChannel(voltage6_dc, "input");
	default:
		return -1;
	}
	return 0;
}

int Adchub::setVoltageScale(ElectricalData phase, double scale)
{
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeChannel(voltage0_ac, "scale", std::to_string(scale).c_str());
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeChannel(voltage2_ac, "scale", std::to_string(scale).c_str());
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeChannel(voltage4_ac, "scale", std::to_string(scale).c_str());
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeChannel(voltage6_dc, "scale", std::to_string(scale).c_str());
	default:
		return -1;
	}

	return 0;
}

int Adchub::setCurrentScale(ElectricalData phase, double scale)
{
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeChannel(current1_ac, "scale", std::to_string(scale).c_str());
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeChannel(current3_ac, "scale", std::to_string(scale).c_str());
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeChannel(current5_ac, "scale", std::to_string(scale).c_str());
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeChannel(current7_dc, "scale", std::to_string(scale).c_str());
	default:
		return -1;
	}
	return 0;
}

int Adchub::setCurrentFiltertap(ElectricalData phase, int filtertap)
{
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeChannel(current1_ac, "set_filter_tap", std::to_string(filtertap).c_str());
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeChannel(current3_ac, "set_filter_tap", std::to_string(filtertap).c_str());
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeChannel(current5_ac, "set_filter_tap", std::to_string(filtertap).c_str());
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeChannel(current7_dc, "set_filter_tap", std::to_string(filtertap).c_str());
	default:
		return -1;
	}
	return 0;
}

int Adchub::setVoltageFiltertap(ElectricalData phase, int filtertap)
{
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeChannel(voltage0_ac, "set_filter_tap", std::to_string(filtertap).c_str());
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeChannel(voltage2_ac, "set_filter_tap", std::to_string(filtertap).c_str());
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeChannel(voltage4_ac, "set_filter_tap", std::to_string(filtertap).c_str());
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeChannel(voltage6_dc, "set_filter_tap", std::to_string(filtertap).c_str());
	default:
		return -1;
	}
	return 0;
}

int Adchub::calibrateCurrentChannel(ElectricalData phase)
{
	std::string attrName = "calibrate";
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeChannel(current1_ac, attrName.c_str(), "1");
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeChannel(current3_ac, attrName.c_str(), "1");
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeChannel(current5_ac, attrName.c_str(), "1");
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeChannel(current7_dc, attrName.c_str(), "1");
	default:
		return -1;
	}
	return 0;
}

int Adchub::calibrateVoltageChannel(ElectricalData phase)
{
	std::string attrName = "calibrate";
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeChannel(voltage0_ac, attrName.c_str(), "1");
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeChannel(voltage2_ac, attrName.c_str(), "1");
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeChannel(voltage4_ac, attrName.c_str(), "1");
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeChannel(voltage6_dc, attrName.c_str(), "1");
	default:
		return -1;
	}
	return 0;
}

std::map<AdcChannels, std::vector<double>> Adchub::fillBuffer(int samples, std::vector<AdcChannels> channels)
{
	std::map<AdcChannels, std::vector<double>> hubdata;
	std::vector<int> hubVector;
	// Remap incomming list to Adchub Channel list

	for (auto it : channels)
	{
		hubVector.push_back(static_cast<int>(it));
	}

	std::map<int, std::vector<double>> rawDataMap = mAdchub_IIO_Handle->getBufferdata(samples, hubVector);
	if (rawDataMap.size() == 0 && hubVector.size() != 0)
	{
		std::cout << " No data samples obtained from Adchub " << std::endl;
		return hubdata;
	}

	// Typecast channel index to enum in the dictonary
	for (auto &it : rawDataMap)
	{
		AdcChannels newKey = static_cast<AdcChannels>(it.first);
		hubdata[newKey] = it.second;
	}

	return hubdata;
}

int Adchub::getChannelId(FaultId event)
{
	AdcChannels channel_id = channelMax;

	switch (event) {
	case FaultId::kPhaseA_OC:
		channel_id = current1_ac;
		break;
	case FaultId::kPhaseB_OC:
		channel_id = current3_ac;
		break;
	case FaultId::kPhaseC_OC:
		channel_id = current5_ac;
		break;
	case FaultId::kDCLink_OC:
		channel_id = current7_dc;
		break;
	case FaultId::kDCLink_OV:
	case FaultId::kDCLink_UV:
		channel_id = voltage6_dc;
		break;
	default:
		// Should never come here.
		assert(false);
	}

	return (channel_id == channelMax)? -1 : static_cast<int>(channel_id);
}

bool Adchub::getEventStatus(FaultId event)
{
	assert(isSupportedEvent(event));

	bool status = false;
	int channel_id = getChannelId(event);
	std::vector<std::string> attr;

	switch (event) {
	case FaultId::kPhaseA_OC:
	case FaultId::kPhaseB_OC:
	case FaultId::kPhaseC_OC:
		attr.push_back("over_range_fault_status");
		attr.push_back("under_range_fault_status");
		break;
	case FaultId::kDCLink_OC:
	case FaultId::kDCLink_OV:
		attr.push_back("over_range_fault_status");
		break;
	case FaultId::kDCLink_UV:
		attr.push_back("under_range_fault_status");
		break;
	default:
		break;
	}

	if (channel_id != -1) {
		for (auto a : attr) {
			status = status ||
				 mAdchub_IIO_Handle->readChannel(channel_id,
								 a.c_str());
		}
	}

	return status;
}

void Adchub::clearEvent(FaultId event)
{
	assert(isSupportedEvent(event));

	int channel_id = getChannelId(event);

	if (channel_id != -1) {
		mAdchub_IIO_Handle->writeChannel(channel_id,
						 "fault_clear", "1");
	}
}

void Adchub::setUpperThreshold(FaultId event, double val)
{
	assert(isSupportedEvent(event));
	int channel_id = getChannelId(event);
	std::string attrName = "thresh_rising_value";

	if (channel_id != -1) {
		mAdchub_IIO_Handle->writeeventattr(channel_id,
						   attrName.c_str(),
						   std::to_string(val).c_str());
	}
}

void Adchub::setLowerThreshold(FaultId event, double val)
{
	assert(isSupportedEvent(event));
	int channel_id = getChannelId(event);
	std::string attrName = "thresh_falling_value";

	if (channel_id != -1) {
		mAdchub_IIO_Handle->writeeventattr(channel_id,
						   attrName.c_str(),
						   std::to_string(val).c_str());
	}
}

double Adchub::getUpperThreshold(FaultId event)
{
	assert(isSupportedEvent(event));

	double threshold = 0;
	int channel_id = getChannelId(event);
	std::string attrName = "thresh_rising_value";

	if (channel_id != -1) {
		threshold = mAdchub_IIO_Handle->readeventattr(channel_id, attrName);
	}
	return threshold;
}

double Adchub::getLowerThreshold(FaultId event)
{
	assert(isSupportedEvent(event));

	double threshold = 0;
	int channel_id = getChannelId(event);
	std::string attrName = "thresh_falling_value";

	if (channel_id != -1) {
		threshold = mAdchub_IIO_Handle->readeventattr(channel_id, attrName);
	}
	return threshold;
}

int Adchub::getEventFd(FaultId event)
{
	assert(isSupportedEvent(event));
	return mAdchub_IIO_Handle->getEventFd();
}

void Adchub::eventEnableDisable(FaultId event, bool enable)
{
	int channel_id = getChannelId(event);
	std::vector<std::string> attr;

	switch (event) {
	case FaultId::kPhaseA_OC:
	case FaultId::kPhaseB_OC:
	case FaultId::kPhaseC_OC:
		attr.push_back("thresh_rising_en");
		attr.push_back("thresh_falling_en");
		break;
	case FaultId::kDCLink_OC:
	case FaultId::kDCLink_OV:
		attr.push_back("thresh_rising_en");
		break;
	case FaultId::kDCLink_UV:
		attr.push_back("thresh_falling_en");
		break;
	default:
		break;
	}

	if (channel_id != -1) {
		for (auto a : attr) {
			mAdchub_IIO_Handle->writeeventattr(channel_id,
							   a.c_str(),
							   std::to_string(enable).c_str());
		}
	}
}

void Adchub::enableEvent(FaultId event)
{
	assert(isSupportedEvent(event));
	eventEnableDisable(event, true);
}

void Adchub::disableEvent(FaultId event)
{
	assert(isSupportedEvent(event));
	eventEnableDisable(event, false);
}

Adchub::~Adchub()
{
	delete mAdchub_IIO_Handle;
}
