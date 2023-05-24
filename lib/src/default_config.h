/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _DEFAULT_CONFIG_H_
#define _DEFAULT_CONFIG_H_

#define SVP_SAMPLE_II		 1
#define SVP_VOLTAGE			24
#define SVP_MODE			 0

#define PWM_FREQ			96800

#define SPEED_RRATE			16384000
#define TORQUE_RRATE		2894

#define SCALE_FACTOR		65536
#define TOR_KP				(151552.0/SCALE_FACTOR)
#define TOR_KI				(80.0/SCALE_FACTOR)
#define FLUX_KP				(123360.0/SCALE_FACTOR)
#define FLUX_KI				(40.0/SCALE_FACTOR)
#define SPEED_KP			(650.0/SCALE_FACTOR)
#define SPEED_KI			(5.0/SCALE_FACTOR)
#define FW_KP				(650.0/SCALE_FACTOR)
#define FW_KI				(5.0/SCALE_FACTOR)

#define TOR_SP				(28945.0/SCALE_FACTOR)
#define SPEED_SP			500

#define VF_VQ				(131072.0/SCALE_FACTOR)
#define VF_VD				(4294927975.0/SCALE_FACTOR)
#define VF_FIXED_SPEED		(3300.0/SCALE_FACTOR)

//TODO: Implement complete set of init values

#endif // _DEFAULT_CONFIG_H_
