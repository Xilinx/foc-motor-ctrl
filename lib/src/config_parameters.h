/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _COFNIG_PARAMETERS_H_
#define _COFNIG_PARAMETERS_H_

#include <map>
#include <string>

enum MConfigParams {
	kParamSvpSample_II,
	kParamSvpVoltage,
	kParamSvpMode,
	kParamPwmFreq,
	kParamPwmDeadCycle,
	kParamPwmPhaseShift,
	kParamPwmSample_II,
	kParamRstSpeed,
 	kParamRstTorque,
  	kParamSpeedRrate,
  	kParamTorqueRrate,
  	kParamTorKp,
  	kParamTorKi,
  	kParamFluxKp,
  	kParamFluxKi,
  	kParamSpeedKp,
  	kParamSpeedKi,
  	kParamFwKp,
  	kParamFwKi,
  	kParamCpr,
  	kParamTorSp,
  	kParamSpeedSp,
  	kParamVfVq,
  	kParamVfVd,
  	kParamAdchubVolScale,
  	kParamAdchubStatorCurScale,
  	kParamAdchubDcCurScale,
  	kParamCurPhaseThresLow,
  	kParamCurPhaseThresHigh,
  	kParamCurDclinkThresLow,
  	kParamCurDclinkThresHigh,
  	kParamVolPhaseThresLow,
  	kParamVolPhaseThresHigh,
  	kParamImbalanceThresHigh,
  	kParamMaxRatedMotorPower,
  	kParamAdchubFiltertap,
  	kParamDclinkFiltertap,
  	kParamCalibrationWaitUs,
  	kParamThetae90deg,
	kParamMax // A special value to represent the number of parameters
};

class MotorConfigParams {
public:
	MotorConfigParams();

	// Read-only access to parameter values using the [] operator with enums
	double operator[](MConfigParams paramId) const;

	// Override the values from a configuration file
	void overrideConfigs(const std::string& configFileName);

private:
	static const char* paramNames[MConfigParams::kParamMax];
	static const double defaultValues[MConfigParams::kParamMax];
	double configData[MConfigParams::kParamMax];
};

#endif // _COFNIG_PARAMETERS_H_
