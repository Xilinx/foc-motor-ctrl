/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#include "foc.h"
#define SCALE 65536

const std::string Foc::kFocDriverName = "hls_foc_periodic";
enum FocChannel
{
	Id = 0,
	Iq,
	I_alpha,
	I_beta,
	I_homopolar,
	speed_pi_out,
	torque_pi_out,
	flux
};

Foc::Foc(/* args */)
{
	mFoc_IIO_Handle = new IIO_Driver(kFocDriverName);
}

Foc::~Foc()
{
	delete mFoc_IIO_Handle;
}

int Foc::setSpeed(int speedSp)
{
	speedSp *= SCALE;
	/* To do :
	 * Add ramp logic here
	 */
	return mFoc_IIO_Handle->writeDeviceattr("speed_sp", std::to_string(speedSp).c_str());
}

int Foc::setTorque(double torqueSp)
{
	int itorqueSp = torqueSp * SCALE;
	/* To do :
	 * Add ramp logic here
	 */
	return mFoc_IIO_Handle->writeDeviceattr("torque_sp", std::to_string(itorqueSp).c_str());
}

int Foc::setGain(GainType gainController, double kp, double ki)
{
	int ikp = kp * SCALE;
	int iki = ki * SCALE;
	switch (gainController)
	{
	case GainType::kTorque:
		mFoc_IIO_Handle->writeDeviceattr("torque_kp", std::to_string(ikp).c_str());
		mFoc_IIO_Handle->writeDeviceattr("torque_ki", std::to_string(iki).c_str());
		break;
	case GainType::kSpeed:
		mFoc_IIO_Handle->writeDeviceattr("speed_kp", std::to_string(ikp).c_str());
		mFoc_IIO_Handle->writeDeviceattr("speed_ki", std::to_string(iki).c_str());
		break;
	case GainType::kFlux:
		mFoc_IIO_Handle->writeDeviceattr("flux_kp", std::to_string(ikp).c_str());
		mFoc_IIO_Handle->writeDeviceattr("flux_ki", std::to_string(iki).c_str());
		break;
	case GainType::kFieldweakening:
		mFoc_IIO_Handle->writeDeviceattr("fw_kp", std::to_string(ikp).c_str());
		mFoc_IIO_Handle->writeDeviceattr("fw_ki", std::to_string(iki).c_str());
		break;
	default:
		return -1;
	}

	return 0;
}

GainData Foc::getGain(GainType gainController)
{
	GainData gaindata = {0,0};

	switch (gainController)
	{
	case GainType::kTorque:
		gaindata.kp = mFoc_IIO_Handle->readDeviceattr("torque_kp")/SCALE;
		gaindata.ki = mFoc_IIO_Handle->readDeviceattr("torque_ki")/SCALE;
		break;
	case GainType::kSpeed:
		gaindata.kp = mFoc_IIO_Handle->readDeviceattr("speed_kp")/SCALE;
		gaindata.ki = mFoc_IIO_Handle->readDeviceattr("speed_ki")/SCALE;
		break;
	case GainType::kFlux:
		gaindata.kp = mFoc_IIO_Handle->readDeviceattr("flux_kp")/SCALE;
		gaindata.ki = mFoc_IIO_Handle->readDeviceattr("flux_ki")/SCALE;
		break;
	case GainType::kFieldweakening:
		gaindata.kp = mFoc_IIO_Handle->readDeviceattr("fw_kp")/SCALE;
		gaindata.ki = mFoc_IIO_Handle->readDeviceattr("fw_ki")/SCALE;
		break;
	default:
		return gaindata;
	}
	return gaindata;
}

int Foc::startFoc()
{
	return mFoc_IIO_Handle->writeDeviceattr("ap_ctrl", "1");
}

int Foc::setAngleOffset(int angleSh)
{
	return mFoc_IIO_Handle->writeDeviceattr("angle_sh", std::to_string(angleSh).c_str());
}

int Foc::setFixedSpeed(int fixedSpeed)
{
	fixedSpeed *= SCALE;
	return mFoc_IIO_Handle->writeDeviceattr("fixed_period_ctrl", std::to_string(fixedSpeed).c_str());
}

int Foc::setVfParam(double vq, double vd, int fixedSpeed)
{
	int ivq = vq * SCALE;
	int ivd = vd * SCALE;
	fixedSpeed *= SCALE;
	mFoc_IIO_Handle->writeDeviceattr("vq", std::to_string(ivq).c_str());
	mFoc_IIO_Handle->writeDeviceattr("vd", std::to_string(ivd).c_str());
	mFoc_IIO_Handle->writeDeviceattr("fixed_period_ctrl", std::to_string(fixedSpeed).c_str());
	return 0;
}
int Foc::stopMotor()
{
	mFoc_IIO_Handle->writeDeviceattr("control_mode", "0");
	mFoc_IIO_Handle->writeDeviceattr("speed_sp", "0");
	mFoc_IIO_Handle->writeDeviceattr("torque_sp", "0");
	mFoc_IIO_Handle->writeDeviceattr("flux_sp", "0");
	return 0;
}

double Foc::getTorque()
{
	double ret;
	ret = mFoc_IIO_Handle->readDeviceattr("torque_sp");
	return ret / SCALE;
}

int Foc::setOperationMode(MotorOpMode mode)
{
	return mFoc_IIO_Handle->writeDeviceattr("control_mode", std::to_string(static_cast<int>(mode)).c_str());
}

FocData Foc::getChanData()
{
	FocData data;
	data.i_d = mFoc_IIO_Handle->readChannel(Id, "raw");
	data.i_q = mFoc_IIO_Handle->readChannel(Iq, "raw");
	data.i_alpha = mFoc_IIO_Handle->readChannel(I_alpha, "raw");
	data.i_beta = mFoc_IIO_Handle->readChannel(I_beta, "raw");
	data.i_homopolar = mFoc_IIO_Handle->readChannel(I_homopolar, "raw");
	data.speed = mFoc_IIO_Handle->readChannel(speed_pi_out, "raw");
	data.torque = mFoc_IIO_Handle->readChannel(torque_pi_out, "raw");
	data.flux = mFoc_IIO_Handle->readChannel(flux, "raw");
	return data;
}