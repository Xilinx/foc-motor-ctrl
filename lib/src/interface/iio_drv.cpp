/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "iio_drv.h"

IIO_Driver::IIO_Driver(const std::string &name)
{
	// init ctx and dev
}


IIO_Driver::~IIO_Driver()
{
}

int IIO_Driver::readChannel(const std::string channelName)
{
	/* find and read channel */
	return 0;
}

void IIO_Driver::writeChannel(const std::string channelName, int val)
{
	/* find & write channel */
}
