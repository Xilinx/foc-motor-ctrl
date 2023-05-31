/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

/*
 *   TODO List:
 *   - Add full description of APIs and structures (comments for detail usage)
 *   - Use smart pointer if it makes more sense
 *   - Check if absolute type is needed (like uint32_t ).
 *   - Check if FocData structure has everything needed (missing i_c?)
 *   - Need to check if it would be good to return error values for set APIs
 *   - Error handling in seperate class and include Here.
 *   - All the set API should return appropriate error codes.
 */

#include <string>

#define DEFAULT_CONFIG_PATH	"/etc/motor-control/config"

enum class MotorOpMode {
	kModeOff = 0,
	kModeSpeed,
	kModeTorque,
	kModeSpeedFW,
	kModeOpenLoop,
	kModePosControl,
	kModeMax
};

/* FOC control data */

struct FocData
{
	double i_d;
	double i_q;
	double i_alpha;
	double i_beta;
	double i_homopolar;
	double speed;
	double torque;
	double flux;
};

struct GainData
{
	double kp;
	double ki;
};

enum class GainType
{
	kTorque = 0,
	kSpeed,
	kFlux,
	kFieldweakening,
	kGainTypeMax
};

enum class ElectricalData {
	kPhaseA = 0,
	kPhaseB,
	kPhaseC,
	kDCLink,
	kElectricalDataMax
};

/*
 * FaultTypes denoted as
 * OC = Over Current
 * OV = Over Voltage
 * UV = Under Voltage
 */
enum class FaultType {
	kPhaseA_OC = 0,
	kPhaseB_OC,
	kPhaseC_OC,
	kDCLink_OC,
	kDCLink_OV,
	kDCLink_UV,
	kFalutTypeMax
};

enum class FaultCategory {
	// TODO: Add 2 classes of the fault
	kFaultCategoryMax
};

class MotorControl {
public:
	/*
	 * Fetch API to get various motor
	 * parameters
	 */
	virtual int getSpeed() = 0;				//Get RPM
	virtual int getPosition() = 0;				//Get Theta
	virtual int getTorque() = 0;				// Future Implementation
	virtual double getTorqueSetpoint() = 0;
	virtual int getSpeedSetpoint() = 0;
	virtual double getCurrent(ElectricalData type) = 0;
	virtual double getVoltage(ElectricalData type) = 0;
	virtual bool getFaultStatus(FaultType type) = 0;
	virtual FocData getFocCalc() = 0;
	virtual MotorOpMode getOperationMode() = 0;
	virtual GainData GetGain(GainType gainController) = 0;

	virtual void SetSpeed(double speed) = 0;
	virtual void SetTorque(double torque) = 0;
	virtual void SetPosition(int position) = 0;
	virtual void SetGain(GainType gainController, GainData value) = 0;
	virtual void setOperationMode(MotorOpMode mode) = 0;

	virtual void clearFaults() = 0;
	virtual void clearFaults(FaultCategory category) = 0;

	virtual ~MotorControl();
	/*
	 * Static fucntion to get the singleton MotorControl Instance
	 */
	static MotorControl* getMotorControlInstance(int sessionId,
						     std::string configPath = DEFAULT_CONFIG_PATH);
private:
	/*
	 * Simulate and hold a singleton instance
	 */
	static MotorControl *mspInstance;
};

#endif // _MOTOR_CONTROL_H_
