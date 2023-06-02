#!/usr/bin/python3
#
# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT
#

import py_foc_motor_ctrl as mcontrol
from bokeh.plotting import figure, curdoc
from bokeh.layouts import layout, row, column
from bokeh.models import Range1d, DataRange1d
from bokeh.models import Select
from bokeh.models import TextInput
from bokeh.models import Paragraph, Div
from bokeh.models import Button
from bokeh.driving import linear
from numpy import nan
from collections import deque

debug_print = False

css_style = Div(text="""
<style>
    body {
        color: #E0E0E0 !important;
        background-color: #15191C !important;
    }
</style>
""")

sample_size = 60
sample_size_actual = 60
interval = 1
x = deque([nan] * sample_size)

# Get a MotorControl instance with session ID 1 and default config path
mc = mcontrol.MotorControl.getMotorControlInstance(1)

# Initialize parameters
time = 0
speed_setpoint_min = 0
speed_setpoint_max = 5000
torque_setpoint_min = -2.5
torque_setpoint_max = 2.5

speed_setpoint = mc.getSpeedSetpoint()
torque_setpoint = mc.getTorqueSetpoint()
open_loop_vd = mc.getVfParamVd()
open_loop_vq = mc.getVfParamVq()
speed_gain = mc.GetGain(mcontrol.GainType.kSpeed)
torque_gain = mc.GetGain(mcontrol.GainType.kCurrent)
flux_gain = mc.GetGain(mcontrol.GainType.kFlux)

# title
title1 = Div(
    text="<h1>Kria&trade; SOM: Motor Dashboard</h1>",
    width=400
)

plot_titles = [
    'Phase A Motor Current',
    'Phase B Motor Current',
    'Phase C Motor Current',
    'Motor Angle',
    'Phase A Motor Voltage',
    'Phase B Motor Voltage',
    'Phase C Motor Voltage',
    'Motor Speed',
    'Id (FOC)',
    'Iq (FOC)',
    'Torque (FOC)',
    'Speed (FOC)'
]

num_plots = len(plot_titles)
plot_data = [0] * num_plots
data_list = [deque([0] * sample_size) for i in range(num_plots)]
plot_list = [figure(plot_width=800, plot_height=300, title=title) for title in plot_titles]
ds_list = [(plot.line(x, plot_data, line_width=2, color="darkseagreen")).data_source for plot, plot_data in zip(plot_list, data_list)]

# sample interval
def update_interval(attr, old, new):
    global interval
    interval = max(float(new), 0)
    interval_input.value = str(interval)
    global callback
    curdoc().remove_periodic_callback(callback)
    callback = curdoc().add_periodic_callback(update, interval * 1000)

interval_title = Paragraph(text="Interval in Seconds:", width=150, align="center")
interval_input = TextInput(value=str(interval), width=80)
interval_input.on_change('value', update_interval)

# sample size
def update_sample_size(attr, old, new):
    global sample_size, sample_size_actual
    new_sample_size = int(new)
    if new_sample_size < sample_size_actual:
        excess = sample_size_actual - new_sample_size
        while excess > 0:
            x.popleft()
            for data in data_list:
                data.popleft()
            excess = excess - 1
        sample_size_actual = new_sample_size
    sample_size = new_sample_size

sample_size_title = Paragraph(text="Sample Size:", width=150, align="center")
sample_size_input = TextInput(value=str(sample_size), width=80)
sample_size_input.on_change('value', update_sample_size)

# Mode dropdown
def change_mode(attr, old, new):
    mode = str(mode_dropdown.value)
    if debug_print: print("Mode selected: " + str(mode_dropdown.value))
    global dynamic_interface
    error_message.text = ""

    if mode == "Off":
        mc.setOperationMode(mcontrol.MotorOpMode.kModeOff)
        speed_Kp_input.disabled = True
        speed_Ki_input.disabled = True
        torque_Kp_input.disabled = True
        torque_Ki_input.disabled = True
        flux_Kp_input.disabled = True
        flux_Ki_input.disabled = True
    elif mode == "Speed":
        mc.setOperationMode(mcontrol.MotorOpMode.kModeSpeed)
        speed_setpoint = mc.getSpeedSetpoint()
        speed_setpoint_input.value = str(speed_setpoint)
        speed_Kp_input.disabled = False
        speed_Ki_input.disabled = False
        torque_Kp_input.disabled = False
        torque_Ki_input.disabled = False
        flux_Kp_input.disabled = False
        flux_Ki_input.disabled = False
    elif mode == "Torque":
        mc.setOperationMode(mcontrol.MotorOpMode.kModeTorque)
        torque_setpoint = mc.getTorqueSetpoint()
        torque_setpoint_input.value = str(torque_setpoint)
        speed_Kp_input.disabled = False
        speed_Ki_input.disabled = False
        torque_Kp_input.disabled = False
        torque_Ki_input.disabled = False
        flux_Kp_input.disabled = False
        flux_Ki_input.disabled = False
    elif mode == "Open Loop":
        mc.setOperationMode(mcontrol.MotorOpMode.kModeOpenLoop)
        speed_Kp_input.disabled = True
        speed_Ki_input.disabled = True
        torque_Kp_input.disabled = True
        torque_Ki_input.disabled = True
        flux_Kp_input.disabled = True
        flux_Ki_input.disabled = True

