/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _DEFAULT_CONFIG_H_
#define _DEFAULT_CONFIG_H_

#define SVP_SAMPLE_II			1
#define SVP_VOLTAGE			24
#define SVP_MODE			0

#define PWM_FREQ			96800
#define PWM_DEAD_CYC			2
#define PWM_PHASE_SHIFT			0
#define PWM_SAMPLE_II			1

/*
 * TODO: revert when fixed
 * Due to bug in the hw, Speed & torque cannot be zero; as it results in fault.
 * use the working values are default reset values
 */
#define RST_SPEED			250			// Reset speed setpoint when motor is Off
#define RST_TORQUE			0.3		// Reset Torque setpoint when motor is Off

#define SPEED_RRATE			250			// Speed change steps
#define TORQUE_RRATE			0.05		// Torque change steps

// KP(proportional) and KI(integral) Gain parameters
#define TOR_KP				9.0
#define TOR_KI				0.01
#define FLUX_KP				8.0
#define FLUX_KI				0.01
#define SPEED_KP			0.005
#define SPEED_KI			0.0001
#define FW_KP				0.009918212890625
#define FW_KI				0.0000762939453125

#define CPR				1000			// Counts per Revolution for auto alignment

#define TOR_SP				0.9	// Initial Torque setpoint value
#define SPEED_SP			1000			// Initial Speed setpoint value.

#define VF_VQ				4.0			// Initial manual openloop torque
#define VF_VD				0.0			// Initial manual openloop flux

//ADCHUB current & voltage scale values
#define ADCHUB_VOL_SCALE		0.018
#define ADCHUB_STATOR_CUR_SCALE		0.005
#define ADCHUB_DC_CUR_SCALE		0.010

//Thresholds for fault triggering
#define CUR_PHASE_THRES_LOW		-2.7			// Phase A, B, C current lower threshold (Amp)
#define CUR_PHASE_THRES_HIGH		2.7			// Phase A, B, C current upper threshold (Amp)
#define CUR_DCLINK_THRES_LOW		-0.625			// DCLink current lower threshold (Amp)
#define CUR_DCLINK_THRES_HIGH		2.8         // DCLink current upper threshold (Amp)
#define VOL_PHASE_THRES_LOW		21			// DCLink voltage lower threshold (V)
#define VOL_PHASE_THRES_HIGH		28			// DCLink voltage upper threshold (V)
#define IMBALANCE_THRES_HIGH		0.5			// Phase Imbalance upper threshold (Amp)

//Filter Tap values
#define ADCHUB_FILTERTAP		32
#define DCLINK_FILTERTAP		128

//Delay to be used during calibration and alignment between start and stop of motor
#define CALIBRATION_WAIT_US		(500 * 1000)		//500ms

#define THETAE90DEG			125			//Fixed Angle for alignment
#endif // _DEFAULT_CONFIG_H_
