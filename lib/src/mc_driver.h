/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#ifndef _MCUIO_H_
#define _MCUIO_H_
#include "interface/uio_drv.h"

class MC_Uio
{
private:
	static const std::string kUioDriverName;
	UioDrv* mUioHandle;
public:
	MC_Uio(/* args */);
	~MC_Uio();
	int setGateDrive(bool value);
	uint32_t getGateDrive();
};

#endif
