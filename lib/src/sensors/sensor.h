/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */


#include "qei.h"
#include "smo.h"

class Sensor {
public:
	virtual int getSpeed() = 0;
	virtual int getPosition() = 0;
	static Sensor *getSensorInstance() {
		(!isQeiPresent()) {
			return new SmoSensor(/* constructor parameters for FOC sensor */);
		}
		return new QeiSensor(/* constructor parameters for QEI sensor */);
	}

private:
	static bool isQeiPresent() {
		// TODO: find if the Qei is present
		return true;
	}
};
