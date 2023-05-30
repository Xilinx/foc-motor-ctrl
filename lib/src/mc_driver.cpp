#include "mc_driver.h"
#include <iostream>

const std::string mc_uio::kUioDriverName = "motor_control";


mc_uio::mc_uio(/* args */)
{
	uioHandle = new UioDrv(kUioDriverName);
}

mc_uio::~mc_uio()
{
	delete uioHandle;
}

int mc_uio::set_gate_drive(bool value)
{
    if (uioHandle->virtualAddr)
    {
        if (value)
        {
            uioHandle->regWrite(GATE_DRIVE_EN, 0x1);
        }
        else
        {
            uioHandle->regWrite(GATE_DRIVE_EN, 0x0);
        }
    return 0;
    }

    throw std::runtime_error("Failed mapping motor control IP address");
}

uint32_t mc_uio::get_gate_drive()
{
    return uioHandle->regRead(GATE_DRIVE_EN);
}
