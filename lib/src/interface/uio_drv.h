/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _UIO_DRV_H_
#define _UIO_DRV_H_

#include <string>

class UioDrv
{
private:
	std::string mUioDevNode;
	uint64_t mPhysicalAddr;
	uint32_t mPhysicalSize;
	uint64_t mVirtualAddr;
	int mFileHandle;

	int findDeviceId(std::string uioName);
	int mapDevice();
	int releaseDevice();

public:
	UioDrv(std::string name);
	int regWrite(int offset, int value);
	uint32_t regRead(int offset);
	~UioDrv();
};

#endif /* _UIO_DRV_H_ */
