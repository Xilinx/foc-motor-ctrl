/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include <lely/ev/loop.hpp>
#include <lely/io2/linux/can.hpp>
#include <lely/io2/posix/poll.hpp>
#include <lely/io2/sys/io.hpp>
#include <lely/io2/sys/sigset.hpp>
#include <lely/io2/sys/timer.hpp>
#include <lely/coapp/slave.hpp>
#include <thread>
#include <iostream>
#include "motor-control/motor-control.hpp"
#include "canopen.h"
#include <atomic>
#include <mutex>
#include <thread>

using namespace lely;
using namespace canopen_foc_mc;

class MotorCtrlSlave: public canopen::BasicSlave {
public:
	using BasicSlave::BasicSlave;

	MotorCtrlSlave(io::TimerBase &timer, io::CanChannelBase &chan,
		       const ::std::string &dcf_txt, const ::std::string &dcf_bin="", uint8_t id=0xff):
		BasicSlave(timer, chan, dcf_txt, dcf_bin,id)
	{
		mpMotorCtrl = MotorControl::getMotorControlInstance(1);
		controlWord = 0x0;
		statusWord = 0x0;
	}

	~MotorCtrlSlave()
	{
		delete mpMotorCtrl;
	}

protected:
	void OnWrite(uint16_t idx, uint8_t subidx) noexcept override
	{
		if (idx == Obj_ControlWord && subidx == 0x0) {
			uint16_t control = (*this)[idx][subidx];

			if (control & 0xB == 0xB) {
				//turn on the motor
				uint16_t mode = (*this)[Obj_OpMode][0];
				switch (mode)
				{
					case No_Mode:
						operationMode.store(mode);
						break;
					case Profiled_Velocity:
						operationMode.store(mode);
						handleModeChange(MotorOpMode::kModeSpeed);
						break;
					case Profiled_Torque:
						operationMode.store(mode);
						handleModeChange(MotorOpMode::kModeTorque);
						break;
					case Profiled_Position:
					case Velocity:
					case Reserved:
					case Homing:
					case Interpolated_Position:
					case Cyclic_Synchronous_Position:
					case Cyclic_Synchronous_Velocity:
					case Cyclic_Synchronous_Torque:
						std::cerr << "Error: Master tried to set unsupported operation mode: "
							<< mode << std::endl;
						break;
					default:
						std::cerr << "Error: Master tried to set unknown operation mode: "
							<< mode << std::endl;
				}
			}
			if (control & 0x104) {
				handleModeChange(MotorOpMode::kModeOff);
			}
		}
	}

	void OnSync(uint8_t cnt, const time_point& t) noexcept override
	{
		std::cout<<".\n";
		(void) cnt;
		(void) t;
		updatePDOs();
		this->TpdoEvent(1);
	}


private:
	MotorControl *mpMotorCtrl;
	uint16_t statusWord;
	uint16_t controlWord;
	std::atomic<int8_t> operationMode;

	void handleModeChange(MotorOpMode mode)
	{
#if 0
		switch (mode) {
		case 0:
			mpMotorCtrl->setOperationMode(MotorOpMode::kModeOff);
			break;
		case 1:
			mpMotorCtrl->setOperationMode(MotorOpMode::kModeSpeed);
			break;
		case 2:
			mpMotorCtrl->setOperationMode(MotorOpMode::kModeTorque);
			break;
		case 3:
			mpMotorCtrl->setOperationMode(MotorOpMode::kModeOpenLoop);
			break;
		default:
			break;
		}
#else
		mpMotorCtrl->setOperationMode(mode);
#endif
	}

	void updatePDOs()
	{
		auto speedValue = getSpeed();
		auto positionValue = getPosition();

		(*this)[Obj_VelocityActual][0] = (int32_t) speedValue; // Update Speed object (object 5000:00).
		(*this)[Obj_StatusWord][0] = statusWord; // update this as specs.
		// (*this)[Obj_PositionActual][0] = (uint32_t) positionValue; // update this as specs.

	}

	void setSpeed(int speed)
	{
		mpMotorCtrl->SetSpeed(speed);
	}

	void setTorque(int torque)
	{
		mpMotorCtrl->SetTorque(torque);
	}

	int getSpeed(void)
	{
		return mpMotorCtrl->getSpeed();
	}
	int getPosition(void)
	{
		return mpMotorCtrl->getPosition();
	}

};

int main(int argc, char* argv[]) {
	// Get CAN interface name (default to "vcan0")
	const char* can_interface = (argc > 1) ? argv[1] : "vcan0";

	// Initialize I/O context and polling instance
	io::IoGuard io_guard;
	io::Context ctx;
	io::Poll poll(ctx);

	// Create event loop, timer, and thread for TPDO update
	ev::Loop loop(poll.get_poll());
	auto exec = loop.get_executor();
	io::Timer timer(poll, exec, CLOCK_MONOTONIC);


	// Create virtual SocketCAN CAN controller and channel
	io::CanController ctrl(can_interface);
	io::CanChannel chan(poll, exec);
	chan.open(ctrl);

	// Create CANopen slave with desired node ID
	MotorCtrlSlave slave(timer, chan, EDS_PATH , "", SLAVE_ID);

	// Create signal handler for clean shutdown
	io::SignalSet sigset(poll, exec);
	sigset.insert(SIGHUP);
	sigset.insert(SIGINT);
	sigset.insert(SIGTERM);
	sigset.submit_wait([&](int /*signo*/) {
		sigset.clear();
		ctx.shutdown();
	});

	// Start NMT service (assuming reset node command)
	slave.Reset();

	// Run main loop and TPDO update thread
	loop.run();

	return 0;
}

