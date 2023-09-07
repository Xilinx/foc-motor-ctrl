#include "softwarefaults.h"

SoftwareFaults::SoftwareFaults(Adchub *adchub, MC_Uio *uio) : EventControl(
							  {FaultId::kAvgPowerFault}),mAvgP(adchub, uio)
{
	mAvgPThread = std::thread(&AVGPower::generateFault, &mAvgP);
	mAvgPThread.detach();
}

SoftwareFaults::~SoftwareFaults()
{
	mAvgPThread.~thread();
}

bool SoftwareFaults::getEventStatus(FaultId event)
{
	assert(isSupportedEvent(event));
	switch (event)
	{
	case FaultId::kAvgPowerFault:
		return mAvgP.getFaultStatus();
	default:
		assert(false);
	}
}

int SoftwareFaults::getEventFd(FaultId event)
{
	assert(isSupportedEvent(event));
	switch (event)
	{
	case FaultId::kAvgPowerFault:
		return mAvgP.getFd();
	default:
		assert(false);
	}
}

void SoftwareFaults::enableEvent(FaultId event)
{
	assert(isSupportedEvent(event));
	switch (event)
	{
	case FaultId::kAvgPowerFault:
		mAvgP.enableFault(true);
		break;
	default:
		assert(false);
	}
}

void SoftwareFaults::disableEvent(FaultId event)
{
	assert(isSupportedEvent(event));
	switch (event)
	{
	case FaultId::kAvgPowerFault:
		mAvgP.enableFault(false);
		break;
	default:
		assert(false);
	}
}

void SoftwareFaults::clearEvent(FaultId event)
{
	assert(isSupportedEvent(event));
	switch (event)
	{
	case FaultId::kAvgPowerFault:
		mAvgP.clearFault();
		break;
	default:
		assert(false);
	}
}

void SoftwareFaults::setUpperThreshold(FaultId event, double val)
{
	assert(isSupportedEvent(event));
	switch (event)
	{
	case FaultId::kAvgPowerFault:
		mAvgP.setThresholdValue(val);
		break;
	default:
		assert(false);
	}
}

void SoftwareFaults::setLowerThreshold(FaultId event, double val)
{
	assert(isSupportedEvent(event));
}

double SoftwareFaults::getUpperThreshold(FaultId event)
{
	assert(isSupportedEvent(event));
	switch (event)
	{
	case FaultId::kAvgPowerFault:
		return mAvgP.getThresholdValue();
	default:
		assert(false);
	}
}

double SoftwareFaults::getLowerThreshold(FaultId event)
{
	assert(isSupportedEvent(event));
	return 0.0;
}
