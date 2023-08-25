/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include "motor-control/motor-control.hpp"
#include <unistd.h>

using namespace std;

int main() {
	MotorControl *ptr = MotorControl::getMotorControlInstance(1234);
	ptr->setOperationMode(MotorOpMode::kModeOpenLoop);
	sleep(3);
	cout<<"Speed: " << ptr->getSpeed() <<endl;
	cout<<"Position: " << ptr->getPosition();
	vector<MotorParam> list = {MotorParam::kVoltagePhaseA, MotorParam::kCurrentPhaseA, MotorParam::kVoltagePhaseB, MotorParam::kCurrentPhaseB, MotorParam::kVoltagePhaseC, MotorParam::kCurrentPhaseC, MotorParam::kId, MotorParam::kIq, MotorParam::kIalpha, MotorParam::kIbeta, MotorParam::kRpm, MotorParam::kPosition};
	map<MotorParam, vector<double>> data;

	/* Uncomment to test Buffer data collection
	for ( int i = 0; i < 10; i++ ) {
		data = ptr->getMotorParams(10, list);

		for ( auto it = data.begin(); it != data.end(); it++) {
				cout << "printing Buffer data" << endl;
				cout << "channel index is : " << (int)it->first << endl;
			for ( auto i : it->second) {
				cout << i << endl;
			}
		}

		data.clear();
	}
	*/

	ptr->setOperationMode(MotorOpMode::kModeOff);
	delete ptr;
	return 0;
}