current_mode = mc.getOperationMode()
if current_mode == mcontrol.MotorOpMode.kModeSpeed:
    current_mode_value = "Speed"
elif current_mode == mcontrol.MotorOpMode.kModeTorque:
    current_mode_value = "Torque"
elif current_mode == mcontrol.MotorOpMode.kModeOpenLoop:
    current_mode_value = "Open Loop"
else:
    current_mode_value = "Off"

mode_dropdown_title = Paragraph(text="Mode:", width=40, align="center")
mode_dropdown = Select(
    value=current_mode_value,
    options=["Off", "Speed", "Torque", "Open Loop"],
    width=150,
)
mode_dropdown.on_change('value', change_mode)

# Speed setpoint
def update_speed_setpoint(attr, old, new):
    global speed_setpoint
    if float(new) >= speed_setpoint_min and float(new) <= speed_setpoint_max:
        speed_setpoint = float(new)
        if debug_print: print("New speed_setpoint is: " + str(speed_setpoint))
        mc.setSpeed(speed_setpoint)
        error_message.text = ""
    else:
        speed_setpoint_input.value = str(speed_setpoint)
        error_message.text = "Error: Invalid input. Speed setpoint must be between " + str(speed_setpoint_min) + " and " + str(speed_setpoint_max) + "."

speed_setpoint_title = Paragraph(text="Speed Setpoint:", width=150, align="center")
speed_setpoint_input = TextInput(value=str(speed_setpoint), width=180)
speed_setpoint_input.on_change('value', update_speed_setpoint)

# Torque setpoint
def update_torque_setpoint(attr, old, new):
    global torque_setpoint
    if float(new) >= torque_setpoint_min and float(new) <= torque_setpoint_max:
        torque_setpoint = float(new)
        if debug_print: print("New torque_setpoint is: " + str(torque_setpoint))
        mc.setTorque(torque_setpoint)
        error_message.text = ""
    else:
        torque_setpoint_input.value = str(torque_setpoint)
        error_message.text = "Error: Invalid input. Torque setpoint must be between " + str(torque_setpoint_min) + " and " + str(torque_setpoint_max) + "."

torque_setpoint_title = Paragraph(text="Torque Setpoint:", width=150, align="center")
torque_setpoint_input = TextInput(value=str(torque_setpoint), width=180)
torque_setpoint_input.on_change('value', update_torque_setpoint)

# Open loop - Vd
def update_open_loop_vd(attr, old, new):
    global open_loop_vd
    open_loop_vd = float(new)
    mc.setVfParamVd(open_loop_vd)

open_loop_vd_title = Paragraph(text="Open Loop - Vd:", width=150, align="center")
open_loop_vd_input = TextInput(value=str(open_loop_vd), width=180)
open_loop_vd_input.on_change('value', update_open_loop_vd)

# Open loop - Vq
def update_open_loop_vq(attr, old, new):
    global open_loop_vq
    open_loop_vq = float(new)
    mc.setVfParamVq(open_loop_vq)

open_loop_vq_title = Paragraph(text="Open Loop - Vq:", width=150, align="center")
open_loop_vq_input = TextInput(value=str(open_loop_vq), width=180)
open_loop_vq_input.on_change('value', update_open_loop_vq)

# Gain parameters
def update_speed_Kp(attr, old, new):
    global speed_gain
    speed_gain.kp = float(new)
    if debug_print: print("New speed_Kp is: " + str(speed_gain.kp))
    mc.setGain(mcontrol.GainType.kSpeed, speed_gain)

speed_Kp_title = Paragraph(text="Speed Kp:", width=70, align="center")
speed_Kp_input = TextInput(value=str(speed_gain.kp), width=180)
speed_Kp_input.on_change('value', update_speed_Kp)
speed_Kp_input.disabled = True

def update_speed_Ki(attr, old, new):
    global speed_gain
    speed_gain.ki = float(new)
    if debug_print: print("New speed_Ki is: " + str(speed_gain.ki))
    mc.setGain(mcontrol.GainType.kSpeed, speed_gain)

speed_Ki_title = Paragraph(text="Speed Ki:", width=70, align="center")
speed_Ki_input = TextInput(value=str(speed_gain.ki), width=180)
speed_Ki_input.on_change('value', update_speed_Ki)
speed_Ki_input.disabled = True

def update_torque_Kp(attr, old, new):
    global torque_gain
    torque_gain.kp = float(new)
    if debug_print: print("New torque_Kp is: " + str(torque_gain.kp))
    mc.setGain(mcontrol.GainType.kCurrent, torque_gain)

torque_Kp_title = Paragraph(text="Torque Kp:", width=70, align="center")
torque_Kp_input = TextInput(value=str(torque_gain.kp), width=180)
torque_Kp_input.on_change('value', update_torque_Kp)
torque_Kp_input.disabled = True

