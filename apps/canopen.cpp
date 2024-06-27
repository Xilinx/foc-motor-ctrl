/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "canopen.h"
#include <atomic>
#include <iostream>
#include <lely/coapp/slave.hpp>
#include <lely/ev/loop.hpp>
#include <lely/io2/linux/can.hpp>
#include <lely/io2/posix/poll.hpp>
#include <lely/io2/sys/io.hpp>
#include <lely/io2/sys/sigset.hpp>
#include <lely/io2/sys/timer.hpp>
#include <mutex>
#include <thread>
#include "motor-control/motor-control.hpp"

using namespace lely;
using namespace canopen_foc_mc;

class MotorCtrlSlave : public canopen::BasicSlave
{
public:
    using BasicSlave::BasicSlave;

    MotorCtrlSlave(io::TimerBase &timer, io::CanChannelBase &chan, const ::std::string &dcf_txt,
                   const ::std::string &dcf_bin = "", uint8_t id = 0xff)
    : BasicSlave(timer, chan, dcf_txt, dcf_bin, id)
    {
        mpMotorCtrl = MotorControl::getMotorControlInstance(1);
        state.store(InternalState::Not_Ready_To_Switch_On);
        status_word = 0x0;
        operation_mode.store(No_Mode);
    }

    ~MotorCtrlSlave() { delete mpMotorCtrl; }

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
                case Profiled_Position:
                case Velocity:
                case Profiled_Velocity:
                case Profiled_Torque:
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
                std::cout << "Fault.. Check why";
                break;
            default:
                break;
        }
    }

    void on_not_ready_to_switch_on() { set_switch_on_disabled(); }

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
            // Stop the Motor if it is running
            stop_motor();
        }
        {
            std::scoped_lock<std::mutex> lock(w_mutex);
            is_relative.store(((control_word >> 6) & 1U) == 1U);
            is_halt.store(((control_word >> 8) & 1U) == 1U);
            is_new_set_point.store(((control_word >> 4) & 1U) == 1U);
        }

        if (old_operation_mode.load() != operation_mode.load())
        {
            // Stop the Motor if it is running
            stop_motor();
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

    double _last_speed, _last_torque;
    int _last_mode;

    void handle_mode_change(MotorOpMode mode)
    {
        int new_mode = static_cast<int>(mode);
        if (_last_mode != new_mode)
        {
            mpMotorCtrl->setOperationMode(mode);
            _last_mode = new_mode;
        }
    }

    void stop_motor(void)
    {
        if (profiled_velocity_mode.joinable())
        {
            std::cout << "Joined velocity thread" << std::endl;
            profiled_velocity_mode.join();
            handle_mode_change(MotorOpMode::kModeOff);
        }
        if (profiled_torque_mode.joinable())
        {
            std::cout << "Joined torque thread" << std::endl;
            profiled_torque_mode.join();
            handle_mode_change(MotorOpMode::kModeOff);
        }
    }

    void run_vel_mode(void)
    {
        std::cout << __FUNCTION__ << ":"
                  << "Started" << std::endl;
        double target_speed = static_cast<double>(((int32_t)(*this)[Obj_TargetVelocity][0])) / 1000;
        double actual_position =
            static_cast<double>(((int32_t)(*this)[Obj_PositionActual][0])) / 1000.0;
        double actual_speed =
            static_cast<double>(((int32_t)(*this)[Obj_VelocityActual][0])) / 1000.0;

        // std::cout << "Velocity (Actual/Target) : " << actual_speed << "/" << target_speed <<
        // std::endl;

        while ((state.load() == InternalState::Operation_Enable) &&
               (operation_mode.load() == Profiled_Velocity))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            target_speed = static_cast<double>(((int32_t)(*this)[Obj_TargetVelocity][0])) / 1000.0;
            // if (actual_speed != target_speed)
            {
                clear_status_bit(SW_Operation_mode_specific0);
                clear_status_bit(SW_Target_reached);
                {
                    std::scoped_lock<std::mutex> lock(w_mutex);
                    (*this)[Obj_StatusWord][0] = status_word;
                    this->TpdoEvent(1);
                }
                is_new_set_point.store(false);

                this->handle_mode_change(MotorOpMode::kModeSpeed);
                this->set_velocity(target_speed);

                actual_position = get_position();
                (*this)[Obj_PositionActual][0] = (int32_t)(actual_position * 1000);
                actual_speed = get_velocity();
                (*this)[Obj_VelocityActual][0] = (int32_t)(actual_speed * 1000);

                clear_status_bit(SW_Operation_mode_specific0);
                set_status_bit(SW_Target_reached);
                {
                    std::scoped_lock<std::mutex> lock(w_mutex);
                    (*this)[0x6041][0] = status_word;
                    this->TpdoEvent(1);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        (*this)[Obj_PositionActual][0] = (int32_t)(0);
        (*this)[Obj_VelocityActual][0] = (int32_t)(0);
        std::cout << __FUNCTION__ << ":"
                  << "Terminated" << std::endl;
    }

    void run_torque_mode(void)
    {
        std::cout << __FUNCTION__ << ":"
                  << "Started" << std::endl;
        double target_torque = static_cast<double>(((int16_t)(*this)[Obj_TargetTorque][0])) / 1000;
        double actual_position =
            static_cast<double>(((int32_t)(*this)[Obj_PositionActual][0])) / 1000.0;
        double actual_speed =
            static_cast<double>(((int32_t)(*this)[Obj_VelocityActual][0])) / 1000.0;

        while ((state.load() == InternalState::Operation_Enable) &&
               (operation_mode.load() == Profiled_Torque))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            target_torque = static_cast<double>(((int16_t)(*this)[Obj_TargetTorque][0])) / 1000;
            // if target is not achieved (Assume always)
            {
                clear_status_bit(SW_Operation_mode_specific0);
                clear_status_bit(SW_Target_reached);
                {
                    std::scoped_lock<std::mutex> lock(w_mutex);
                    (*this)[Obj_StatusWord][0] = status_word;
                    this->TpdoEvent(1);
                }
                is_new_set_point.store(false);

                handle_mode_change(MotorOpMode::kModeTorque);
                set_effort(target_torque);

                actual_position = get_position();
                actual_speed = get_position();
                (*this)[Obj_PositionActual][0] = (int32_t)(actual_position * 1000);
                (*this)[Obj_VelocityActual][0] = (int32_t)(actual_speed * 1000);

                clear_status_bit(SW_Operation_mode_specific0);
                set_status_bit(SW_Target_reached);
                {
                    std::scoped_lock<std::mutex> lock(w_mutex);
                    (*this)[0x6041][0] = status_word;
                    this->TpdoEvent(1);
                }
            }
        }
        std::cout << __FUNCTION__ << ":"
                  << "Terminated" << std::endl;
    }

    void set_velocity(double speed)
    {
        if (_last_speed != speed) mpMotorCtrl->SetSpeed(speed);
        _last_speed = speed;
    }

    void set_effort(double torque)
    {
        if (_last_torque != torque) mpMotorCtrl->SetTorque(torque);
        _last_torque = torque;
    }

    int get_velocity(void) { return mpMotorCtrl->getSpeed(); }

    int get_position(void) { return mpMotorCtrl->getPosition(); }
};

void print_usage(const char *programName)
{
    std::cout << "Usage: " << programName << " [options]\n"
              << "Options:\n"
              << "  -h, --help                   Print this help message\n"
              << "  -i, --interface <interface>  Name of the CAN interface. Default is "
              << DEFAULT_CAN_IF << "\n"
              << "  -n, --node <id>              Node id of the can slave. Default is "
              << DEFAULT_NODE_ID << "'\n"
              << "  -e, --eds <eds file>         Path to the eds file. \n"
              << "                               Default is " << DEFAULT_EDS_PATH << "\n";
}

void parse_arguments(int argc, char *argv[], std::string &interface, int &node,
                     std::string &eds_file)
{
    // Set defaults from macros
    interface = DEFAULT_CAN_IF;
    node = DEFAULT_NODE_ID;
    eds_file = DEFAULT_EDS_PATH;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            print_usage(argv[0]);
            exit(0);
        }
        else if (arg == "-e" || arg == "--eds")
        {
            if (i + 1 < argc)
            {
                eds_file = argv[++i];
                // TODO: Validate if the path exists
            }
            else
            {
                std::cerr << "Error: --eds option requires an argument.\n";
                print_usage(argv[0]);
                exit(1);
            }
        }
        else if (arg == "-i" || arg == "--interface")
        {
            if (i + 1 < argc)
            {
                interface = argv[++i];
            }
            else
            {
                std::cerr << "Error: --interface option requires an argument.\n";
                print_usage(argv[0]);
                exit(1);
            }
        }
        else if (arg == "-n" || arg == "--node")
        {
            if (i + 1 < argc)
            {
                try
                {
                    node = std::stoi(argv[++i]);
                }
                catch (std::invalid_argument &)
                {
                    std::cerr << "Error: Invalid node ID. It must be an integer.\n";
                    print_usage(argv[0]);
                    exit(1);
                }
            }
            else
            {
                std::cerr << "Error: --node option requires an argument.\n";
                print_usage(argv[0]);
                exit(1);
            }
        }
        else
        {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage(argv[0]);
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    std::string can_interface;
    std::string eds_file;
    int node_id;

    parse_arguments(argc, argv, can_interface, node_id, eds_file);

    // Initialize I/O context and polling instance
    io::IoGuard io_guard;
    io::Context ctx;
    io::Poll poll(ctx);

    // Create event loop, timer, and thread for TPDO update
    ev::Loop loop(poll.get_poll());
    auto exec = loop.get_executor();
    io::Timer timer(poll, exec, CLOCK_MONOTONIC);

    // Create virtual SocketCAN CAN controller and channel
    io::CanController ctrl(can_interface.c_str());
    io::CanChannel chan(poll, exec);
    chan.open(ctrl);

    // Print the arguments used
    std::cout << "CAN Interface: " << can_interface << "\n";
    std::cout << "Node ID: " << node_id << "\n";
    std::cout << "EDS file: " << eds_file << "\n";

    // Create CANopen slave with desired node ID
    MotorCtrlSlave slave(timer, chan, eds_file, "", node_id);

    // Create signal handler for clean shutdown
    io::SignalSet sigset(poll, exec);
    sigset.insert(SIGHUP);
    sigset.insert(SIGINT);
    sigset.insert(SIGTERM);
    sigset.submit_wait(
        [&](int /*signo*/)
        {
            sigset.clear();
            ctx.shutdown();
        });

    // Start NMT service (assuming reset node command)
    slave.Reset();

    // Run main loop and TPDO update thread
    loop.run();

    return 0;
}
