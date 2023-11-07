/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#include <pybind11/pybind11.h>
#include "motor-control/motor-control.hpp"
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <map>
#include <vector>

#ifndef PY_MODULE_NAME
#define PY_MODULE_NAME	py_motor_control
#endif

namespace py = pybind11;

PYBIND11_MODULE(PY_MODULE_NAME, m) {
    py::class_<MotorControl, std::unique_ptr<MotorControl, py::nodelete>>(m, "MotorControl")
        .def_static("getMotorControlInstance", &MotorControl::getMotorControlInstance,
				py::arg("sessionId"), py::arg("configPath") = DEFAULT_CONFIG_PATH)
        .def("getSpeed", &MotorControl::getSpeed)
        .def("getPosition", &MotorControl::getPosition)
        .def("getTorque", &MotorControl::getTorque)
        .def("getCurrent", &MotorControl::getCurrent)
        .def("getVoltage", &MotorControl::getVoltage)
        .def("getFaultStatus", &MotorControl::getFaultStatus)
        .def("getFocCalc", &MotorControl::getFocCalc)
        .def("getOperationMode", &MotorControl::getOperationMode)
        .def("GetGain", &MotorControl::GetGain)
        .def("getVfParamVq", &MotorControl::getVfParamVq)
        .def("getVfParamVd", &MotorControl::getVfParamVd)
        .def("setSpeed", &MotorControl::SetSpeed)
        .def("setTorque", &MotorControl::SetTorque)
        .def("getSpeedSetpoint", &MotorControl::getSpeedSetValue)
        .def("getTorqueSetpoint", &MotorControl::getTorqueSetValue)
        .def("setPosition", &MotorControl::SetPosition)
        .def("setGain", &MotorControl::SetGain)
        .def("setOperationMode", &MotorControl::setOperationMode)
        .def("setVfParamVq", &MotorControl::setVfParamVq)
        .def("setVfParamVd", &MotorControl::setVfParamVd)
        .def("clearFaults", &MotorControl::clearFaults)
        .def("getMotorParams", &MotorControl::getMotorParams)
        .def("getConfigName", &MotorControl::getConfigName);

    py::class_<FocData>(m, "FocData")
        .def(py::init<>())
        .def_readwrite("i_alpha", &FocData::i_alpha)
        .def_readwrite("i_beta", &FocData::i_beta)
        .def_readwrite("i_d", &FocData::i_d)
        .def_readwrite("i_q", &FocData::i_q)
        .def_readwrite("i_homopolar", &FocData::i_homopolar)
        .def_readwrite("torque", &FocData::torque)
        .def_readwrite("speed", &FocData::speed)
        .def_readwrite("flux", &FocData::flux);

    py::class_<GainData>(m, "GainData")
        .def(py::init<>())
        .def_readwrite("kp", &GainData::kp)
        .def_readwrite("ki", &GainData::ki);

    py::enum_<MotorOpMode>(m, "MotorOpMode")
        .value("kModeOff", MotorOpMode::kModeOff)
        .value("kModeSpeed", MotorOpMode::kModeSpeed)
        .value("kModeTorque", MotorOpMode::kModeTorque)
        .value("kModeSpeedFW", MotorOpMode::kModeSpeedFW)
        .value("kModeOpenLoop", MotorOpMode::kModeOpenLoop)
        .value("kModePosControl", MotorOpMode::kModePosControl)
        .value("kModeMax", MotorOpMode::kModeMax);

    py::enum_<GainType>(m, "GainType")
        .value("kCurrent", GainType::kTorque)
        .value("kSpeed", GainType::kSpeed)
        .value("kFlux", GainType::kFlux)
        .value("kGainTypeMax", GainType::kGainTypeMax);

    py::enum_<ElectricalData>(m, "ElectricalData")
        .value("kPhaseA", ElectricalData::kPhaseA)
        .value("kPhaseB", ElectricalData::kPhaseB)
        .value("kPhaseC", ElectricalData::kPhaseC)
        .value("kDCLink", ElectricalData::kDCLink)
        .value("kElectricalDataMax", ElectricalData::kElectricalDataMax);

    py::enum_<FaultId>(m, "FaultId")
        .value("kPhaseA_OC", FaultId::kPhaseA_OC)
        .value("kPhaseB_OC", FaultId::kPhaseB_OC)
        .value("kPhaseC_OC", FaultId::kPhaseC_OC)
        .value("kDCLink_OC", FaultId::kDCLink_OC)
        .value("kDCLink_OV", FaultId::kDCLink_OV)
        .value("kDCLink_UV", FaultId::kDCLink_UV)
        .value("kPhaseImbalance", FaultId::kPhaseImbalance)
        .value("kAvgPowerFault", FaultId::kAvgPowerFault)
        .value("kFalutIdMax", FaultId::kFaultIdMax);

    py::enum_<MotorParam>(m, "MotorParam")
        .value("kVoltagePhaseA", MotorParam::kVoltagePhaseA)
        .value("kCurrentPhaseA", MotorParam::kCurrentPhaseA)
        .value("kVoltagePhaseB", MotorParam::kVoltagePhaseB)
        .value("kCurrentPhaseB", MotorParam::kCurrentPhaseB)
        .value("kVoltagePhaseC", MotorParam::kVoltagePhaseC)
        .value("kCurrentPhaseC", MotorParam::kCurrentPhaseC)
        .value("kVoltageDCLink", MotorParam::kVoltageDCLink)
        .value("kCurrentDCLink", MotorParam::kCurrentDCLink)
        .value("kId", MotorParam::kId)
        .value("kIq", MotorParam::kIq)
        .value("kIalpha", MotorParam::kIalpha)
        .value("kIbeta", MotorParam::kIbeta)
        .value("kIhomopolar", MotorParam::kIhomopolar)
        .value("kSpeedFoc", MotorParam::kSpeedFoc)
        .value("kTorqueFoc", MotorParam::kTorqueFoc)
        .value("kFlux", MotorParam::kFlux)
        .value("kRpmFoc", MotorParam::kRpmFoc)
        .value("kPositionFoc", MotorParam::kPositionFoc)
        .value("kRpm", MotorParam::kRpm)
        .value("kPosition", MotorParam::kPosition)
        .value("kMotorParamMax", MotorParam::kMotorParamMax);
}
