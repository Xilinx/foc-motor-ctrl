/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "qei.h"
#include "smo.h"

Sensor *Sensor::getSensorInstance()
{
	if (!isQeiPresent()) {
		return new SmoSensor(/* constructor parameters for FOC sensor */);
	}
	return new QeiSensor(/* constructor parameters for QEI sensor */);
}

Sensor::~Sensor() {}

bool Sensor::isQeiPresent()
{
	//TODO: Find if the Qei is present
	return true;
}
