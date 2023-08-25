/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#include <unistd.h>
#include <chrono>
#include <iostream>
#include "foc.h"

/*
 * TODO: revert inclusion of default_config.h
 * This temporary, ramp rates and default speed to come through constr or api
 */
#include "default_config.h"

#define RAMP_INTERVAL_MS	500

const std::string Foc::kFocDriverName = "hls_foc_periodic";

Foc::Foc():
	mTargetSpeed(RST_SPEED),
	mSpeedRRate(SPEED_RRATE),
	mDoSpeedRamp(false),
	mTargetTorque(RST_TORQUE),
	mTorRRate(TORQUE_RRATE),
	mDoTorRamp(false)
{
	mFoc_IIO_Handle = new IIO_Driver(kFocDriverName);
	// sample buffer data at 100us intervals
	mFoc_IIO_Handle->writeDeviceattr("sample_interval_us", "100");
}

Foc::~Foc()
{
	delete mFoc_IIO_Handle;
}

int Foc::setSpeed(double speedSp)
{
	std::lock_guard<std::mutex> lock(mSpeedMutex);
	mTargetSpeed = speedSp;

	if(mFoc_IIO_Handle->readDeviceattr("control_mode") != 0) {
		// if not ramping already
		if (!mDoSpeedRamp) {
			mDoSpeedRamp = true;
			mSpeedThread = std::thread(&Foc::rampSpeed, this);
		}
	}
	return 0;
}

int Foc::setTorque(double torqueSp)
{
	std::lock_guard<std::mutex> lock(mTorMutex);
	mTargetTorque = torqueSp;

	if(mFoc_IIO_Handle->readDeviceattr("control_mode") != 0) {
		// if not ramping already
		if (!mDoTorRamp) {
			mDoTorRamp = true;
			mTorThread = std::thread(&Foc::rampTorque, this);
		}
	}
	return 0;

}

int Foc::setGain(GainType gainController, double kp, double ki)
{
	switch (gainController)
	{
	case GainType::kTorque:
		mFoc_IIO_Handle->writeDeviceattr("torque_kp", std::to_string(kp).c_str());
		mFoc_IIO_Handle->writeDeviceattr("torque_ki", std::to_string(ki).c_str());
		break;
	case GainType::kSpeed:
		mFoc_IIO_Handle->writeDeviceattr("speed_kp", std::to_string(kp).c_str());
		mFoc_IIO_Handle->writeDeviceattr("speed_ki", std::to_string(ki).c_str());
		break;
	case GainType::kFlux:
		mFoc_IIO_Handle->writeDeviceattr("flux_kp", std::to_string(kp).c_str());
		mFoc_IIO_Handle->writeDeviceattr("flux_ki", std::to_string(ki).c_str());
		break;
	case GainType::kFieldweakening:
		mFoc_IIO_Handle->writeDeviceattr("fw_kp", std::to_string(kp).c_str());
		mFoc_IIO_Handle->writeDeviceattr("fw_ki", std::to_string(ki).c_str());
		break;
	default:
		return -1;
	}

	return 0;
}

GainData Foc::getGain(GainType gainController)
{
	GainData gaindata = {0,0};

	switch (gainController)
	{
	case GainType::kTorque:
		gaindata.kp = mFoc_IIO_Handle->readDeviceattr("torque_kp");
		gaindata.ki = mFoc_IIO_Handle->readDeviceattr("torque_ki");
		break;
	case GainType::kSpeed:
		gaindata.kp = mFoc_IIO_Handle->readDeviceattr("speed_kp");
		gaindata.ki = mFoc_IIO_Handle->readDeviceattr("speed_ki");
		break;
	case GainType::kFlux:
		gaindata.kp = mFoc_IIO_Handle->readDeviceattr("flux_kp");
		gaindata.ki = mFoc_IIO_Handle->readDeviceattr("flux_ki");
		break;
	case GainType::kFieldweakening:
		gaindata.kp = mFoc_IIO_Handle->readDeviceattr("fw_kp");
		gaindata.ki = mFoc_IIO_Handle->readDeviceattr("fw_ki");
		break;
	default:
		return gaindata;
	}
	return gaindata;
}

int Foc::startFoc()
{
	return mFoc_IIO_Handle->writeDeviceattr("ap_ctrl", "1");
}

int Foc::setAngleOffset(int angleSh)
{
	return mFoc_IIO_Handle->writeDeviceattr("angle_sh", std::to_string(angleSh).c_str());
}

int Foc::setFixedAngleCmd(int angleCmd)
{
	return mFoc_IIO_Handle->writeDeviceattr("fixed_angle_cpr", std::to_string(angleCmd).c_str());
}

int Foc::setVfParam(double vq, double vd)
{
	mFoc_IIO_Handle->writeDeviceattr("vq", std::to_string(vq).c_str());
	mFoc_IIO_Handle->writeDeviceattr("vd", std::to_string(vd).c_str());
	return 0;
}

double Foc::getTorqueSetValue()
{
	return mTargetTorque;
}

double Foc::getSpeedSetValue()
{
	return mTargetSpeed;
}

