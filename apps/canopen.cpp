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
#if 1
#include "motor-control/motor-control.hpp"
#else
#include "dummy-motor-control.hpp"
#endif
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
		state.store(InternalState::Not_Ready_To_Switch_On);
		status_word = 0x0;
		operation_mode.store(No_Mode);
	}

	~MotorCtrlSlave()
	{
		//MotorControl::destroyMotorControlInstance(mpMotorCtrl);
		delete mpMotorCtrl;
	}

protected:
	void OnWrite(uint16_t idx, uint8_t subidx) noexcept override
	{
		if (idx == Obj_ControlWord && subidx == 0x0)
		{
			{
				std::scoped_lock<std::mutex> lock(w_mutex);
				control_word = (*this)[idx][subidx];
			}
			update_state();
			{
				std::scoped_lock<std::mutex> lock(w_mutex);
				(*this)[Obj_StatusWord][0] = status_word;
				this->TpdoEvent(1);
			}
		}
		// Operation Mode
		if (idx == Obj_OpMode && subidx == 0)
		{
			int8_t mode = (*this)[idx][subidx];
			switch (mode)
			{
			case No_Mode:
			case Profiled_Velocity:
			case Profiled_Torque:
			// Remove below modes
			case Profiled_Position:
			case Velocity:
			case Reserved:
			case Homing:
			case Interpolated_Position:
			case Cyclic_Synchronous_Position:
			case Cyclic_Synchronous_Velocity:
			case Cyclic_Synchronous_Torque:
				operation_mode.store(mode);
				break;
			default:
				std::cout << "Error: Master tried to set unknown operation mode." << std::endl;
			}
			std::cout << "Switched to mode :" << mode << std::endl;
			(*this)[Obj_OpModeDisplay][0] = (int8_t)(mode);
			this->TpdoEvent(1);
		}
	}

	void update_state()
	{
		switch (state.load())
		{
		case InternalState::Not_Ready_To_Switch_On:
			on_not_ready_to_switch_on();
			break;
		case InternalState::Switch_On_Disabled:
			on_switch_on_disabled();
			break;
		case InternalState::Ready_To_Switch_On:
			on_ready_to_switch_on();
			break;
		case InternalState::Switched_On:
			on_switched_on();
			break;
		case InternalState::Operation_Enable:
			on_operation_enabled();
			break;
		case InternalState::Quick_Stop_Active:
			on_quickstop_active();
			break;
		case InternalState::Fault_Reaction_Active:
			break;
		case InternalState::Fault:
			std::cout<<"Fault.. Check why";
			break;
		default:
			break;
		}
	}

	void on_not_ready_to_switch_on()
	{
		set_switch_on_disabled();
	}

	void on_switch_on_disabled()
	{
		if (is_shutdown())
		{
			set_ready_to_switch_on();
		}
	}

	void on_ready_to_switch_on()
	{
		if (is_disable_voltage())
		{
			set_switch_on_disabled();
		}
		if (is_switch_on())
		{
			set_switch_on();
		}
		if (is_fault_reset())
		{
			set_ready_to_switch_on();
		}
	}

	void on_switched_on()
	{
		if (is_disable_voltage())
		{
			set_switch_on_disabled();
		}
		if (is_shutdown())
		{
			set_ready_to_switch_on();
		}
		if (is_enable_operation())
		{
			set_operation_enabled();
		}
	}

	void on_operation_enabled()
	{
		if (is_disable_voltage())
		{
			set_switch_on_disabled();
		}
		if (is_shutdown())
		{
			set_ready_to_switch_on();
		}
		if (is_switch_on())
		{
			set_switch_on();
		}
		if (is_quickstop())
		{
			set_quick_stop();
		}
		{
			std::scoped_lock<std::mutex> lock(w_mutex);
			is_relative.store(((control_word >> 6) & 1U) == 1U);
			is_halt.store(((control_word >> 8) & 1U) == 1U);
			is_new_set_point.store(((control_word >> 4) & 1U) == 1U);
		}

		if (old_operation_mode.load() != operation_mode.load())
		{
			if (profiled_velocity_mode.joinable())
			{
				std::cout<<"Joined velocity thread"<<std::endl;
				handleModeChange(MotorOpMode::kModeOff);
				profiled_velocity_mode.join();
			}
			if (profiled_torque_mode.joinable())
			{
				std::cout<<"Joined torque thread"<<std::endl;
				handleModeChange(MotorOpMode::kModeOff);
				profiled_torque_mode.join();
			}

			old_operation_mode.store(operation_mode.load());

			switch (operation_mode.load())
			{
			case Profiled_Velocity:
				start_profile_vel_mode();
				break;
			case Profiled_Torque:
				start_profile_torque_mode();
				break;
			default:
				break;
			}
		}
	}

	void start_profile_vel_mode()
	{
		profiled_velocity_mode = std::thread(std::bind(&MotorCtrlSlave::run_vel_mode, this));
	}

	void start_profile_torque_mode()
	{
		profiled_torque_mode = std::thread(std::bind(&MotorCtrlSlave::run_torque_mode, this));
	}

	void on_quickstop_active()
	{
		if (is_enable_operation())
		{
			set_operation_enabled();
		}
		if (is_disable_voltage())
		{
			set_switch_on_disabled();
		}
	}

	void set_status_bit(int bit)
	{
		std::scoped_lock<std::mutex> lock(w_mutex);
		status_word |= 1UL << bit;
	}

	void clear_status_bit(int bit)
	{
		std::scoped_lock<std::mutex> lock(w_mutex);
		status_word &= ~(1UL << bit);
	}

	bool is_shutdown()
	{
		std::scoped_lock<std::mutex> lock(w_mutex);
		bool fr_unset = ((control_word >> CW_Fault_Reset) & 1U) == 0U;
		bool qs_set = ((control_word >> CW_Quick_Stop) & 1U) == 1U;
		bool ev_set = ((control_word >> CW_Enable_Voltage) & 1U) == 1U;
		bool so_unset = ((control_word >> CW_Switch_On) & 1U) == 0U;

		if (fr_unset && qs_set && ev_set && so_unset)
		{
			std::cout << __FUNCTION__ << " : True" << std::endl;
			return true;
		}
		return false;
	}

	bool is_disable_voltage()
	{
		std::scoped_lock<std::mutex> lock(w_mutex);
		bool fr_unset = ((control_word >> CW_Fault_Reset) & 1U) == 0U;
		bool ev_unset = ((control_word >> CW_Enable_Voltage) & 1U) == 0U;

		if (fr_unset && ev_unset)
		{
			std::cout << __FUNCTION__ << " : True" << std::endl;
			return true;
		}
		return false;
	}

	bool is_switch_on()
	{
		std::scoped_lock<std::mutex> lock(w_mutex);
		bool fr_unset = ((control_word >> CW_Fault_Reset) & 1U) == 0U;
		bool eo_unset = ((control_word >> CW_Enable_Operation) & 1U) == 0U;
		bool qs_set = ((control_word >> CW_Quick_Stop) & 1U) == 1U;
		bool ev_set = ((control_word >> CW_Enable_Voltage) & 1U) == 1U;
		bool so_set = ((control_word >> CW_Switch_On) & 1U) == 1U;
		if (fr_unset && eo_unset && qs_set && ev_set && so_set)
		{
			std::cout << __FUNCTION__ << " : True" << std::endl;
			return true;
		}
		return false;
	}

	bool is_enable_operation()
	{
		std::scoped_lock<std::mutex> lock(w_mutex);
		bool fr_unset = ((control_word >> CW_Fault_Reset) & 1U) == 0U;
		bool eo_set = ((control_word >> CW_Enable_Operation) & 1U) == 1U;
		bool qs_set = ((control_word >> CW_Quick_Stop) & 1U) == 1U;
		bool ev_set = ((control_word >> CW_Enable_Voltage) & 1U) == 1U;
		bool so_set = ((control_word >> CW_Switch_On) & 1U) == 1U;
		if (fr_unset && eo_set && qs_set && ev_set && so_set)
		{
			std::cout << __FUNCTION__ << " : True" << std::endl;
			return true;
		}
		return false;
	}

	bool is_quickstop()
	{
		std::scoped_lock<std::mutex> lock(w_mutex);
		bool fr_unset = ((control_word >> CW_Fault_Reset) & 1U) == 0U;
		bool qs_unset = ((control_word >> CW_Quick_Stop) & 1U) == 0U;
		bool ev_set = ((control_word >> CW_Enable_Voltage) & 1U) == 1U;
		if (fr_unset && qs_unset && ev_set)
		{
			std::cout << __FUNCTION__ << " : True" << std::endl;
			return true;
		}
		return false;
	}

	bool is_fault_reset()
	{
		std::scoped_lock<std::mutex> lock(w_mutex);
		bool fr_set = ((control_word >> CW_Fault_Reset) & 1U) == 1U;
		if (fr_set)
		{
			std::cout << __FUNCTION__ << " : True" << std::endl;
			return true;
		}
		return false;
	}

	void set_switch_on_disabled()
	{
		std::cout << __FUNCTION__ << std::endl;
		state.store(InternalState::Switch_On_Disabled);
		clear_status_bit(SW_Ready_To_Switch_On);
		clear_status_bit(SW_Switched_On);
		clear_status_bit(SW_Operation_enabled);
		clear_status_bit(SW_Fault);
		set_status_bit(SW_Switch_on_disabled);
	}

	void set_ready_to_switch_on()
	{
		std::cout << __FUNCTION__ << std::endl;
		state.store(InternalState::Ready_To_Switch_On);
		set_status_bit(SW_Ready_To_Switch_On);
		clear_status_bit(SW_Switched_On);
		clear_status_bit(SW_Operation_enabled);
		clear_status_bit(SW_Fault);
		set_status_bit(SW_Quick_stop);
		clear_status_bit(SW_Switch_on_disabled);
	}

	void set_switch_on()
	{
		std::cout << __FUNCTION__ << std::endl;
		state.store(InternalState::Switched_On);
		set_status_bit(SW_Ready_To_Switch_On);
		set_status_bit(SW_Switched_On);
		clear_status_bit(SW_Operation_enabled);
		clear_status_bit(SW_Fault);
		set_status_bit(SW_Quick_stop);
		clear_status_bit(SW_Switch_on_disabled);
	}

	void set_operation_enabled()
	{
		std::cout << __FUNCTION__ << std::endl;
		state.store(InternalState::Operation_Enable);
		set_status_bit(SW_Ready_To_Switch_On);
		set_status_bit(SW_Switched_On);
		set_status_bit(SW_Operation_enabled);
		clear_status_bit(SW_Fault);
		set_status_bit(SW_Quick_stop);
		clear_status_bit(SW_Switch_on_disabled);
	}

	void set_quick_stop()
	{
		std::cout << __FUNCTION__ << std::endl;
		state.store(InternalState::Quick_Stop_Active);
		set_status_bit(SW_Ready_To_Switch_On);
		set_status_bit(SW_Switched_On);
		set_status_bit(SW_Operation_enabled);
		clear_status_bit(SW_Fault);
		clear_status_bit(SW_Quick_stop);
		clear_status_bit(SW_Switch_on_disabled);
	}

	void OnSync(uint8_t cnt, const time_point& t) noexcept override
	{
		(void) cnt;
		(void) t;
	}


