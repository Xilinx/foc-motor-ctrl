/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <unistd.h>
#include <cassert>
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include "motor-control/motor-control.hpp"
#include "sensors/sensor.h"
#include "foc.h"
#include "adchub.h"
#include "pwm.h"
#include "svpwm.h"
#include "softwarefaults.h"
#include "mc_driver.h"
#include "event_manager.h"
#include "config_parameters.h"

/** TODO List:
 *   - populate InitConfig structure
 *	- If possible group the drivers specific init as sub strutures defined
 *	in respective drivers
 *   - Implement logging facility.
 *   - Implement exceptions for the runtime errors.
 */

#define ANGLE2CPR(x)    (((x) * mInitConfig[kParamCpr] )/360)

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
	bool getFaultStatus(FaultId id) override;
	FocData getFocCalc() override;
	MotorOpMode getOperationMode() override;
	GainData GetGain(GainType gainController) override;
	double getVfParamVq() override;
	double getVfParamVd() override;
	string getConfigName() override;

	void SetSpeed(double speed) override;
	void SetTorque(double torque) override;
	void SetPosition(int position) override;
	void SetGain(GainType gainController, GainData value) override;
	void setOperationMode(MotorOpMode mode) override;
	void setVfParamVq(double vq) override;
	void setVfParamVd(double vd) override;

	void clearFaults() override;

	map<MotorParam, vector<double>> getMotorParams(int numSamples, vector<MotorParam> list) override;

	/*
	 * Implementation specific members
	 */

	int getSessionId ();
	bool mInitDone;

private:
	int mSessionId;
	MotorOpMode mCurrentMode;

	/*
	 * Class handlers
	 */
	Foc mFoc;
	Pwm mPwm;
	Svpwm mSvPwm;
	Adchub mAdcHub;
	MC_Uio mMcUio;
	Sensor *mpSensor;
	SoftwareFaults mSfaults;
	EventManager mEvents;
	MotorConfigParams mInitConfig;

	/*
	 * Shadow Vq and Vd
	 * TODO: Find the right place
	 */
	double mVq;
	double mVd;

	int initMotor(bool);
	Foc::OpMode getFocOpMode(MotorOpMode);
	int transitionMode(Foc::OpMode);
	bool isSupportedMode(MotorOpMode);
	struct PlatformChannels parseMotorParams(std::vector<MotorParam> motoParamList);
	MotorParam remapFoc (FocChannel channel);
	MotorParam remapQei (Qeichannel channel);
	MotorParam remapHub (AdcChannels channel);

};

// Initialize the static member variable of the MotorControl class
MotorControl *MotorControl::mspInstance = nullptr;

struct PlatformChannels {
		std::vector<AdcChannels> adcChannelList;
		std::vector<FocChannel> focChannelList;
		std::vector<Qeichannel> qeichannelList;
};

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
		if (dynamic_cast<MotorControlImpl *>(mspInstance)->mInitDone) {
			ptr = mspInstance;
		} else {
			std::cerr << "Failed to create motor control object" << std::endl;
			delete mspInstance;
		}
	}
	return ptr;
}

MotorControlImpl::MotorControlImpl(int sessionId, string configPath):
	mSessionId(sessionId),
	mCurrentMode(MotorOpMode::kModeMax),
	mSfaults(&mAdcHub, &mMcUio),
	mEvents({&mAdcHub, &mMcUio, &mSfaults})

