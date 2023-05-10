#include "adchub.h"
/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

const std::string Adchub::kAdcHubDriverName = "xilinx_adc_hub";

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

Adchub::Adchub()
{
	mAdchub_IIO_Handle = new IIO_Driver(kAdcHubDriverName);
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

bool Adchub::getFaultStatus(FaultType fault)
{
	int data;
	switch (fault)
	{
	case FaultType::kPhaseA_OC:
		data = mAdchub_IIO_Handle->readChannel(current1_ac, "over_range_fault_status");
		break;
	case FaultType::kPhaseB_OC:
		data = mAdchub_IIO_Handle->readChannel(current3_ac, "over_range_fault_status");
		break;
	case FaultType::kPhaseC_OC:
		data = mAdchub_IIO_Handle->readChannel(current5_ac, "over_range_fault_status");
		break;
	case FaultType::kDCLink_OC:
		data = mAdchub_IIO_Handle->readChannel(current7_dc, "over_range_fault_status");
		break;
	case FaultType::kDCLink_OV:
		data = mAdchub_IIO_Handle->readChannel(voltage6_dc, "over_range_fault_status");
		break;
	case FaultType::kDCLink_UV:
		data = mAdchub_IIO_Handle->readChannel(voltage6_dc, "under_range_fault_status");
		break;
	default:
		return false;
	}
	return (data) ? true : false;
}

int Adchub::clearFaults()
{
	for (int index = 0; index < channelMax; index++)
		mAdchub_IIO_Handle->writeChannel(index, "fault_clear", "1");
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

int Adchub::setFiltertap(int filtertap)
{
	for (int index = 0; index < channelMax; index++)
		mAdchub_IIO_Handle->writeChannel(index, "set_filter_tap", std::to_string(filtertap).c_str());
	return 0;
}

int Adchub::set_voltage_threshold_falling_limit(ElectricalData phase, double threshold)
{
	std::string attrName = "thresh_falling_value";
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeeventattr(voltage0_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeeventattr(voltage2_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeeventattr(voltage4_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeeventattr(voltage6_dc, attrName.c_str(), std::to_string(threshold).c_str());
	default:
		return -1;
	}
	return 0;
}

int Adchub::set_voltage_threshold_rising_limit(ElectricalData phase, double threshold)
{
	std::string attrName = "thresh_rising_value";
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeeventattr(voltage0_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeeventattr(voltage2_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeeventattr(voltage4_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeeventattr(voltage6_dc, attrName.c_str(), std::to_string(threshold).c_str());
	default:
		return -1;
	}
	return 0;
}

int Adchub::set_current_threshold_falling_limit(ElectricalData phase, double threshold)
{
	std::string attrName = "thresh_falling_value";
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeeventattr(current1_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeeventattr(current3_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeeventattr(current5_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeeventattr(current7_dc, attrName.c_str(), std::to_string(threshold).c_str());
	default:
		return -1;
	}
	return 0;
}

int Adchub::set_current_threshold_rising_limit(ElectricalData phase, double threshold)
{
	std::string attrName = "thresh_rising_value";
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeeventattr(current1_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeeventattr(current3_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeeventattr(current5_ac, attrName.c_str(), std::to_string(threshold).c_str());
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeeventattr(current7_dc, attrName.c_str(), std::to_string(threshold).c_str());
	default:
		return -1;
	}
	return 0;
}

int Adchub::disable_undervoltage_protection(ElectricalData phase)
{
	std::string attrName = "thresh_falling_en";
	switch (phase)
	{
	case ElectricalData::kPhaseA:
		return mAdchub_IIO_Handle->writeeventattr(voltage0_ac, attrName.c_str(), "0");
	case ElectricalData::kPhaseB:
		return mAdchub_IIO_Handle->writeeventattr(voltage2_ac, attrName.c_str(), "0");
	case ElectricalData::kPhaseC:
		return mAdchub_IIO_Handle->writeeventattr(voltage4_ac, attrName.c_str(), "0");
	case ElectricalData::kDCLink:
		return mAdchub_IIO_Handle->writeeventattr(voltage6_dc, attrName.c_str(), "0");
	default:
		return -1;
	}
	return 0;
}

Adchub::~Adchub()
{
	delete mAdchub_IIO_Handle;
}
