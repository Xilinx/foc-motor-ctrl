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
#include "default_config.h"
#include "mc_driver.h"

/* TODO: implement following
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
	double getTorqueSetValue() override;
	int getSpeedSetValue() override;
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
	mc_uio mMcUio;

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

double MotorControlImpl::getTorqueSetValue()
{
	return mFoc.getTorqueSetValue();
}

int MotorControlImpl::getSpeedSetValue()
{
	return mFoc.getSpeedSetValue();
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
	mFoc.setSpeed(speed);
}

void MotorControlImpl::SetTorque(double torque)
{
	mFoc.setTorque(torque);
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
	transitionMode(MotorOpMode::kModeOff);
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
	return mFoc.getGain(gainController);
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
	switch(target) {
		case MotorOpMode::kModeOff:
			//TODO: disable GD using mc ip ~
			mMcUio.set_gate_drive(false);
			mFoc.stopMotor();
			break;
		case MotorOpMode::kModeSpeed:
		case MotorOpMode::kModeTorque:
		case MotorOpMode::kModeSpeedFW:
			mMcUio.set_gate_drive(true);
			mFoc.setOperationMode(target);
			break;
		case MotorOpMode::kModeOpenLoop:
			//TODO: incorrect use of MotorOpMode. Foc should have its own enum and diff func name
			mFoc.setOperationMode(static_cast<MotorOpMode>(5));
			//TODO: eanble GD
			mMcUio.set_gate_drive(true);
			break;
		default:
			return;
	}
	mCurrentMode = target;
}

void MotorControlImpl::initMotor(bool full_init)
{

	//TODO: Instead of using the values from the default config
	//initialize the private member structure with the default config
	//if the config file is not present.
	mFoc.stopMotor();

	mSvPwm.setSampleII(SVP_SAMPLE_II);
	mSvPwm.setDcLink(SVP_VOLTAGE);
	mSvPwm.setMode(SVP_MODE);

	mPwm.setFrequency(PWM_FREQ);
	mPwm.setDeadCycle(PWM_DEAD_CYC);
	mPwm.setPhaseShift(PWM_PHASE_SHIFT);
	mPwm.setSampleII(PWM_SAMPLE_II);

	mFoc.setAngleOffset(FOC_ANGLE_OFFSET);
	mFoc.setFixedSpeed(VF_FIXED_SPEED); //730 RPM

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
	* calibrating offsets for current channel
	*/
	mAdcHub.calibrateCurrentChannel( ElectricalData::kPhaseA);
	mAdcHub.calibrateCurrentChannel( ElectricalData::kPhaseB);
	mAdcHub.calibrateCurrentChannel(ElectricalData::kPhaseC);
	mAdcHub.calibrateCurrentChannel(ElectricalData::kDCLink);
	/*
	 * set scaling for Voltage and Current
	 */
	for (auto phase : all_Edata) {
		mAdcHub.setVoltageScale(phase, ADCHUB_VOL_SCALE);
		mAdcHub.setCurrentScale(phase, ADCHUB_CUR_SCALE);
	}

	/*
	 * set the thresholds for the fault
	 */
	for (auto phase : all_Edata) {
		mAdcHub.set_voltage_threshold_falling_limit(phase, ADCHUB_VOL_PHASE_FALLING_THRES);
		mAdcHub.set_voltage_threshold_rising_limit(phase, ADCHUB_VOL_PHASE_RISING_THRES);
		mAdcHub.set_current_threshold_falling_limit(phase, ADCHUB_CUR_PHASE_FALLING_THRES);
		mAdcHub.set_current_threshold_rising_limit(phase, ADCHUB_CUR_PHASE_RISING_THRES);
	}
	// Update the thresholds for DCLink. It requires different settings
	mAdcHub.set_current_threshold_falling_limit(ElectricalData::kDCLink,
													ADCHUB_DCLINK_FALLING_THRES);
	mAdcHub.set_current_threshold_rising_limit(ElectricalData::kDCLink,
													ADCHUB_DCLINK_RISING_THRES);

	mAdcHub.setFiltertap(ADCHUB_FILTERTAP); //TODO: Check it is doing for all channels whereas script does only for the currents.

	mAdcHub.clearFaults();

	for (auto phase : all_Edata) {
		mAdcHub.disable_undervoltage_protection(phase);
	}

	mFoc.setGain(GainType::kTorque, TOR_KP, TOR_KI);
	mFoc.setGain(GainType::kFlux, FLUX_KP, FLUX_KI);
	mFoc.setGain(GainType::kSpeed, SPEED_KP, SPEED_KI);
	mFoc.setGain(GainType::kFieldweakening, FW_KP, FW_KI);

	//Note: flux sp is set to zero by motor_stop
	mFoc.setTorque(TOR_SP);
	mFoc.setSpeed(SPEED_SP);

	mFoc.setVfParam(VF_VQ, VF_VD, VF_FIXED_SPEED);

	mMcUio.set_gate_drive(true);

	if(full_init) {
		transitionMode(MotorOpMode::kModeOpenLoop);
		usleep(CALIBRATION_WAIT_US);
	}
}
