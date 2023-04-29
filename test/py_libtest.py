#!/usr/bin/python3
#
# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT
#

import py_foc_motor_ctrl as mcontrol

# Get a MotorControl instance with session ID 1 and default config path
mc = mcontrol.MotorControl.getMotorControlInstance(1)

# Use the MotorControl instance to call its member functions

# Set speed. No actual effect, just print from the lib
mc.setSpeed(1000)

speed = mc.getSpeed()
print(speed)

print(mc.getPosition())

# get FocData structure
foc = mc.getFocCalc()

# print a member of FocData structure
print("Torque =", foc.torque)

# Set Gain. No actual effect, just print from the lib
mc.setGain(mcontrol.GainType.kFlux, 6,8)