{
	mInitConfig.overrideConfigs(configPath);
	mFoc.updateConfig(mInitConfig[kParamSpeedRrate], mInitConfig[kParamTorqueRrate],
			mInitConfig[kParamRstSpeed], mInitConfig[kParamRstTorque]);
	// init all the driving classes
	mpSensor = Sensor::getSensorInstance();

	mInitDone = false;
	if (initMotor(true) == 0)
		mInitDone = true;

	setOperationMode(MotorOpMode::kModeOff);
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
	//TODO: NOT IMPLEMENTED
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

bool MotorControlImpl::getFaultStatus(FaultId id)
{
	return mEvents.getStatus(id);
}

FocData MotorControlImpl::getFocCalc()
{
	return mFoc.getChanData();
}

double MotorControlImpl::getVfParamVq()
{
	return mVq;
}

double MotorControlImpl::getVfParamVd()
{
	return mVd;
}

string MotorControlImpl::getConfigName()
{
	return mInitConfig.mName;
}

void MotorControlImpl::setVfParamVd(double vd)
{
	mVd = vd;
	mFoc.setVfParam(mVq, mVd);
}

void MotorControlImpl::setVfParamVq(double vq)
{
	mVq = vq;
	mFoc.setVfParam(mVq, mVd);
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
	//TODO: NOT IMPLEMENTED
	static_cast<void>(position);
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
	setOperationMode(MotorOpMode::kModeOff);
	mEvents.resetAllEvents();
	mEvents.activateAllEvents();

	//TODO: Check if the faults still exists and report
}

int MotorControlImpl::getSessionId ()
{
	return mSessionId;
}

MotorOpMode MotorControlImpl::getOperationMode()
{
	return mCurrentMode;
}

GainData MotorControlImpl::GetGain(GainType gainController)
{
	return mFoc.getGain(gainController);
}

bool MotorControlImpl::isSupportedMode(MotorOpMode mode)
{
	bool supported = false;
	switch(mode) {
		case MotorOpMode::kModeOff:
		case MotorOpMode::kModeSpeed:
		case MotorOpMode::kModeTorque:
		case MotorOpMode::kModeSpeedFW:
		case MotorOpMode::kModeOpenLoop:
			supported = true;
			break;
		default:
			supported = false;
	}
	return supported;
}

struct PlatformChannels MotorControlImpl::parseMotorParams(std::vector<MotorParam> motoParamList)
{

	PlatformChannels parseData;
	for (auto it : motoParamList)
	{
		switch (it)
		{
		case MotorParam::kVoltagePhaseA:
			parseData.adcChannelList.push_back(AdcChannels::voltage0_ac);
			break;
		case MotorParam::kCurrentPhaseA:
			parseData.adcChannelList.push_back(AdcChannels::current1_ac);
			break;
		case MotorParam::kVoltagePhaseB:
			parseData.adcChannelList.push_back(AdcChannels::voltage2_ac);
			break;
		case MotorParam::kCurrentPhaseB:
			parseData.adcChannelList.push_back(AdcChannels::current3_ac);
			break;
		case MotorParam::kVoltagePhaseC:
			parseData.adcChannelList.push_back(AdcChannels::voltage4_ac);
			break;
		case MotorParam::kCurrentPhaseC:
			parseData.adcChannelList.push_back(AdcChannels::current5_ac);
			break;
		case MotorParam::kVoltageDCLink:
			parseData.adcChannelList.push_back(AdcChannels::voltage6_dc);
			break;
		case MotorParam::kCurrentDCLink:
			parseData.adcChannelList.push_back(AdcChannels::current7_dc);
			break;
		case MotorParam::kId:
			parseData.focChannelList.push_back(FocChannel::Id);
			break;
		case MotorParam::kIq:
			parseData.focChannelList.push_back(FocChannel::Iq);
			break;
		case MotorParam::kIalpha:
			parseData.focChannelList.push_back(FocChannel::I_alpha);
			break;
		case MotorParam::kIbeta:
			parseData.focChannelList.push_back(FocChannel::I_beta);
			break;
		case MotorParam::kIhomopolar:
			parseData.focChannelList.push_back(FocChannel::I_homopolar);
			break;
		case MotorParam::kSpeedFoc:
			parseData.focChannelList.push_back(FocChannel::speed_pi_out);
			break;
		case MotorParam::kTorqueFoc:
			parseData.focChannelList.push_back(FocChannel::torque_pi_out);
			break;
		case MotorParam::kFlux:
			parseData.focChannelList.push_back(FocChannel::flux);
			break;
		case MotorParam::kRpmFoc:
			parseData.focChannelList.push_back(FocChannel::rpm);
			break;
		case MotorParam::kPositionFoc:
			parseData.focChannelList.push_back(FocChannel::position);
			break;
		case MotorParam::kRpm:
			parseData.qeichannelList.push_back(Qeichannel::RPM);
			break;
		case MotorParam::kPosition:
			parseData.qeichannelList.push_back(Qeichannel::THETA);
			break;
		default:
			break;
		}
	}
	return parseData;
}

MotorParam MotorControlImpl::remapHub (AdcChannels channel)
{
	switch (static_cast<int>(channel)){
	case 0:
		return MotorParam::kVoltagePhaseA;
	case 1:
		return MotorParam::kCurrentPhaseA;
	case 2:
		return MotorParam::kVoltagePhaseB;
	case 3:
		return MotorParam::kCurrentPhaseB;
	case 4:
		return MotorParam::kVoltagePhaseC;
	case 5:
		return MotorParam::kCurrentPhaseC;
	case 6:
		return MotorParam::kVoltageDCLink;
	case 7:
		return MotorParam::kCurrentDCLink;
	default:
		assert(false);
	}
}

MotorParam MotorControlImpl::remapFoc(FocChannel channel)
{
	switch (static_cast<int>(channel)){
	case 0:
		return MotorParam::kId;
	case 1:
		return MotorParam::kIq;
	case 2:
		return MotorParam::kIalpha;
	case 3:
		return MotorParam::kIbeta;
	case 4:
		return MotorParam::kIhomopolar;
	case 5:
		return MotorParam::kSpeedFoc;
	case 6:
		return MotorParam::kTorqueFoc;
	case 7:
		return MotorParam::kFlux;
	case 8:
		return MotorParam::kRpmFoc;
	case 9:
		return MotorParam::kPositionFoc;
	default:
		assert(false);
	}
}

MotorParam MotorControlImpl::remapQei(Qeichannel channel)
{
	switch (static_cast<int>(channel)){
	case 0:
		return MotorParam::kRpm;
	case 1:
		return MotorParam::kPosition;
	default:
		assert(false);
	}
}

Foc::OpMode MotorControlImpl::getFocOpMode(MotorOpMode mode)
{
	Foc::OpMode target = Foc::OpMode::kModeMax;

	switch(mode) {
		case MotorOpMode::kModeOff:
			target = Foc::OpMode::kModeStop;
			break;
		case MotorOpMode::kModeSpeed:
			target = Foc::OpMode::kModeSpeed;
			break;
		case MotorOpMode::kModeTorque:
			target = Foc::OpMode::kModeTorque;
			break;
		case MotorOpMode::kModeSpeedFW:
			target = Foc::OpMode::kModeFlux;
			break;
		case MotorOpMode::kModeOpenLoop:
			target = Foc::OpMode::kModeManualTF;
			break;
		default:
			break;
	}

	return target;
}

void MotorControlImpl::setOperationMode(MotorOpMode mode)
{
	assert(isSupportedMode(mode));

	if(mode != mCurrentMode)
	{
		auto foc_mode = getFocOpMode(mode);

		assert(foc_mode != Foc::OpMode::kModeMax);

		if(mode != MotorOpMode::kModeOff) {
			/*
			 * Transition to OFF mode before changing mode
			 */
			transitionMode(Foc::OpMode::kModeStop);
			/*
			 * After OFF mode, make sure the spinning motor has
			 * ramped down atleast close to the RESET value (+100).
			 * Max wait period is 5 seconds. The check is every
			 * 100ms.
			 */
			int loop_timeout_ms = 5000;
			int loop_sleep_ms = 100;
			while (std::abs(getSpeed()) > mInitConfig[kParamRstSpeed] + 100) {
				auto sleep_time =
					std::chrono::milliseconds(loop_sleep_ms);
				std::this_thread::sleep_for(sleep_time);
				loop_timeout_ms -= loop_sleep_ms;
				if (loop_timeout_ms < 0) {
					//assert(false && "Motor Off loop timeout");
					std::cerr << "Motor didn't stop. Loop timed out. Something Wrong!!" << std::endl;
					break;
				}
			}
		}
		if (transitionMode(foc_mode) == 0)
			mCurrentMode = mode;
	}
}

int MotorControlImpl::transitionMode(Foc::OpMode target)
{
	bool run_motor = false;
	switch(target) {
		case Foc::OpMode::kModeStop:
			run_motor = false;
			break;
		case Foc::OpMode::kModeSpeed:
		case Foc::OpMode::kModeTorque:
		case Foc::OpMode::kModeFlux:
		case Foc::OpMode::kModeManualTFFixedSpeed:
		case Foc::OpMode::kModeManualTF:
		case Foc::OpMode::kModeManualT:
		case Foc::OpMode::kModeManualF:
		case Foc::OpMode::kModeManualTFFixedAngle:
			run_motor = true;
			break;
		default:
			return -1;
	}

	mMcUio.setGateDrive(run_motor);
	mFoc.setMode(target);

	return 0;
}

int MotorControlImpl::initMotor(bool full_init)
{
	// Make sure the motor is off and GateDrive is disabled
	transitionMode(Foc::OpMode::kModeStop);

	const ElectricalData all_Edata[] = {ElectricalData::kPhaseA,
					ElectricalData::kPhaseB,
					ElectricalData::kPhaseC,
					ElectricalData::kDCLink,
					};

	if(full_init) {
		/*
		 * Board & IP Configs
		 */
		mSvPwm.setSampleII(mInitConfig[kParamSvpSample_II]);
		mSvPwm.setDcLink(mInitConfig[kParamSvpVoltage]);
		mSvPwm.setMode(mInitConfig[kParamSvpMode]);

		mPwm.setFrequency(mInitConfig[kParamPwmFreq]);
		mPwm.setDeadCycle(mInitConfig[kParamPwmDeadCycle]);
		mPwm.setPhaseShift(mInitConfig[kParamPwmPhaseShift]);
		mPwm.setSampleII(mInitConfig[kParamPwmSample_II]);

		/*
		 * set scaling for Voltage and Current
		 */
		for (auto phase : all_Edata) {
			mAdcHub.setVoltageScale(phase, mInitConfig[kParamAdchubVolScale]);
			mAdcHub.setCurrentScale(phase, mInitConfig[kParamAdchubStatorCurScale]);
		}
		mAdcHub.setCurrentScale(ElectricalData::kDCLink, mInitConfig[kParamAdchubDcCurScale]);

		/*
		 * Set Filter Taps
		 */
		for (auto phase : all_Edata) {
			mAdcHub.setCurrentFiltertap(phase, mInitConfig[kParamAdchubFiltertap]);
			mAdcHub.setVoltageFiltertap(phase, mInitConfig[kParamAdchubFiltertap]);
		}
		mAdcHub.setCurrentFiltertap(ElectricalData::kDCLink, mInitConfig[kParamDclinkFiltertap]);
		mAdcHub.setVoltageFiltertap(ElectricalData::kDCLink, mInitConfig[kParamDclinkFiltertap]);

		/*
		 * Set the thresholds for all the events
		 */
		mEvents.setUpperThreshold(FaultId::kPhaseA_OC, mInitConfig[kParamCurPhaseThresHigh]);
		mEvents.setUpperThreshold(FaultId::kPhaseB_OC, mInitConfig[kParamCurPhaseThresHigh]);
		mEvents.setUpperThreshold(FaultId::kPhaseC_OC, mInitConfig[kParamCurPhaseThresHigh]);
		mEvents.setLowerThreshold(FaultId::kPhaseA_OC, mInitConfig[kParamCurPhaseThresLow]);
		mEvents.setLowerThreshold(FaultId::kPhaseB_OC, mInitConfig[kParamCurPhaseThresLow]);
		mEvents.setLowerThreshold(FaultId::kPhaseC_OC, mInitConfig[kParamCurPhaseThresLow]);

		mEvents.setUpperThreshold(FaultId::kDCLink_OC, mInitConfig[kParamCurDclinkThresHigh]);
		mEvents.setLowerThreshold(FaultId::kDCLink_OC, mInitConfig[kParamCurDclinkThresLow]);

		mEvents.setUpperThreshold(FaultId::kDCLink_OV, mInitConfig[kParamVolPhaseThresHigh]);
		mEvents.setLowerThreshold(FaultId::kDCLink_UV, mInitConfig[kParamVolPhaseThresLow]);

		mEvents.setUpperThreshold(FaultId::kPhaseImbalance, mInitConfig[kParamImbalanceThresHigh]);
		mEvents.setUpperThreshold(FaultId::kAvgPowerFault, mInitConfig[kParamMaxRatedMotorPower]);


		/*
		 * Reset, clear and activate all the events
		 */
		mEvents.resetAllEvents();
		mEvents.activateAllEvents();

		/*
		 * Foc configuration
		 */
		mFoc.setGain(GainType::kTorque, mInitConfig[kParamTorKp], mInitConfig[kParamTorKi]);
		mFoc.setGain(GainType::kFlux, mInitConfig[kParamFluxKp], mInitConfig[kParamFluxKi]);
		mFoc.setGain(GainType::kSpeed, mInitConfig[kParamSpeedKp], mInitConfig[kParamSpeedKi]);
		mFoc.setGain(GainType::kFieldweakening, mInitConfig[kParamFwKp], mInitConfig[kParamFwKi]);

		mFoc.setTorque(mInitConfig[kParamTorSp]);
		mFoc.setSpeed(mInitConfig[kParamSpeedSp]);

		mVq = mInitConfig[kParamVfVq];
		mVd = mInitConfig[kParamVfVd];

		mFoc.setVfParam(mInitConfig[kParamVfVq], mInitConfig[kParamVfVd]);

		/*
		 * Do AP_Start for all the IPs
		 */
		mPwm.startPwm();
		mSvPwm.startSvpwm();
		mpSensor->start();
		mFoc.startFoc();

		/*
		* calibrating offsets for current channel
		* TODO: check if this can be moved before enabling the AP_start
		*/
		mAdcHub.calibrateCurrentChannel( ElectricalData::kPhaseA);
		mAdcHub.calibrateCurrentChannel( ElectricalData::kPhaseB);
		mAdcHub.calibrateCurrentChannel(ElectricalData::kPhaseC);

		/*
		* KD240 hardware has non-linearity at value < 500mA, thus not including in
		* DC offset calc.
		*/
		//mAdcHub.calibrateCurrentChannel(ElectricalData::kDCLink);
		usleep(mInitConfig[kParamCalibrationWaitUs]);	//wait for the calibration and event recording

		if(getFaultStatus(FaultId::kDCLink_UV)) {
			std::cerr << "Failed to detect DCLink voltage. Make sure Motor is powered up"
				  << std::endl << "ERROR: Initialization Failed" << std::endl;
			return -1;
		}
	}

	/*
	 * Auto alignment
	 */
	transitionMode(Foc::OpMode::kModeManualTF);	//Setting OpenLoop Mode
	usleep(mInitConfig[kParamCalibrationWaitUs]);
	transitionMode(Foc::OpMode::kModeStop);

	mFoc.setAngleOffset(0);
	mFoc.setFixedAngleCmd(0);
	transitionMode(Foc::OpMode::kModeManualTFFixedAngle);
	usleep(mInitConfig[kParamCalibrationWaitUs]);
	int positionalCpr = ANGLE2CPR(mpSensor->getPosition());
	int cprAligned = ((positionalCpr - mInitConfig[kParamThetae90deg]) > 0) ? (positionalCpr
			- mInitConfig[kParamThetae90deg]) : (positionalCpr - mInitConfig[kParamThetae90deg] + mInitConfig[kParamCpr]);
	mFoc.setAngleOffset(cprAligned);

	transitionMode(Foc::OpMode::kModeStop);

	/*
	 * Check for any critical faults after the alignment
	 */
	FaultId criticalFaults[] = {
				    FaultId::kPhaseA_OC,
                                    FaultId::kPhaseB_OC,
                                    FaultId::kPhaseC_OC,
                                    FaultId::kDCLink_OC,
                                    FaultId::kDCLink_OV,
                                    FaultId::kDCLink_UV,
				};

	for (auto ev: criticalFaults) {
		if(getFaultStatus(ev)) {
			std::cerr << "ERROR: Critical Fault detected. Initialization Failed"
				  << std::endl;
			return -1;
		}
	}

	return 0;
}

map<MotorParam, vector<double>> MotorControlImpl::getMotorParams(int numSamples, vector<MotorParam> list)
{
	map<AdcChannels, vector<double>> hubMap;
	map<FocChannel, vector<double>> focMap;
	map<Qeichannel, vector<double>> qeiMap;
	map<MotorParam, vector<double>> MotorConcatMap;

	if (list.size() == 0) {
		cout << " No channels specified for data collection" << endl;
		return MotorConcatMap;
	}

	PlatformChannels parseData = MotorControlImpl::parseMotorParams(list);

	if (parseData.adcChannelList.size() != 0) {
		hubMap = mAdcHub.fillBuffer(numSamples, parseData.adcChannelList);
		for (auto &it : hubMap) {
			MotorParam NewKey = remapHub(it.first);
			MotorConcatMap[NewKey] = it.second;
		}
	}
	if (parseData.focChannelList.size() != 0) {
		focMap = mFoc.fillBuffer(numSamples, parseData.focChannelList);
		for (auto &it : focMap){
			MotorParam Newkey = remapFoc(it.first);
			MotorConcatMap[Newkey] = it.second;
		}
	}
	if (parseData.qeichannelList.size() != 0) {
		qeiMap = mpSensor->fillBuffer(numSamples, parseData.qeichannelList);
		for (auto &it : qeiMap){
			MotorParam Newkey = remapQei(it.first);
			MotorConcatMap[Newkey] = it.second;
		}
	}

	return MotorConcatMap;
}