def update_torque_Ki(attr, old, new):
    global torque_gain
    torque_gain.ki = float(new)
    if debug_print: print("New torque_Ki is: " + str(torque_gain.ki))
    mc.setGain(mcontrol.GainType.kCurrent, torque_gain)

torque_Ki_title = Paragraph(text="Torque Ki:", width=70, align="center")
torque_Ki_input = TextInput(value=str(torque_gain.ki), width=180)
torque_Ki_input.on_change('value', update_torque_Ki)
torque_Ki_input.disabled = True

def update_flux_Kp(attr, old, new):
    global flux_gain
    flux_gain.kp = float(new)
    if debug_print: print("New flux_Kp is: " + str(flux_gain.kp))
    mc.setGain(mcontrol.GainType.kFlux, flux_gain)

flux_Kp_title = Paragraph(text="Flux Kp:", width=70, align="center")
flux_Kp_input = TextInput(value=str(flux_gain.kp), width=180)
flux_Kp_input.on_change('value', update_flux_Kp)
flux_Kp_input.disabled = True

def update_flux_Ki(attr, old, new):
    global flux_gain
    flux_gain.ki = float(new)
    if debug_print: print("New flux_Ki is: " + str(flux_gain.ki))
    mc.setGain(mcontrol.GainType.kFlux, flux_gain)

flux_Ki_title = Paragraph(text="Flux Ki:", width=70, align="center")
flux_Ki_input = TextInput(value=str(flux_gain.ki), width=180)
flux_Ki_input.on_change('value', update_flux_Ki)
flux_Ki_input.disabled = True

# Clear Faults button
def clear_faults():
    mc.clearFaults()

clear_faults_button = Button(label="Clear Faults", width=100, button_type='primary')
clear_faults_button.on_click(clear_faults)

# Error message
error_message = Paragraph(text="", style={'color': 'red'}, width=230, align="center")

@linear()
def update(step):
    plot_data[0] = mc.getCurrent(mcontrol.ElectricalData.kPhaseA)
    plot_data[1] = mc.getCurrent(mcontrol.ElectricalData.kPhaseB)
    plot_data[2] = mc.getCurrent(mcontrol.ElectricalData.kPhaseC)
    plot_data[3] = mc.getPosition()
    plot_data[4] = mc.getVoltage(mcontrol.ElectricalData.kPhaseA)
    plot_data[5] = mc.getVoltage(mcontrol.ElectricalData.kPhaseB)
    plot_data[6] = mc.getVoltage(mcontrol.ElectricalData.kPhaseC)
    plot_data[7] = mc.getSpeed()
    foc_data = mc.getFocCalc()
    plot_data[8] = foc_data.i_d
    plot_data[9] = foc_data.i_q
    plot_data[10] = foc_data.torque
    plot_data[11] = foc_data.speed

    global time
    global sample_size_actual
    if sample_size_actual >= sample_size:
        x.popleft()
    x.append(time)
    time = time + interval

    for i in range(len(data_list)):
        if sample_size_actual >= sample_size:
            data_list[i].popleft()
        val_read = plot_data[i]
        data_list[i].append(val_read)
        ds_list[i].trigger('data', x, data_list[i])

    if sample_size_actual < sample_size:
        sample_size_actual = sample_size_actual + 1

# margin:  Margin-Top, Margin-Right, Margin-Bottom and Margin-Left
mode_interface = row(mode_dropdown_title, mode_dropdown, margin=(30, 30, 30, 30))

plot_controls_interface = column(
    row(sample_size_title, sample_size_input),
    row(interval_title, interval_input),
    margin=(30, 30, 30, 30)
)

gain_controls_interface = column(
    row(speed_Kp_title, speed_Kp_input),
    row(speed_Ki_title, speed_Ki_input),
    row(torque_Kp_title, torque_Kp_input),
    row(torque_Ki_title, torque_Ki_input),
    row(flux_Kp_title, flux_Kp_input),
    row(flux_Ki_title, flux_Ki_input),
    margin=(30, 30, 30, 30)
)

dynamic_interface = column(
    row(speed_setpoint_title, speed_setpoint_input),
    row(torque_setpoint_title, torque_setpoint_input),
    row(open_loop_vd_title, open_loop_vd_input),
    row(open_loop_vq_title, open_loop_vq_input),
    error_message,
    margin=(30, 30, 30, 30)
)

fault_interface = column(clear_faults_button, margin=(30, 30, 30, 30))

layout1 = layout(
    column(row(title1, align='center'),
    row(
        mode_interface,
        plot_controls_interface,
        gain_controls_interface,
        dynamic_interface,
        fault_interface
    ),
    row(plot_list[0], plot_list[1], plot_list[2], plot_list[3]),
    row(plot_list[4], plot_list[5], plot_list[6], plot_list[7]),
    row(plot_list[8], plot_list[9], plot_list[10], plot_list[11]),
    css_style)
)

# Add a periodic callback to be run every interval*1000 milliseconds
callback = curdoc().add_periodic_callback(update, interval * 1000)

curdoc().theme = 'dark_minimal'
curdoc().add_root(layout1)
