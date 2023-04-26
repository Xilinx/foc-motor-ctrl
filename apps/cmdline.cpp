/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include "motor-control/motor-control.hpp"

using namespace std;

int main() {
	MotorControl *ptr = MotorControl::getMotorControlInstance(1234);
	cout<<"Speed: " << ptr->getSpeed() <<endl;
	cout<<"Position: " << ptr->getPosition();
	delete ptr;
	return 0;
}
