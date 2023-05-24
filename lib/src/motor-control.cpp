/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <unistd.h>
#include "motor-control/motor-control.hpp"
#include "sensors/sensor.h"
#include "foc.h"
#include "adchub.h"
#include "pwm.h"
#include "svpwm.h"
/* TODO: implement following
#include "mc_driver.hpp"
#include "logging.hpp"
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
	double getCurrent(ElectricalData type) override;
	double getVoltage(ElectricalData type) override;
	bool getFaultStatus(FaultType type) override;
	FocData getFocCalc() override;
	MotorOpMode getOperationMode() override;
	GainData GetGain(GainType gainController) override;

	void SetSpeed(double speed) override;
	void SetTorque(double torque) override;
	void SetPosition(int position) override;
	void SetGain(GainType gainController, GainData value) override;
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
	MotorOpMode mCurrentMode;

	/*
	 * Persistent Settings
	 */
	struct {
		int rampRate;

	}mConfigData;


	/*
	 * Class handlers
	 */
	Foc mFoc;
	Pwm mPwm;
	Svpwm mSvPwm;
	Adchub mAdcHub;
	Sensor *mpSensor;

	void parseConfig();
	void transitionMode(MotorOpMode target);
	void initMotor(bool full_init);
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
	mpSensor = Sensor::getSensorInstance();

	initMotor(true);

	transitionMode(MotorOpMode::kModeOff);
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

double MotorControlImpl::getCurrent(ElectricalData type)
{
	return mAdcHub.getCurrent(type);
}

double MotorControlImpl::getVoltage(ElectricalData type)
{
	return mAdcHub.getVoltage(type);
}

bool MotorControlImpl::getFaultStatus(FaultType type)
{
	return mAdcHub.getFaultStatus(type);
}

FocData MotorControlImpl::getFocCalc()
{
	return mFoc.getChanData();
}

void MotorControlImpl::SetSpeed(double speed)
{

}

void MotorControlImpl::SetTorque(double torque)
{

}

void MotorControlImpl::SetPosition(int position)
{
	/*
	 * Not availabe untill position control
	 */
}

void MotorControlImpl::SetGain(GainType gainController, GainData value)
{
	mFoc.setGain(gainController, value.kp, value.ki);
}

void MotorControlImpl::clearFaults()
{
	mAdcHub.clearFaults();
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

GainData MotorControlImpl::GetGain(GainType gainController)
{
	return GainData({0,0});
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
	/*
	 * Check if the transition is possible
	 */
	mCurrentMode = target;
}

void MotorControlImpl::initMotor(bool full_init)
{
	mFoc.stopMotor();

	mSvPwm.setSampleII(1);
	mSvPwm.setDcLink(24);
	mSvPwm.setMode(0);

	mPwm.setFrequency(96800);
	mPwm.setDeadCycle(2);
	mPwm.setPhaseShift(0);
	mPwm.setSampleII(1);

	mFoc.setAngleOffset(28);
	mFoc.setFixedSpeed(0xCE4); //TODO: Fix Scaling (750?)

	mPwm.startPwm();
	mSvPwm.startSvpwm();
	mpSensor->start();
	mFoc.startFoc();


	vector <ElectricalData> all_Edata = {ElectricalData::kPhaseA,
					ElectricalData::kPhaseB,
					ElectricalData::kPhaseC,
					ElectricalData::kDCLink,
					};

	/*
	 * set scaling for Voltage and Current
	 */
	for (auto phase : all_Edata) {
		mAdcHub.setVoltageScale(phase, 0.018);
		mAdcHub.setCurrentScale(phase, 0.005);
	}

	/*
	 * set the thresholds for the fault
	 */
	for (auto phase : all_Edata) {
		mAdcHub.set_voltage_threshold_falling_limit(phase, 0.5);
		mAdcHub.set_voltage_threshold_rising_limit(phase, 28);
		mAdcHub.set_current_threshold_falling_limit(phase, -2.7);
		mAdcHub.set_current_threshold_rising_limit(phase, 2.7);
	}
	// TODO:Check mAdcHub.set_voltage_threshold_falling_limit(ElectricalData::kDCLink, 0.5); // script doesn't set it.
	// DCLink require different settings
	mAdcHub.set_current_threshold_falling_limit(ElectricalData::kDCLink, -0.625);
	mAdcHub.set_current_threshold_rising_limit(ElectricalData::kDCLink, 0.625);

	mAdcHub.setFiltertap(16); //TODO: Check it is doing for all channels whereas script does only for the currents.

	mAdcHub.clearFaults();

	for (auto phase : all_Edata) {
		mAdcHub.disable_undervoltage_protection(phase);
	}

	//TODO: check Scaling for setGain and Torque. Check the values
	mFoc.setGain(GainType::kTorque, 151552, 80);
	mFoc.setGain(GainType::kFlux, 123360, 40);
	mFoc.setGain(GainType::kSpeed, 650, 5);
	mFoc.setGain(GainType::kFieldweakening, 65536, 218);

	//Note: flux sp is set to zero by motor_stop
	mFoc.setTorque(28945); // incorrect scalling ??
	mFoc.setSpeed(500); // after scaling : 32768000

	mFoc.setVfParam(131072, 4294927975, 578);

	//TODO: enable GD using mc ip

	if(full_init) {
		//TODO: incorrect use of MotorOpMode. Foc should have its own enum and diff func name
		mFoc.setOperationMode(static_cast<MotorOpMode>(5)); // open loop
		usleep(100 * 1000);
		mFoc.setOperationMode(static_cast<MotorOpMode>(1)); // speed Mode
	}
}

