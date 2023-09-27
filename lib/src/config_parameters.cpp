/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "config_parameters.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <cassert>

/*
 * Define default values for all the parameters
 */

// Define the static arrays
const double MotorConfigParams::defaultValues[MConfigParams::kParamMax] = {
   	1,                  // kParamSvpSample_II
   	24,                 // kParamSvpVoltage
   	0,                  // kParamSvpMode
   	96800,              // kParamPwmFreq
   	2,                  // kParamPwmDeadCycle
   	0,                  // kParamPwmPhaseShift
   	1,                  // kParamPwmSample_II
   	250,                // kParamRstSpeed
   	0.3,                // kParamRstTorque
   	250,                // kParamSpeedRrate
   	0.05,               // kParamTorqueRrate
   	9.0,                // kParamTorKp
   	0.001,              // kParamTorKi
   	8.0,                // kParamFluxKp
   	0.001,              // kParamFluxKi
   	0.005,              // kParamSpeedKp
   	0.0001,             // kParamSpeedKi
   	0.009918212890625,  // kParamFwKp
   	0.0000762939453125, // kParamFwKi
   	1000,               // kParamCpr
   	0.3,                // kParamTorSp
   	1000,               // kParamSpeedSp
   	4.0,                // kParamVfVq
   	0.0,                // kParamVfVd
   	0.018,              // kParamAdchubVolScale
   	0.005,              // kParamAdchubStatorCurScale
   	0.010,              // kParamAdchubDcCurScale
   	-2.7,               // kParamCurPhaseThresLow
   	2.7,                // kParamCurPhaseThresHigh
   	-0.625,             // kParamCurDclinkThresLow
   	2.8,                // kParamCurDclinkThresHigh
   	21,                 // kParamVolPhaseThresLow
   	28,                 // kParamVolPhaseThresHigh
   	0.5,                // kParamImbalanceThresHigh
   	15.0,               // kParamMaxRatedMotorPower
   	32,                 // kParamAdchubFiltertap
   	128,                // kParamDclinkFiltertap
   	500000,             // kParamCalibrationWaitUs
   	125                 // kParamThetae90deg
};

const char* MotorConfigParams::paramNames[MConfigParams::kParamMax] = {
   	"SVP_SAMPLE_II",            // kParamSvpSample_II 
   	"SVP_VOLTAGE",              // kParamSvpVoltage
   	"SVP_MODE",                 // kParamSvpMode
   	"PWM_FREQ",                 // kParamPwmFreq
   	"PWM_DEAD_CYC",             // kParamPwmDeadCycle
   	"PWM_PHASE_SHIFT",          // kParamPwmPhaseShift
   	"PWM_SAMPLE_II",            // kParamPwmSample_II
   	"RST_SPEED",                // kParamRstSpeed
   	"RST_TORQUE",               // kParamRstTorque
   	"SPEED_RRATE",              // kParamSpeedRrate
   	"TORQUE_RRATE",             // kParamTorqueRrate
   	"TOR_KP",                   // kParamTorKp
   	"TOR_KI",                   // kParamTorKi
   	"FLUX_KP",                  // kParamFluxKp
   	"FLUX_KI",                  // kParamFluxKi
   	"SPEED_KP",                 // kParamSpeedKp
   	"SPEED_KI",                 // kParamSpeedKi
   	"FW_KP",                    // kParamFwKp
   	"FW_KI",                    // kParamFwKi
   	"CPR",                      // kParamCpr
   	"TOR_SP",                   // kParamTorSp
   	"SPEED_SP",                 // kParamSpeedSp
   	"VF_VQ",                    // kParamVfVq
   	"VF_VD",                    // kParamVfVd
   	"ADCHUB_VOL_SCALE",         // kParamAdchubVolScale
   	"ADCHUB_STATOR_CUR_SCALE",  // kParamAdchubStatorCurScale  
   	"ADCHUB_DC_CUR_SCALE",      // kParamAdchubDcCurScale
   	"CUR_PHASE_THRES_LOW",      // kParamCurPhaseThresLow
   	"CUR_PHASE_THRES_HIGH",     // kParamCurPhaseThresHigh
   	"CUR_DCLINK_THRES_LOW",     // kParamCurDclinkThresLow
   	"CUR_DCLINK_THRES_HIGH",    // kParamCurDclinkThresHigh
   	"VOL_PHASE_THRES_LOW",      // kParamVolPhaseThresLow
   	"VOL_PHASE_THRES_HIGH",     // kParamVolPhaseThresHigh
   	"IMBALANCE_THRES_HIGH",     // kParamImbalanceThresHigh
   	"MAX_RATED_MOTOR_POWER",    // kParamMaxRatedMotorPower
   	"ADCHUB_FILTERTAP",         // kParamAdchubFiltertap
   	"DCLINK_FILTERTAP",         // kParamDclinkFiltertap
   	"CALIBRATION_WAIT_US",      // kParamCalibrationWaitUs
   	"THETAE90DEG"               // kParamThetae90deg
};


MotorConfigParams::MotorConfigParams()
{
	// Initialize the array with default values
	for (int i = 0; i < MConfigParams::kParamMax; ++i) {
		configData[i] = defaultValues[i];
	}
}

double MotorConfigParams::operator[](MConfigParams paramId) const
{
	// Assert / Error out if paramId is < 0 or >= ParamMax
  	assert(paramId >= 0 && paramId < MConfigParams::kParamMax);
	return configData[paramId];
}

void MotorConfigParams::overrideConfigs(const std::string& configFileName)
{
	std::ifstream configFile(configFileName);
	if (configFile.is_open()) {
		std::string line;
		while (std::getline(configFile, line)) {
			// Skip lines starting with '#'
			if (line.empty() || line[0] == '#') {
				continue;
			}
      			// Skip whitespaces in the lines
      			std::size_t  nonSpace_pos = line.find_first_not_of(" \t");
      			if ((nonSpace_pos == std::string::npos) || line[nonSpace_pos] == '#') {
            			// The line contains only white spaces or starting with # after white spaces
            			continue;
      			}	
      			line = line.substr(nonSpace_pos); 
      			// Find position of '=' character
			std::size_t pos = line.find('=');
			if (pos != std::string::npos) {
				std::string paramName = line.substr(0, pos);
				std::string paramValueStr = line.substr(pos + 1);
				double paramValue = std::stod(paramValueStr);
				for (int i = 0; i < static_cast<int>(MConfigParams::kParamMax); ++i) {
					if (strcmp(paramNames[i], paramName.c_str()) == 0) {
						configData[i] = paramValue;
						break;
					}
				}
			}
		}
		configFile.close();
	}
}
