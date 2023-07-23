#ifndef _MCUIO_H_
#define _MCUIO_H_

#include "interface/uio_drv.h"
#define GATE_DRIVE_EN 0x00
#define PHASE_CURRENT_BALANCE_FAULT_VALUE 0x04
#define PHASE_CURRENT_BALANCE_FAULT_ENABLE 0x08
#define PHASE_CURRENT_BALANCE_FAULT_CLEAR 0x0C
#define PHASE_CURRENT_BALANCE_FAULT_STATUS 0x10 // read only
#define MOTOR_CONTORL_UIO_IRQ_DISABLE 0x14
#define MUX_SEL 0x18

class mc_uio
{
private:
	static const std::string kUioDriverName;
	UioDrv* uioHandle;
public:
	mc_uio(/* args */);
	~mc_uio();
	int set_gate_drive(bool value);
	uint32_t get_gate_drive();
};

#endif
