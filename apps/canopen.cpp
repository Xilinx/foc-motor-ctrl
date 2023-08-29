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
#include "motor-control/motor-control.hpp"

using namespace lely;

#define CONTROL_WORD	0x6040
#define STATUS_WORD	0x6041
#define OP_MODE		0x6060
#define PDO_SPEED	0x6081
#define PDO_POSITION	0x5001

#define EDS_PATH "/opt/xilinx/xlnx-app-kd240-foc-motor-ctrl/share/foc-motor-ctrl/foc-mc.eds"
#define SLAVE_ID	4


class MotorCtrlSlave: public canopen::BasicSlave {
public:
	using BasicSlave::BasicSlave;

	MotorCtrlSlave(io::TimerBase &timer, io::CanChannelBase &chan,
		       const ::std::string &dcf_txt, const ::std::string &dcf_bin="", uint8_t id=0xff):
		BasicSlave(timer, chan, dcf_txt, dcf_bin,id)
	{
		mpMotorCtrl = MotorControl::getMotorControlInstance(1);
	}

	~MotorCtrlSlave()
	{
		delete mpMotorCtrl;
	}

	// Update TPDO data at a regular interval
	void UpdateTpdoPeriodically()
	{
		//todo: Placeholder in case OnRead is not working
		while (true) {
			updatePDOs();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
protected:
	void OnWrite(uint16_t idx, uint8_t subidx) noexcept override
	{
		if (idx == CONTROL_WORD && subidx == 0x0) {
			uint16_t control = (*this)[idx][subidx];

			if (control & 0xB == 0xB) {
				//turn on the motor
				uint16_t mode = (*this)[OP_MODE][0];
				if(mode == 3) {
					handleModeChange(MotorOpMode::kModeSpeed);
				} else if (mode == 4) {
					handleModeChange(MotorOpMode::kModeTorque);
				} else {
					// not handling other modes
				}
			}
			if (control & 0x104) {
				handleModeChange(MotorOpMode::kModeOff);
			}
		}
	}


private:
	MotorControl *mpMotorCtrl;

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

		(*this)[PDO_SPEED][0] = speedValue; // Update Speed object (object 5000:00).
		(*this)[PDO_POSITION][0] = positionValue; // Update Position object (object 5001:00).
		(*this)[STATUS_WORD][0] = 0x0; // update this as specs.
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

	// Lamda function to update the tpdo periodically
	void (MotorCtrlSlave::*update_tpdo_func)() = &MotorCtrlSlave::UpdateTpdoPeriodically;
	std::thread tpdo_update_thread([update_tpdo_func, &slave]() {
		(slave.UpdateTpdoPeriodically)();
	});

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
	tpdo_update_thread.join();

	return 0;
}

