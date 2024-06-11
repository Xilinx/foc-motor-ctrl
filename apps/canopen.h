/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */

#define DEFAULT_EDS_PATH	"/opt/xilinx/xlnx-app-kd240-foc-motor-ctrl/share/foc-motor-ctrl/foc-mc.eds"
#define DEFAULT_NODE_ID		4
#define DEFAULT_CAN_IF		"can0"

namespace canopen_foc_mc
{
	enum ObjectIndx
	{
	// todo: Convert to array of index+subindex
	// Cia402 Profile specific objects
		Obj_ErrCode = 0x603F,
		Obj_ControlWord = 0x6040,
		Obj_StatusWord = 0x6041,
		Obj_QuickStop = 0x605A,
		Obj_Shutdown = 0x605B,
		Obj_OpMode = 0x6060,
		Obj_OpModeDisplay = 0x6061,
		Obj_PositionActual = 0x6064,
		Obj_VelocityActual = 0x606C,
		Obj_TargetTorque = 0x6071,
		Obj_TorqueActual = 0x6077,
		Obj_TargetVelocity = 0x60FF,
		Obj_SupportedDriveModes = 0x6502,
	};

	enum InternalState
	{
		Unknown = 0,
		Start = 0,
		Not_Ready_To_Switch_On = 1,
		Switch_On_Disabled = 2,
		Ready_To_Switch_On = 3,
		Switched_On = 4,
		Operation_Enable = 5,
		Quick_Stop_Active = 6,
		Fault_Reaction_Active = 7,
		Fault = 8,
	};

	enum StatusWord
	{
		SW_Ready_To_Switch_On = 0,
		SW_Switched_On = 1,
		SW_Operation_enabled = 2,
		SW_Fault = 3,
		SW_Voltage_enabled = 4,
		SW_Quick_stop = 5,
		SW_Switch_on_disabled = 6,
		SW_Warning = 7,
		SW_Manufacturer_specific0 = 8,
		SW_Remote = 9,
		SW_Target_reached = 10,
		SW_Internal_limit = 11,
		SW_Operation_mode_specific0 = 12,
		SW_Operation_mode_specific1 = 13,
		SW_Manufacturer_specific1 = 14,
		SW_Manufacturer_specific2 = 15
	};
	enum ControlWord
	{
		CW_Switch_On = 0,
		CW_Enable_Voltage = 1,
		CW_Quick_Stop = 2,
		CW_Enable_Operation = 3,
		CW_Operation_mode_specific0 = 4,
		CW_Operation_mode_specific1 = 5,
		CW_Operation_mode_specific2 = 6,
		CW_Fault_Reset = 7,
		CW_Halt = 8,
		CW_Operation_mode_specific3 = 9,
		// CW_Reserved1=10,
		CW_Manufacturer_specific0 = 11,
		CW_Manufacturer_specific1 = 12,
		CW_Manufacturer_specific2 = 13,
		CW_Manufacturer_specific3 = 14,
		CW_Manufacturer_specific4 = 15,
	};

	enum OperationMode
	{
		No_Mode = 0,
		Profiled_Position = 1,
		Velocity = 2,
		Profiled_Velocity = 3,
		Profiled_Torque = 4,
		Reserved = 5,
		Homing = 6,
		Interpolated_Position = 7,
		Cyclic_Synchronous_Position = 8,
		Cyclic_Synchronous_Velocity = 9,
		Cyclic_Synchronous_Torque = 10,
	};
}
