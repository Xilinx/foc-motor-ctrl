/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#include "pwm.h"

const std::string Pwm::kPwmDriverName = "hls_pwm_gen";

Pwm::Pwm()
{
	m_Pwm_IIO_Handle = new IIO_Driver(kPwmDriverName);
}

int Pwm::setFrequency(int frequency)
{
	return m_Pwm_IIO_Handle->writeDeviceattr("pwm_freq", std::to_string(frequency).c_str());
}

int Pwm::startPwm()
{
	return m_Pwm_IIO_Handle->writeDeviceattr("ap_ctrl", "1");
}

int Pwm::setDeadCycle(int deadCycle)
{
	return m_Pwm_IIO_Handle->writeDeviceattr("dead_cycles", std::to_string(deadCycle).c_str());
}

int Pwm::setPhaseShift(int phaseShift)
{
	return m_Pwm_IIO_Handle->writeDeviceattr("phase_shift", std::to_string(phaseShift).c_str());
}

int Pwm::setSampleII(int sample)
{
	return m_Pwm_IIO_Handle->writeDeviceattr("sample_ii", std::to_string(sample).c_str());
}

Pwm::~Pwm()
{
	delete m_Pwm_IIO_Handle;
}
