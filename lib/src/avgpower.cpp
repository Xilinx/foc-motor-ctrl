#include "avgpower.h"

AVGPower::AVGPower(Adchub* adchub, MC_Uio* mcuio) : madc(adchub), muio(mcuio)
{
	AVGPowerRegister.faultEnable = false;
	AVGPowerRegister.faultStatus = false;
	mAvgFd = eventfd(0, 0);
}

AVGPower::~AVGPower()
{
	close(mAvgFd);
	mAvgFd = -1;
}

void AVGPower::setThresholdValue(double value)
{
	AVGPowerRegister.faultThreshold = value;
}

double AVGPower::getThresholdValue()
{
	return AVGPowerRegister.faultThreshold;
}

int AVGPower::getFd()
{
	return mAvgFd;
}

void AVGPower::enableFault(bool value)
{
	AVGPowerRegister.faultEnable = value;
}

void AVGPower::clearFault()
{
	AVGPowerRegister.faultStatus = false;
}

bool AVGPower::getFaultStatus()
{
	return AVGPowerRegister.faultStatus;
}

double AVGPower::calculateAvgPower()
{
	double integralPower = 0;
	for (int i = 0; i < 500; i++)
	{
		double dcCurrent = madc->getCurrent(ElectricalData::kDCLink);
		double dcVoltage = madc->getVoltage(ElectricalData::kDCLink);
		integralPower += (dcCurrent * dcVoltage);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return (integralPower / 500);
}

void AVGPower::generateFault()
{
	const uint64_t exit_signal = 1;

	while (true)
	{
		double avgPower = calculateAvgPower();
		if ((avgPower > AVGPowerRegister.faultThreshold) && AVGPowerRegister.faultEnable)
		{
			muio->setGateDrive(false);
			AVGPowerRegister.faultStatus = true;
			eventfd_write(mAvgFd, exit_signal);
			std::cerr << "Tripping Gate Drive as Average power: " << avgPower << " is greater than rated Power" << std::endl;
		}
	}
}
