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

using namespace std;


UioDrv::UioDrv(std::string name)
{
    find_device_id(name);
    map_device();
}

int UioDrv::regWrite(int offset, int value)
{
    uint64_t regaddr = virtualAddr + offset;
    *(volatile int *)(regaddr) = value;
    return 0;
}

uint32_t UioDrv::regRead(int offset)
{
    uint64_t regaddr = virtualAddr + offset;
    return *(volatile int *)(regaddr);
}

int UioDrv::find_device_id(std::string kUioDriverName)
{
    std::string namepath;
    filesystem::path path = UIO_PATH;
    filesystem::directory_iterator list(path);
    if (list == end(list))
    {
        throw std::runtime_error("no uio device found");
    }

    for (auto const &entry : list)
    {
        std::string dname = "";
        std::string namepath = "/sys/class/uio/" + entry.path().filename().string() + "/name";
        ifstream devicefile(namepath);
        if (devicefile.is_open())
        {
            getline(devicefile, dname);
            devicefile.close();
        }
        if (kUioDriverName == dname)
        {
            std::cout << "Using uio device " << entry.path().filename().string() << " for motor contorl" << std::endl;
            kUioDevNo = entry.path().filename().string();
            break;
        }
    }
    if (kUioDevNo == "NULL")
    {
        std::cout << " No uio device found for " << kUioDriverName << std::endl;
        return -1;
    }

    return 0;
}

int UioDrv::map_device()
{
    string data = "";
    string uioaddrpath = UIO_PATH + kUioDevNo + ADDR_PATH + "addr";
    string uiosizepath = UIO_PATH + kUioDevNo + ADDR_PATH + "size";

    ifstream uioaddrfile(uioaddrpath);
    if (uioaddrfile.is_open())
    {
        getline(uioaddrfile, data);
        uioaddrfile.close();
        cout << "Map uio physical address: " << data << endl;
        physicalAddr = stoi(data);
    }
    else
    {
        cout << "uio addr not found" << endl;
        return -1;
    }
    ifstream uiosize(uiosizepath);
    if (uiosize.is_open())
    {
        getline(uiosize, data);
        uiosize.close();
        cout << "Map uio size: " << data << endl;
        physicalSize = stoi(data);
    }
    string uionode = "/dev/" + kUioDevNo;

    if ((FileHandle = open(uionode.c_str(), O_RDWR)) < 0)
        return -1;

    /* system bug in uio-generic driver
        unable to use the size form syfs node.
        the size form syfs node is 0x1000
        but in HW the size alvailble is 0x100
        mapping fails with size > 0x100
        */
    virtualAddr = (uint64_t)mmap(NULL, PHYSIZE,
                                 PROT_READ | PROT_WRITE, MAP_SHARED,
                                 FileHandle,
                                 0 * getpagesize()); /* use map0 */

    if (virtualAddr == (uint64_t)MAP_FAILED)
    {
        perror("Failed to map memory");
        close(FileHandle);
        return -1;
    }
    return 0;
}

int UioDrv::release_device()
{
    if (virtualAddr)
    {
        munmap((void *)virtualAddr, PHYSIZE);
        close(FileHandle);
        physicalAddr = 0;
        physicalSize = 0;
        virtualAddr = 0;
        FileHandle = -1;
    }
    return 0;
}


UioDrv::~UioDrv()
{
    release_device();
}
