/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "uio_drv.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define UIO_PATH "/sys/class/uio/"
#define ADDR_PATH "/maps/map0/"
/* only use the first map (only one mapping is expected for this device) */
#define PHYSIZE 0x100

using namespace std;

UioDrv::UioDrv(std::string name)
{
    findDeviceId(name);
    mapDevice();
}

int UioDrv::regWrite(int offset, int value)
{
    if (mVirtualAddr == 0)
    {
        return -1;
    }
    uint64_t regAddr = mVirtualAddr + offset;
    *(volatile int *)(regAddr) = value;
    return 0;
}

uint32_t UioDrv::regRead(int offset)
{
    if (mVirtualAddr == 0)
    {
        return -1;
    }
    uint64_t regAddr = mVirtualAddr + offset;
    return *(volatile int *)(regAddr);
}

int UioDrv::findUioDevicenode(string &name)
{
    if (UioDrv::mUioDevNode.empty())
    {
        std::cerr << "Uio dev node not found" << std::endl;
        return -1;
    }
    name = "/dev/uio/" + UioDrv::mUioDevNode;
    return 0;
}

int UioDrv::findDeviceId(std::string kUioDriverName)
{
    std::string namePath;
    filesystem::path path = UIO_PATH;
    filesystem::directory_iterator list(path);
    if (list == end(list))
    {
        std::cout << " No uio device found for " << kUioDriverName << std::endl;
        return -1;
    }

    for (auto const &entry : list)
    {
        std::string dName = "";
        std::string namePath = "/sys/class/uio/" + entry.path().filename().string() + "/name";
        ifstream devicefile(namePath);
        if (devicefile.is_open())
        {
            getline(devicefile, dName);
            devicefile.close();
        }
        if (kUioDriverName == dName)
        {
            std::cout << "Using uio device " << entry.path().filename().string() << " for motor contorl" << std::endl;
            mUioDevNode = entry.path().filename().string();
            break;
        }
    }
    if (mUioDevNode == "NULL")
    {
        std::cout << " No uio device found for " << kUioDriverName << std::endl;
        return -1;
    }

    return 0;
}

int UioDrv::mapDevice()
{
    string data = "";
    string uioAddrPath = UIO_PATH + mUioDevNode + ADDR_PATH + "addr";
    string uioSizePath = UIO_PATH + mUioDevNode + ADDR_PATH + "size";

    ifstream uioaddrfile(uioAddrPath);
    if (uioaddrfile.is_open())
    {
        getline(uioaddrfile, data);
        uioaddrfile.close();
        cout << "Map uio physical address: " << data << endl;
        mPhysicalAddr = stoi(data);
    }
    else
    {
        cout << "uio addr not found" << endl;
        return -1;
    }
    ifstream uiosize(uioSizePath);
    if (uiosize.is_open())
    {
        getline(uiosize, data);
        uiosize.close();
        cout << "Map uio size: " << data << endl;
        mPhysicalSize = stoi(data);
    }
    string uionode = "/dev/" + mUioDevNode;

    if ((mFileHandle = open(uionode.c_str(), O_RDWR)) < 0)
        return -1;

    /* system bug in uio-generic driver
        unable to use the size form syfs node.
        the size form syfs node is 0x1000
        but in HW the size alvailble is 0x100
        mapping fails with size > 0x100
        */
    mVirtualAddr = (uint64_t)mmap(NULL, PHYSIZE,
                                  PROT_READ | PROT_WRITE, MAP_SHARED,
                                  mFileHandle,
                                  0 * getpagesize()); /* use map0 */

    if (mVirtualAddr == (uint64_t)MAP_FAILED)
    {
        perror("Failed to map memory");
        close(mFileHandle);
        return -1;
    }
    return 0;
}

int UioDrv::releaseDevice()
{
    if (mVirtualAddr)
    {
        munmap((void *)mVirtualAddr, PHYSIZE);
        close(mFileHandle);
        mPhysicalAddr = 0;
        mPhysicalSize = 0;
        mVirtualAddr = 0;
        mFileHandle = -1;
    }
    return 0;
}

UioDrv::~UioDrv()
{
    releaseDevice();
}
