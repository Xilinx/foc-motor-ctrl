/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "svpwm.h"
#define SCALE 65536

const std::string Svpwm::kSvpwmDriverName = "hls_svpwm_duty";

Svpwm::Svpwm()
{
	m_Svpwm_IIO_Handle = new IIO_Driver(kSvpwmDriverName);
}

int Svpwm::setSampleII(int sample)
{
	return m_Svpwm_IIO_Handle->writeDeviceattr("sample_ii", std::to_string(sample).c_str());
}

int Svpwm::setDcLink(int volt)
{
	volt *= SCALE;
	return m_Svpwm_IIO_Handle->writeDeviceattr("dc_link_ref_voltage", std::to_string(volt).c_str());
}

int Svpwm::setMode(int mode)
{
	return m_Svpwm_IIO_Handle->writeDeviceattr("dc_src_mode", std::to_string(mode).c_str());
}

int Svpwm::startSvpwm()
{
	return m_Svpwm_IIO_Handle->writeDeviceattr("ap_ctrl", "1");
}

Svpwm::~Svpwm()
{
	delete m_Svpwm_IIO_Handle;
}
