/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _FOC_HPP_
#define _FOC_HPP_

#include <thread>
#include <mutex>

#include "interface/iio_drv.h"
#include "motor-control/motor-control.hpp"

// TODO: move FocChannel enum within the class namespace
enum FocChannel
{
	Id = 0,
	Iq,
	I_alpha,
	I_beta,
	I_homopolar,
	speed_pi_out,
	torque_pi_out,
	flux,
	rpm,
	position
};

class Foc
{
public:
	/*
	 * OperationModes in Foc namespace
	 */
	enum class OpMode {
		// usr modes
		kModeStop = 0,
		kModeSpeed,
		kModeTorque,
		kModeFlux,
		// expert modes
		kModeManualTFFixedSpeed,
		kModeManualTF,
		kModeManualT,
		kModeManualF,
		kModeManualTFFixedAngle,

		kModeMax
	};

	/*
	 * Public Interface for FOC
	 */

	Foc();
	int setSpeed(double speedSp);
	int setTorque(double torqueSp);
	int setGain(GainType gainController, double kp, double ki);
	GainData getGain(GainType gainController);
	int startFoc();
	int setAngleOffset(int angleSh);
	int setFixedAngleCmd(int angleCmd);
	int setVfParam(double vq, double vd);
	double getTorqueSetValue();
	int setMode(OpMode mode);
	double getSpeedSetValue();
	FocData getChanData();
	std::map<FocChannel, std::vector<double>> fillBuffer(int samples, std::vector<FocChannel> channels);
  	void updateConfig(double paramSpeedRrate, double paramTorqueRrate, double paramRstSpeed=0, double paramRstTorque=0);
  
	~Foc();

private:
	IIO_Driver *mFoc_IIO_Handle;
	static const std::string kFocDriverName;

	double mTargetSpeed;
	int mSpeedRRate;
	bool mDoSpeedRamp;
	std::thread mSpeedThread;
	std::mutex mSpeedMutex;
	void rampSpeed(void);

	double mTargetTorque;
	double mTorRRate;
	bool mDoTorRamp;
	std::thread mTorThread;
	std::mutex mTorMutex;
	void rampTorque(void);
	double mRstSpeed;
	double mRstTorque;
};

#endif // _FOC_HPP_
