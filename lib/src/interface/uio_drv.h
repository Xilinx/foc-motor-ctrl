/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef _UIO_DRV_H_
#define _UIO_DRV_H_

#include <string>
#define UIO_PATH "/sys/class/uio/"
#define ADDR_PATH "/maps/map0/"
#define PHYSIZE 0x100

class UioDrv
{
private:
	std::string kUioDevNo;
	uint64_t physicalAddr;
	uint32_t physicalSize;
	int FileHandle;
	int find_device_id(std::string uioName);
	int map_device();
	int release_device();

public:
	UioDrv(std::string name);
	int regWrite(int offset, int value);
	uint32_t regRead(int offset);
	~UioDrv();
	uint64_t virtualAddr;

};

#endif /* _UIO_DRV_H_ */