private:
	MotorControl *mpMotorCtrl;
	std::atomic<bool> is_relative;
	std::atomic<bool> is_running;
	std::atomic<bool> is_halt;
	std::atomic<bool> is_new_set_point;
	std::atomic<int8_t> operation_mode;
	std::atomic<int8_t> old_operation_mode;

	std::mutex w_mutex;
	uint16_t status_word;
	uint16_t control_word;
	std::atomic<InternalState> state;

	std::thread profiled_velocity_mode;
	std::thread profiled_torque_mode;

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

	void run_vel_mode(void)
	{
		std::cout << __FUNCTION__ << ":" <<__LINE__<< std::endl;
		double target_speed = static_cast<double>(((uint32_t)(*this)[Obj_TargetVelocity][0])) / 1000;
		double actual_position = static_cast<double>(((int32_t)(*this)[Obj_PositionActual][0])) / 1000.0;
		double actual_speed = static_cast<double>(((int32_t)(*this)[Obj_VelocityActual][0])) / 1000.0;

		while ((state.load() == InternalState::Operation_Enable) &&
			   (operation_mode.load() == Profiled_Velocity))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			target_speed = static_cast<double>(((int32_t)(*this)[Obj_TargetVelocity][0])) / 1000.0;
			//if (target_position != actual_position)
			if ((actual_speed > (target_speed + (target_speed/10))) ||
				(actual_speed < (target_speed - (target_speed/10)))	) 	//for now consider 10% margin
			{
				clear_status_bit(SW_Operation_mode_specific0);
				clear_status_bit(SW_Target_reached);
				{
					std::scoped_lock<std::mutex> lock(w_mutex);
					(*this)[Obj_StatusWord][0] = status_word;
					this->TpdoEvent(1);
				}
				is_new_set_point.store(false);

				this->handleModeChange (MotorOpMode::kModeSpeed);
				this->setSpeed(target_speed);
				//while (Vel target reached)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					actual_position = getPosition();
					actual_speed = getPosition();
					(*this)[Obj_PositionActual][0] = (int32_t)(actual_position * 1000);
					(*this)[Obj_VelocityActual][0] = (int32_t)(actual_speed * 1000);
				}

				clear_status_bit(SW_Operation_mode_specific0);
				set_status_bit(SW_Target_reached);
				{
					std::scoped_lock<std::mutex> lock(w_mutex);
					(*this)[0x6041][0] = status_word;
					this->TpdoEvent(1);
				}
			}
		}
	}

	void run_torque_mode(void)
	{
		std::cout << __FUNCTION__ << ":" <<__LINE__<< std::endl;
		double target_torque = static_cast<double>(((uint32_t)(*this)[Obj_TargetTorque][0])) / 1000;
		double actual_position = static_cast<double>(((int32_t)(*this)[Obj_PositionActual][0])) / 1000.0;
		double actual_speed = static_cast<double>(((int32_t)(*this)[Obj_VelocityActual][0])) / 1000.0;

		while ((state.load() == InternalState::Operation_Enable) &&
			   (operation_mode.load() == Profiled_Velocity))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			target_torque = static_cast<double>(((uint32_t)(*this)[Obj_TargetTorque][0])) / 1000;
			//if (target_position != actual_position)
			//if ((actual_speed > (target_speed + (target_speed/10))) ||
			//	(actual_speed < (target_speed - (target_speed/10)))	) 	//for now consider 10% margin
			{
				clear_status_bit(SW_Operation_mode_specific0);
				clear_status_bit(SW_Target_reached);
				{
					std::scoped_lock<std::mutex> lock(w_mutex);
					(*this)[Obj_StatusWord][0] = status_word;
					this->TpdoEvent(1);
				}
				is_new_set_point.store(false);

				handleModeChange (MotorOpMode::kModeTorque);
				setTorque(target_torque);
				//while (Vel target reached)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					actual_position = getPosition();
					actual_speed = getPosition();
					(*this)[Obj_PositionActual][0] = (int32_t)(actual_position * 1000);
					(*this)[Obj_VelocityActual][0] = (int32_t)(actual_speed * 1000);
				}

				clear_status_bit(SW_Operation_mode_specific0);
				set_status_bit(SW_Target_reached);
				{
					std::scoped_lock<std::mutex> lock(w_mutex);
					(*this)[0x6041][0] = status_word;
					this->TpdoEvent(1);
				}
			}
		}
	}

	void setSpeed(double speed)
	{
		mpMotorCtrl->SetSpeed(speed);
	}

	void setTorque(double torque)
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
