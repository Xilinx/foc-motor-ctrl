/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _FOC_HPP_
#define _FOC_HPP_

#include <thread>
#include <mutex>

#include "motor-control/motor-control.hpp"
#include "interface/iio_drv.h"

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
	int setFixedSpeed(int fixedSpeed);
	int setVfParam(double vq, double vd, int fixedSpeed);
	double getTorqueSetValue();
	int setMode(OpMode mode);
	double getSpeedSetValue();
	FocData getChanData();
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

};

#endif // _FOC_HPP_
