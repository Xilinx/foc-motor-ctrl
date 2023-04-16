/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <iio.h>

IIO_Driver::IIO_Driver(const std::string &name)
{
	ctx = iio_create_scan_context(NULL, 0);
	if (!ctx) {
		//error handling
	}

	dev = iio_context_find_device(ctx, name);
	if (!dev) {
		//error handling
	}
}


IIO_Driver::~IIO_Driver()
{
	delete ctx;
	delete dev;
}

int IIO_Driver::readChannel(const std::string channelName)
{
	/* find and read channel */
	return 0;
}

void IIO_Driver::writeChannel(const std::string channelName)
{
	/* find & write channel */
}
