/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

//#include "logging.hpp"
#include "motor-control/motor-control.hpp"
#include "sensors/sensor.h"
/* TODO: implement following
#include "adchub.hpp"
#include "foc.hpp"
#include "pwm.hpp"
#include "mc_driver.hpp"
*/

/** TODO List:
 *   - Move fucntion definitions out of the class
 *   - Use smart pointers if make sense
 *   - populate InitConfig structure
 *	- If possible group the drivers specific init as sub strutures defined
 *	in respective drivers
 *   - Check Naming convention and use '_' after the specifier if required.
 */

using namespace std;

/*
 * TODO: config struct should be part of the implemenation class
 */
struct InitCofig {
	int foc_RampRate;
};

class MotorControlImpl : public MotorControl {
public:

	MotorControlImpl(int sessionId = 0, string configPath = DEFAULT_CONFIG_PATH);
	~MotorControlImpl();

	/*
	 * Override the MotorControl APIs for the
	 * actual implementations.
	 */
	int getSpeed() override;
	int getPosition() override;
	int getTorque() override;
	int getCurrent(ElectricalData type) override;
	int getVoltage(ElectricalData type) override;
	bool getFaultStatus(FaultType type) override;
	FocData getFocCalc() override;
	MotorOpMode getOperationMode() override;

	void SetSpeed(int speed) override;
	void SetTorque(int torque) override;
	void SetPosition(int position) override;
	void SetGain(GainType gainController, int k_p, int k_i) override;
	void setOperationMode(MotorOpMode mode) override;

	void clearFaults() override;
	void clearFaults(FaultCategory category) override;

	/*
	 * Implementation specific members
	 */

	int getSessionId ();

private:
	int mSessionId;
	string mConfigPath;
	InitCofig mConfig;
	MotorOpMode mCurrentMode;

	/*
	 * Class handlers
	 */
	Sensor *mpSensor;
	/* TODO:
	FOCImpl *mpFoc;
	ADCHub *mpAdcHub;
	PWMImpl *mpPwm;
	*/

	void parseConfig();
	void transitionMode(MotorOpMode target);
};

// Initialize the static member variable of the MotorControl class
MotorControl *MotorControl::mspInstance = nullptr;

MotorControl::~MotorControl()
{
	if (mspInstance) { /* sanity check, though not needed */
		mspInstance = nullptr;
		// Note: Do not need to delete mspInstance, as delete of this
		// ptr implies deletion of the derived class instance
	}
}

MotorControl *MotorControl::getMotorControlInstance(int sessionId,
						    string configPath)
{
	MotorControl *ptr = nullptr;
	if (mspInstance) {
		MotorControlImpl *impl_ptr = dynamic_cast<MotorControlImpl *>(mspInstance);
		if (impl_ptr->getSessionId() == sessionId) {
			// Check if it is called for same sessionId
			// TODO: Implement restriction for subsequent calls
			// here.
			ptr = mspInstance;
		}
	} else {
		mspInstance = new MotorControlImpl(sessionId, configPath);
		ptr = mspInstance;
	}
	return ptr;
}

MotorControlImpl::MotorControlImpl(int sessionId, string configPath):
	mSessionId(sessionId), mConfigPath(configPath)
{
	parseConfig();
	// init all the driving classes
	mpSensor=Sensor::getSensorInstance();

	// TODO: Perform init sequence and set the current Mode
	mCurrentMode = MotorOpMode::kModeOff;
}

MotorControlImpl::~MotorControlImpl()
{
	mSessionId =-1;
	delete mpSensor;
}

int MotorControlImpl::getSpeed()
{
	return mpSensor->getSpeed();
}

int MotorControlImpl::getPosition()
{
	return mpSensor->getPosition();
}

int MotorControlImpl::getTorque()
{
	/*
	 * To Implement only after Torque sensor is available.
	 */
	return 0;
}

int MotorControlImpl::getCurrent(ElectricalData type)
{
	return 0;
}

int MotorControlImpl::getVoltage(ElectricalData type)
{
	return 0;
}

bool MotorControlImpl::getFaultStatus(FaultType type)
{
	return false;
}

FocData MotorControlImpl::getFocCalc()
{
	FocData data = {0,0,0,0,0,0,0,0};
	return data;
}

void MotorControlImpl::SetSpeed(int speed)
{

}

void MotorControlImpl::SetTorque(int torque)
{

}

void MotorControlImpl::SetPosition(int position)
{

}

void MotorControlImpl::SetGain(GainType gainController, int k_p, int k_i)
{

}

void MotorControlImpl::clearFaults()
{

}

void MotorControlImpl::clearFaults(FaultCategory category)
{

}

int MotorControlImpl::getSessionId ()
{
	return mSessionId;
}

void MotorControlImpl::parseConfig()
{
	// parse the config using mConfigPath
	// and fillup mCofig structure
}

MotorOpMode MotorControlImpl::getOperationMode()
{
	return mCurrentMode;
}

void MotorControlImpl::setOperationMode(MotorOpMode mode)
{
	if(mode != mCurrentMode)
	{
		transitionMode(mode);
	}
}

void MotorControlImpl::transitionMode(MotorOpMode target)
{
	// TODO: Transition to target mode
	mCurrentMode = target;
}