int Foc::setMode(OpMode mode)
{
	std::lock_guard<std::mutex> Slock(mSpeedMutex);
	std::lock_guard<std::mutex> Tlock(mTorMutex);

	if (mode != OpMode::kModeSpeed) {
		// not speed mode
		mDoSpeedRamp = false;
		if (mSpeedThread.joinable()) {
			mSpeedThread.join();
		}
	}

	if (mode != OpMode::kModeTorque) {
		// not torque mode
		mDoTorRamp = false;
		if (mTorThread.joinable()) {
			mTorThread.join();
		}
	}

	mFoc_IIO_Handle->writeDeviceattr("control_mode", std::to_string(static_cast<int>(mode)).c_str());

	switch(mode) {
	case OpMode::kModeStop:
		// Reset the SP values to default reset
		mFoc_IIO_Handle->writeDeviceattr("speed_sp", std::to_string(RST_SPEED).c_str());
		mFoc_IIO_Handle->writeDeviceattr("torque_sp", std::to_string(RST_TORQUE).c_str());
		mFoc_IIO_Handle->writeDeviceattr("flux_sp", "0");
		break;
	case OpMode::kModeSpeed:
		// start ramping
		if (!mDoSpeedRamp) {
			mDoSpeedRamp = true;
			mSpeedThread = std::thread(&Foc::rampSpeed, this);
		}
		break;
	case OpMode::kModeTorque:
		// start ramping
		if (!mDoTorRamp) {
			mDoTorRamp = true;
			mTorThread = std::thread(&Foc::rampTorque, this);
		}
		break;
	default:
		break;
	}
	return 0;
}

FocData Foc::getChanData()
{
	FocData data;
	data.i_d = mFoc_IIO_Handle->readChannel(Id, "raw");
	data.i_q = mFoc_IIO_Handle->readChannel(Iq, "raw");
	data.i_alpha = mFoc_IIO_Handle->readChannel(I_alpha, "raw");
	data.i_beta = mFoc_IIO_Handle->readChannel(I_beta, "raw");
	data.i_homopolar = mFoc_IIO_Handle->readChannel(I_homopolar, "raw");
	data.speed = mFoc_IIO_Handle->readChannel(speed_pi_out, "raw");
	data.torque = mFoc_IIO_Handle->readChannel(torque_pi_out, "raw");
	data.flux = mFoc_IIO_Handle->readChannel(flux, "raw");
	return data;
}

void Foc::rampSpeed(void)
{

	while(mDoSpeedRamp) {
		int currentTarget;
		int currentSpeed;
		{
			std::lock_guard<std::mutex> lock(mSpeedMutex);
			currentSpeed = mFoc_IIO_Handle->readDeviceattr("speed_sp");
			currentTarget= mTargetSpeed;
#if 0
			if (currentSpeed == currentTarget) {
				mDoSpeedRamp = false;
				//todo: join / kill if possible
				break;
			}
#endif
		}

		int newSpeed = currentTarget;

		if (currentSpeed < currentTarget) {	 //Ramp Up
			newSpeed = currentSpeed + mSpeedRRate;
			if (newSpeed > currentTarget)
				newSpeed = currentTarget;
		}
		else if (currentSpeed > currentTarget) {
			newSpeed = currentSpeed - mSpeedRRate;
			if (newSpeed < currentTarget)
				newSpeed = currentTarget;
		}

		mFoc_IIO_Handle->writeDeviceattr("speed_sp", std::to_string(newSpeed).c_str());

		std::this_thread::sleep_for(std::chrono::milliseconds(RAMP_INTERVAL_MS));
	}
}

std::map<FocChannel, std::vector<double>> Foc::fillBuffer(int samples, std::vector<FocChannel> channels)
{
	std::map<FocChannel, std::vector<double>> focData;
	std::vector<int> focVector;

	for (auto it : channels)
	{
		focVector.push_back(static_cast<int>(it));
	}

	std::map<int, std::vector<double>> rawDataMap = mFoc_IIO_Handle->getBufferdata(samples, focVector);
	if (rawDataMap.size() == 0 && focVector.size() != 0)
	{
		std::cout << " No data samples obtained from FOC " << std::endl;
		return focData;
	}

	for (auto &it : rawDataMap)
	{
		FocChannel newKey = static_cast<FocChannel>(it.first);
		focData[newKey] = it.second;
	}

	return focData;
}

void Foc::rampTorque(void)
{
	while(mDoTorRamp) {
		double currentTarget;
		double currentTorque;
		{
			std::lock_guard<std::mutex> lock(mTorMutex);
			currentTorque = mFoc_IIO_Handle->readDeviceattr("torque_sp");
			currentTarget= mTargetTorque;
#if 0
			if (currentTorque == currentTarget) {
				mDoTorRamp = false;
				//todo: join / kill if possible
				break;
			}
#endif
		}

		double newTor = currentTarget;

		if (currentTorque < currentTarget) {	 //Ramp Up
			newTor = currentTorque + mTorRRate;
			if (newTor > currentTarget)
				newTor = currentTarget;
		}
		else if (currentTorque > currentTarget) {
			newTor = currentTorque - mTorRRate;
			if (newTor < currentTarget)
				newTor = currentTarget;
		}

		mFoc_IIO_Handle->writeDeviceattr("torque_sp", std::to_string(newTor).c_str());

		std::this_thread::sleep_for(std::chrono::milliseconds(RAMP_INTERVAL_MS));
	}
}
