/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _DEFAULT_CONFIG_H_
#define _DEFAULT_CONFIG_H_

#define SVP_SAMPLE_II		1
#define SVP_VOLTAGE			24
#define SVP_MODE			0

#define PWM_FREQ			96800
#define PWM_DEAD_CYC		2
#define PWM_PHASE_SHIFT		0
#define PWM_SAMPLE_II		1

/*
 * TODO: revert when fixed
 * Due to bug in the hw, Speed & torque cannot be zero; as it results in fault.
 * use the working values are default reset values
 */
#define RST_SPEED		250
#define RST_TORQUE		0.44

#define SPEED_RRATE			250
#define TORQUE_RRATE		0.0441589355468

#define TOR_KP				2.3125
#define TOR_KI				0.001220703125
#define FLUX_KP				1.88232421875
#define FLUX_KI				0.0006103515625
#define SPEED_KP			0.0099182128906
#define SPEED_KI			0.0000762939453125
#define FW_KP				0.009918212890625
#define FW_KI				0.0000762939453125
#define CPR				1000

#define TOR_SP				0.4416656494140625
#define SPEED_SP			500

#define VF_VQ				4.0
#define VF_VD				0.0
#define VF_FIXED_SPEED		3300

#define ADCHUB_VOL_SCALE	0.018
#define ADCHUB_STATOR_CUR_SCALE	0.005
#define ADCHUB_DC_CUR_SCALE	0.010

#define ADCHUB_VOL_PHASE_FALLING_THRES	0.5
#define ADCHUB_VOL_PHASE_RISING_THRES	28
#define ADCHUB_CUR_PHASE_FALLING_THRES	-2.7
#define ADCHUB_CUR_PHASE_RISING_THRES	2.7
#define ADCHUB_DCLINK_FALLING_THRES		-0.625
#define ADCHUB_DCLINK_RISING_THRES		1.0

#define PHASE_IMBALANCE_RISING_THRES	1

#define ADCHUB_FILTERTAP	32
#define DCLINK_FILTERTAP	128

#define CALIBRATION_WAIT_US (500 * 1000) //500ms

#define THETAE90DEG     125
#endif // _DEFAULT_CONFIG_H_
