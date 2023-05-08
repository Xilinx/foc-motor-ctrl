/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: MIT
 */
#include <pybind11/pybind11.h>
#include "motor-control/motor-control.hpp"

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
        .def("setSpeed", &MotorControl::SetSpeed)
        .def("setTorque", &MotorControl::SetTorque)
        .def("setPosition", &MotorControl::SetPosition)
        .def("setGain", py::overload_cast<GainType, int, int>(&MotorControl::SetGain))
        .def("clearFaults", py::overload_cast<>(&MotorControl::clearFaults))
        .def("clearFaults", py::overload_cast<FaultCategory>(&MotorControl::clearFaults));

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

    py::enum_<FaultType>(m, "FaultType")
        .value("kPhaseA_OC", FaultType::kPhaseA_OC)
        .value("kPhaseB_OC", FaultType::kPhaseB_OC)
        .value("kPhaseC_OC", FaultType::kPhaseC_OC)
        .value("kDCLink_OC", FaultType::kDCLink_OC)
        .value("kDCLink_OV", FaultType::kDCLink_OV)
        .value("kDCLink_UV", FaultType::kDCLink_UV)
        .value("kFalutTypeMax", FaultType::kFalutTypeMax);

    py::enum_<FaultCategory>(m, "FaultCategory")
        .value("kFaultCategoryMax", FaultCategory::kFaultCategoryMax);
}
