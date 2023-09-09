#!/usr/bin/python3
#
# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT
#

import py_foc_motor_ctrl as mcontrol
from bokeh.plotting import figure, curdoc
from bokeh.layouts import layout, row, column
from bokeh.models import Range1d, DataRange1d
from bokeh.models import ColumnDataSource
from bokeh.models import LinearAxis
from bokeh.models import Select
from bokeh.models import TextInput
from bokeh.models import Paragraph, Div
from bokeh.models import Button
from bokeh.models import Label
from bokeh.models import RadioGroup
from bokeh.driving import linear
from numpy import nan
from collections import deque

css_style = Div(text="""
<style>
    body {
        color: #E0E0E0 !important;
        background-color: #15191C !important;
    }
</style>
""")

sample_size = 500
interval = 2
x = deque([nan] * sample_size)
for i in range(sample_size):
    x[i] = i+1

# Get a MotorControl instance with session ID 1 and default config path
mc = mcontrol.MotorControl.getMotorControlInstance(1)
if mc is None:
    print("Error: Unable to get MotorControl instance.\nPlease check your motor setup and restart the server.")
    exit()
# Initialize parameters
time = 0
sample_size_min = 2
sample_size_max = 3000
speed_setpoint_min = 250
speed_setpoint_max = 10000
torque_setpoint_min = -2.5
torque_setpoint_max = 2.5
open_loop_vd_min = 0
open_loop_vd_max = 24
open_loop_vq_min = 0
open_loop_vq_max = 24

speed_setpoint = round(mc.getSpeedSetpoint(), 5)
torque_setpoint = round(mc.getTorqueSetpoint(), 5)
open_loop_vd = round(mc.getVfParamVd(), 5)
open_loop_vq = round(mc.getVfParamVq(), 5)
speed_gain = mc.GetGain(mcontrol.GainType.kSpeed)
torque_gain = mc.GetGain(mcontrol.GainType.kCurrent)
flux_gain = mc.GetGain(mcontrol.GainType.kFlux)

# title
title1 = Div(
    text="<h1>Kria&trade; SOM: FOC Motor Dashboard</h1>",
    width=500
)

color_list = ["darkseagreen", "steelblue", "indianred", "chocolate", "gold", "mediumaquamarine"]

# Electrical Plot
electrical_data_titles = [
    'Phase A Current',
    'Phase B Current',
    'Phase C Current',
    'Phase A Voltage',
    'Phase B Voltage',
    'Phase C Voltage'
]
num_electrical_data = len(electrical_data_titles)
electrical_data_list = [deque([nan] * sample_size) for i in range(num_electrical_data)]
electrical_plot = figure(plot_width=1000, plot_height=300, title='Electrical Data')
electrical_lines = [0] * num_electrical_data
electrical_ds_list = [0] * num_electrical_data
electrical_plot.xaxis.axis_label = "Sample"
electrical_plot.yaxis.visible = False
electrical_plot.extra_y_ranges["Current"] = DataRange1d(only_visible=True)
electrical_plot.add_layout(LinearAxis(y_range_name="Current", axis_label="Current (mA)"), 'left')
electrical_plot.extra_y_ranges["Voltage"] = DataRange1d(only_visible=True)
electrical_plot.add_layout(LinearAxis(y_range_name="Voltage", axis_label="Voltage (V)"), 'right')

for i in range(num_electrical_data):
    electrical_lines[i] = electrical_plot.line(x, electrical_data_list[i], line_width=2, color=color_list[i], legend_label=electrical_data_titles[i], y_range_name=electrical_data_titles[i][8:])

electrical_plot.extra_y_ranges["Current"].renderers = [electrical_lines[0], electrical_lines[1], electrical_lines[2]]
electrical_plot.extra_y_ranges["Voltage"].renderers = [electrical_lines[3], electrical_lines[4], electrical_lines[5]]
electrical_plot.legend.click_policy = "hide"
electrical_plot.add_layout(electrical_plot.legend[0], 'right')

for i in range(num_electrical_data):
    electrical_ds_list[i] = electrical_lines[i].data_source

# Hide voltage lines by default
electrical_lines[3].visible = False
electrical_lines[4].visible = False
electrical_lines[5].visible = False

# Mechanical Plot
mechanical_data_titles = [
    'Motor Speed',
    'Motor Position'
]
num_mechanical_data = len(mechanical_data_titles)
mechanical_data_list = [deque([nan] * sample_size) for i in range(num_mechanical_data)]
mechanical_plot = figure(plot_width=1000, plot_height=300, title='Mechanical Data')
mechanical_lines = [0] * num_mechanical_data
mechanical_ds_list = [0] * num_mechanical_data
mechanical_plot.xaxis.axis_label = "Sample"
mechanical_plot.yaxis.visible = False
mechanical_plot.extra_y_ranges["Speed"] = DataRange1d(only_visible=True)
mechanical_plot.add_layout(LinearAxis(y_range_name="Speed", axis_label="Speed (rpm)"), 'left')
mechanical_plot.extra_y_ranges["Position"] = DataRange1d(only_visible=True)
mechanical_plot.add_layout(LinearAxis(y_range_name="Position", axis_label="Position (degrees)"), 'right')

for i in range(num_mechanical_data):
    mechanical_lines[i] = mechanical_plot.line(x, mechanical_data_list[i], line_width=2, color=color_list[i], legend_label=mechanical_data_titles[i], y_range_name=mechanical_data_titles[i][6:])

mechanical_plot.extra_y_ranges["Speed"].renderers = [mechanical_lines[0]]
mechanical_plot.extra_y_ranges["Position"].renderers = [mechanical_lines[1]]
mechanical_plot.legend.click_policy = "hide"
mechanical_plot.add_layout(mechanical_plot.legend[0], 'right')

for i in range(num_mechanical_data):
    mechanical_ds_list[i] = mechanical_lines[i].data_source

# Live Analysis Plot
live_analysis_options = [
    "PhA Current",
    "PhB Current",
    "PhC Current",
    "I_d",
    "I_q",
    "I_alpha",
    "I_beta",
    "PhA Voltage",
    "PhB Voltage",
    "PhC Voltage",
    "Speed",
    "Position"
]
live_analysis_axis_labels = {
    "PhA Current": "Phase A Current (mA)",
    "PhB Current": "Phase B Current (mA)",
    "PhC Current": "Phase C Current (mA)",
    "I_d": "Direct Current (mA)",
    "I_q": "Quadrature Current (mA)",
    "I_alpha": "I_alpha (mA)",
    "I_beta": "I_beta (mA)",
    "PhA Voltage": "Phase A Voltage (V)",
    "PhB Voltage": "Phase B Voltage (V)",
    "PhC Voltage": "Phase C Voltage (V)",
    "Speed": "Speed (rpm)",
    "Position": "Position (degrees)"
}
live_analysis_x_selection = live_analysis_options[0]
live_analysis_y_selection = live_analysis_options[1]

def live_analysis_x_select_data(self):
    global live_analysis_x_data_list
    global live_analysis_x_selection
    global live_analysis_x_buffer_mapping
    live_analysis_x_selection = live_analysis_options[live_analysis_x_options.active]
    live_analysis_plot.xaxis.axis_label = live_analysis_axis_labels[live_analysis_x_selection]
    live_analysis_x_buffer_mapping = get_live_analysis_buffer_mapping(live_analysis_x_selection)

def live_analysis_y_select_data(self):
    global live_analysis_y_data_list
    global live_analysis_y_selection
    global live_analysis_y_buffer_mapping
    live_analysis_y_selection = live_analysis_options[live_analysis_y_options.active]
    live_analysis_plot.yaxis.axis_label = live_analysis_axis_labels[live_analysis_y_selection]
    live_analysis_y_buffer_mapping = get_live_analysis_buffer_mapping(live_analysis_y_selection)

live_analysis_x_title = Paragraph(text="X-Axis Data:", width=80, align="start")
live_analysis_x_options = RadioGroup(labels=live_analysis_options, active=0)
live_analysis_x_options.on_click(live_analysis_x_select_data)
live_analysis_y_title = Paragraph(text="Y-Axis Data:", width=80, align="start")
live_analysis_y_options = RadioGroup(labels=live_analysis_options, active=1)
live_analysis_y_options.on_click(live_analysis_y_select_data)

def get_live_analysis_buffer_mapping(val):
    if val == "PhA Current":
        return mcontrol.MotorParam.kCurrentPhaseA
    elif val == "PhB Current":
        return mcontrol.MotorParam.kCurrentPhaseB
    elif val == "PhC Current":
        return mcontrol.MotorParam.kCurrentPhaseC
    elif val == "I_d":
        return mcontrol.MotorParam.kId
    elif val == "I_q":
        return mcontrol.MotorParam.kIq
    elif val == "I_alpha":
        return mcontrol.MotorParam.kIalpha
    elif val == "I_beta":
        return mcontrol.MotorParam.kIbeta
    elif val == "PhA Voltage":
        return mcontrol.MotorParam.kVoltagePhaseA
    elif val == "PhB Voltage":
        return mcontrol.MotorParam.kVoltagePhaseB
    elif val == "PhC Voltage":
        return mcontrol.MotorParam.kVoltagePhaseC
    elif val == "Speed":
        return mcontrol.MotorParam.kRpm
    elif val == "Position":
        return mcontrol.MotorParam.kPosition
    else:
        return 0

live_analysis_x_buffer_mapping = get_live_analysis_buffer_mapping(live_analysis_x_selection)
live_analysis_y_buffer_mapping = get_live_analysis_buffer_mapping(live_analysis_y_selection)
live_analysis_x_data_list = deque([nan] * sample_size)
live_analysis_y_data_list = deque([nan] * sample_size)

live_analysis_plot = figure(plot_width=800, plot_height=620, title="Live Analysis")
live_analysis_source = ColumnDataSource(dict(x=live_analysis_x_data_list, y=live_analysis_y_data_list))
live_analysis_scatter = live_analysis_plot.scatter(
    x="x",
    y="y",
    size=20,
    marker="circle",
    color="red",
    source=live_analysis_source
    )
live_analysis_ds = live_analysis_scatter.data_source
live_analysis_plot.xaxis.axis_label = live_analysis_axis_labels[live_analysis_x_selection]
live_analysis_plot.yaxis.axis_label = live_analysis_axis_labels[live_analysis_y_selection]

# Fault Status Indicators
fault_status_plot_x = [1, 1, 1, 1, 2, 2, 2, 2]
fault_status_plot_y = [4, 3, 2, 1, 4, 3, 2, 1]
fault_list = [
    mcontrol.FaultId.kPhaseA_OC,
    mcontrol.FaultId.kPhaseB_OC,
    mcontrol.FaultId.kPhaseC_OC,
    mcontrol.FaultId.kPhaseImbalance,
    mcontrol.FaultId.kDCLink_OC,
    mcontrol.FaultId.kDCLink_OV,
    mcontrol.FaultId.kDCLink_UV,
    mcontrol.FaultId.kAvgPowerFault
]
fault_labels = [
    "PhaseA_OC",
    "PhaseB_OC",
    "PhaseC_OC",
    "PhaseImbalance",
    "DCLink_OC",
    "DCLink_OV",
    "DCLink_UV",
    "AvgPowerFault"
]

fault_colors = [""] * len(fault_labels)
for i in range(len(fault_list)):
    if i==3 or i==6: # Show PhaseImbalance and DCLink_UV as yellow (warning)
        fault_colors[i] = "yellow" if mc.getFaultStatus(fault_list[i]) else "green"
    else:
        if mc.getFaultStatus(fault_list[i]):
            fault_colors[i] = "red"
            mode_dropdown.disabled = True
        else:
            fault_colors[i] = "green"

fault_status_plot = figure(plot_width=400, plot_height=200, title='Fault Status')
fault_status_plot.grid.visible = False
fault_status_plot.axis.visible = False
fault_status_plot.x_range = Range1d(0.8, 3)
fault_status_plot.y_range = Range1d(0.5, 4.5)
fault_status_plot.toolbar.active_drag = None
fault_status_plot.toolbar.active_scroll = None
fault_status_plot.toolbar.active_tap = None
fault_status_plot.toolbar.logo = None
fault_status_plot.toolbar_location = None

for i in range(len(fault_labels)):
    mytext = Label(
        x = fault_status_plot_x[i]+0.15,
        y = fault_status_plot_y[i]-0.18,
        text = fault_labels[i],
        text_color = '#E0E0E0',
        text_font_size = '14px')
    fault_status_plot.add_layout(mytext)

fault_status_ds = fault_status_plot.circle(fault_status_plot_x, fault_status_plot_y, radius=0.08, color=fault_colors).data_source

# Fault Status Callback
def update_fault_status():
    for i in range(len(fault_list)):
        if i==3 or i==6: # Show PhaseImbalance and DCLink_UV as yellow (warning)
            fault_colors[i] = "yellow" if mc.getFaultStatus(fault_list[i]) else "green"
        else:
            if mc.getFaultStatus(fault_list[i]):
                fault_colors[i] = "red"
                mode_dropdown.disabled = True
            else:
                fault_colors[i] = "green"
    fault_status_ds.trigger('data', fault_colors, fault_colors)

fault_status_callback_interval = 1000 #milliseconds
fault_status_callback = curdoc().add_periodic_callback(update_fault_status, fault_status_callback_interval)

def is_numeric(val):
    try:
        float(val)
        return True
    except ValueError:
        return False

def get_min_interval(samples):
    if samples <= 500:
        return 1
    elif samples <= 1000:
        return 2
    elif samples <= 2000:
        return 3
    elif samples <= 2500:
        return 4
    else:
        return 5

# sample interval
def update_interval(attr, old, new):
    global interval, sample_size
    if new == "" or is_numeric(new) == False:
        interval_input.value = old
        return
    interval = max(round(float(new), 5), get_min_interval(sample_size))
    interval_input.value = str(interval)

    global callback
    curdoc().remove_periodic_callback(callback)
    callback = curdoc().add_periodic_callback(update, interval * 1000)

interval_title = Paragraph(text="Refresh Interval (Seconds):", width=170, align="center")
interval_input = TextInput(value=str(interval), width=80)
interval_input.on_change('value', update_interval)

# sample size
def update_sample_size(attr, old, new):
    global sample_size, x, interval
    if new == "":
        sample_size_input.value = old
        return
    if is_numeric(new) and int(new) >= sample_size_min and int(new) <= sample_size_max:
        sample_size = int(new)
        plot_error_message.text = ""
        interval = max(interval, get_min_interval(sample_size))
        interval_input.value = str(interval)
    else:
        sample_size_input.value = str(sample_size)
        plot_error_message.text = "Error: Invalid input. Sample size must be between " + str(sample_size_min) + " and " + str(sample_size_max) + "."

    while len(live_analysis_x_data_list) > sample_size:
        x.pop()
        live_analysis_x_data_list.popleft()
        live_analysis_y_data_list.popleft()
        for data in electrical_data_list:
            data.popleft()
        for data in mechanical_data_list:
            data.popleft()

    while len(live_analysis_x_data_list) < sample_size:
        x.append(len(x)+1)
        live_analysis_x_data_list.append(0)
        live_analysis_y_data_list.append(0)
        for data in electrical_data_list:
            data.append(0)
        for data in mechanical_data_list:
            data.append(0)

sample_size_title = Paragraph(text="Sample Size (Samples):", width=170, align="center")
sample_size_input = TextInput(value=str(sample_size), width=80)
sample_size_input.on_change('value', update_sample_size)

# Mode dropdown
def change_mode(attr, old, new):
    mode = str(mode_dropdown.value)
    global dynamic_interface
    error_message.text = ""

    if mode == "Off":
        mc.setOperationMode(mcontrol.MotorOpMode.kModeOff)
    elif mode == "Speed":
        mc.setOperationMode(mcontrol.MotorOpMode.kModeSpeed)
        speed_setpoint = mc.getSpeedSetpoint()
        speed_setpoint_input.value = str(speed_setpoint)
    elif mode == "Torque":
        mc.setOperationMode(mcontrol.MotorOpMode.kModeTorque)
        torque_setpoint = mc.getTorqueSetpoint()
        torque_setpoint_input.value = str(torque_setpoint)
    elif mode == "Open Loop":
        mc.setOperationMode(mcontrol.MotorOpMode.kModeOpenLoop)

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
    width=120,
)
mode_dropdown.on_change('value', change_mode)

# Speed setpoint
def update_speed_setpoint(attr, old, new):
    global speed_setpoint
    if new != "" and is_numeric(new) and abs(float(new)) >= speed_setpoint_min and abs(float(new)) <= speed_setpoint_max:
        speed_setpoint = round(float(new), 5)
        mc.setSpeed(speed_setpoint)
        error_message.text = ""
        speed_setpoint_input.value = str(speed_setpoint)
    else:
        speed_setpoint_input.value = str(speed_setpoint)
        error_message.text = "Error: Invalid input. Speed setpoint must be between " + str(speed_setpoint_min) + " and " + str(speed_setpoint_max) + " or -" + str(speed_setpoint_min) + " and -" + str(speed_setpoint_max) + "."

speed_setpoint_title = Paragraph(text="Speed Setpoint:", width=110, align="center")
speed_setpoint_input = TextInput(value=str(speed_setpoint), width=80)
speed_setpoint_input.on_change('value', update_speed_setpoint)

# Torque setpoint
def update_torque_setpoint(attr, old, new):
    global torque_setpoint
    if new != "" and is_numeric(new) and float(new) >= torque_setpoint_min and float(new) <= torque_setpoint_max:
        torque_setpoint = round(float(new), 5)
        mc.setTorque(torque_setpoint)
        error_message.text = ""
        torque_setpoint_input.value = str(torque_setpoint)
    else:
        torque_setpoint_input.value = str(torque_setpoint)
        error_message.text = "Error: Invalid input. Torque setpoint must be between " + str(torque_setpoint_min) + " and " + str(torque_setpoint_max) + "."

torque_setpoint_title = Paragraph(text="Torque Setpoint:", width=110, align="center")
torque_setpoint_input = TextInput(value=str(torque_setpoint), width=80)
torque_setpoint_input.on_change('value', update_torque_setpoint)

# Open loop - Vd
def update_open_loop_vd(attr, old, new):
    global open_loop_vd
    if new != "" and is_numeric(new) and float(new) >= open_loop_vd_min and float(new) <= open_loop_vd_max:
        open_loop_vd = round(float(new), 5)
        mc.setVfParamVd(open_loop_vd)
        error_message.text = ""
        open_loop_vd_input.value = str(open_loop_vd)
    else:
        open_loop_vd_input.value = str(open_loop_vd)
        error_message.text = "Error: Invalid input. Open Loop - Vd must be between " + str(open_loop_vd_min) + " and " + str(open_loop_vd_max) + "."

open_loop_vd_title = Paragraph(text="Open Loop - Vd:", width=110, align="center")
open_loop_vd_input = TextInput(value=str(open_loop_vd), width=80)
open_loop_vd_input.on_change('value', update_open_loop_vd)

# Open loop - Vq
def update_open_loop_vq(attr, old, new):
    global open_loop_vq
    if new != "" and is_numeric(new) and float(new) >= open_loop_vq_min and float(new) <= open_loop_vq_max:
        open_loop_vq = round(float(new), 5)
        mc.setVfParamVq(open_loop_vq)
        error_message.text = ""
        open_loop_vq_input.value = str(open_loop_vq)
    else:
        open_loop_vq_input.value = str(open_loop_vq)
        error_message.text = "Error: Invalid input. Open Loop - Vq must be between " + str(open_loop_vq_min) + " and " + str(open_loop_vq_max) + "."

open_loop_vq_title = Paragraph(text="Open Loop - Vq:", width=110, align="center")
open_loop_vq_input = TextInput(value=str(open_loop_vq), width=80)
open_loop_vq_input.on_change('value', update_open_loop_vq)

# Gain parameters
def update_speed_Kp(attr, old, new):
    global speed_gain
    if new != "" and is_numeric(new):
        speed_gain.kp = round(float(new), 5)
        mc.setGain(mcontrol.GainType.kSpeed, speed_gain)
    speed_Kp_input.value = str(round(speed_gain.kp, 5))

speed_Kp_title = Paragraph(text="Speed Kp:", width=70, align="center")
speed_Kp_input = TextInput(value=str(round(speed_gain.kp, 5)), width=80)
speed_Kp_input.on_change('value', update_speed_Kp)

def update_speed_Ki(attr, old, new):
    global speed_gain
    if new != "" and is_numeric(new):
        speed_gain.ki = round(float(new), 5)
        mc.setGain(mcontrol.GainType.kSpeed, speed_gain)
    speed_Ki_input.value = str(round(speed_gain.ki, 5))

speed_Ki_title = Paragraph(text="Speed Ki:", width=70, align="center")
speed_Ki_input = TextInput(value=str(round(speed_gain.ki, 5)), width=80)
speed_Ki_input.on_change('value', update_speed_Ki)

def update_torque_Kp(attr, old, new):
    global torque_gain
    if new != "" and is_numeric(new):
        torque_gain.kp = round(float(new), 5)
        mc.setGain(mcontrol.GainType.kCurrent, torque_gain)
    torque_Kp_input.value = str(round(torque_gain.kp, 5))

torque_Kp_title = Paragraph(text="Torque Kp:", width=70, align="center")
torque_Kp_input = TextInput(value=str(round(torque_gain.kp, 5)), width=80)
torque_Kp_input.on_change('value', update_torque_Kp)

def update_torque_Ki(attr, old, new):
    global torque_gain
    if new != "" and is_numeric(new):
        torque_gain.ki = round(float(new), 5)
        mc.setGain(mcontrol.GainType.kCurrent, torque_gain)
    torque_Ki_input.value = str(round(torque_gain.ki, 5))

torque_Ki_title = Paragraph(text="Torque Ki:", width=70, align="center")
torque_Ki_input = TextInput(value=str(round(torque_gain.ki, 5)), width=80)
torque_Ki_input.on_change('value', update_torque_Ki)

def update_flux_Kp(attr, old, new):
    global flux_gain
    if new != "" and is_numeric(new):
        flux_gain.kp = round(float(new), 5)
        mc.setGain(mcontrol.GainType.kFlux, flux_gain)
    flux_Kp_input.value = str(round(flux_gain.kp, 5))

flux_Kp_title = Paragraph(text="Flux Kp:", width=70, align="center")
flux_Kp_input = TextInput(value=str(round(flux_gain.kp, 5)), width=80)
flux_Kp_input.on_change('value', update_flux_Kp)

def update_flux_Ki(attr, old, new):
    global flux_gain
    if new != "" and is_numeric(new):
        flux_gain.ki = round(float(new), 5)
        mc.setGain(mcontrol.GainType.kFlux, flux_gain)
    flux_Ki_input.value = str(round(flux_gain.ki, 5))

flux_Ki_title = Paragraph(text="Flux Ki:", width=70, align="center")
flux_Ki_input = TextInput(value=str(round(flux_gain.ki, 5)), width=80)
flux_Ki_input.on_change('value', update_flux_Ki)

# Clear Faults button
def clear_faults():
    mc.clearFaults()
    mode_dropdown.value = "Off"
    mode_dropdown.disabled = False
    update_fault_status()

clear_faults_button = Button(label="Clear Faults", width=100, button_type='primary')
clear_faults_button.on_click(clear_faults)

# Error message (plotting)
plot_error_message = Paragraph(text="", style={'color': 'red'}, width=250, align="center")

# Error message (setpoints)
error_message = Paragraph(text="", style={'color': 'red'}, width=290, align="center")

# List of buffered data that will be requested
buffer_list = [
    mcontrol.MotorParam.kCurrentPhaseA,
    mcontrol.MotorParam.kCurrentPhaseB,
    mcontrol.MotorParam.kCurrentPhaseC,
    mcontrol.MotorParam.kVoltagePhaseA,
    mcontrol.MotorParam.kVoltagePhaseB,
    mcontrol.MotorParam.kVoltagePhaseC,
    mcontrol.MotorParam.kRpm,
    mcontrol.MotorParam.kPosition,
    mcontrol.MotorParam.kId,
    mcontrol.MotorParam.kIq,
    mcontrol.MotorParam.kIalpha,
    mcontrol.MotorParam.kIbeta
]

@linear()
def update(step):
    buffer_data = mc.getMotorParams(sample_size, buffer_list)

    for i in range(sample_size):
        live_analysis_x_data_list[i] = buffer_data[live_analysis_x_buffer_mapping][i]
        live_analysis_y_data_list[i] = buffer_data[live_analysis_y_buffer_mapping][i]
    live_analysis_ds.trigger('data', live_analysis_ds, live_analysis_ds)

    for i in range(len(electrical_data_list)):
        for j in range(sample_size):
            electrical_data_list[i][j] = buffer_data[buffer_list[i]][j]
        electrical_ds_list[i].trigger('data', x, electrical_data_list[i])

    for i in range(len(mechanical_data_list)):
        for j in range(sample_size):
            mechanical_data_list[i][j] = buffer_data[buffer_list[i+6]][j]
        mechanical_ds_list[i].trigger('data', x, mechanical_data_list[i])

# margin:  Margin-Top, Margin-Right, Margin-Bottom and Margin-Left
mode_interface = row(mode_dropdown_title, mode_dropdown, margin=(30, 30, 30, 30))

plot_controls_interface = column(
    row(sample_size_title, sample_size_input),
    row(interval_title, interval_input),
    plot_error_message,
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

fault_interface = column(fault_status_plot, clear_faults_button, margin=(25, 30, 30, 30))

layout1 = layout(
    column(
        row(title1, align='center'),
        row(
            mode_interface,
            plot_controls_interface,
            gain_controls_interface,
            dynamic_interface,
            fault_interface
        ),
        row(
            column(
                row(electrical_plot, margin=(30, 30, 10, 30)),
                row(mechanical_plot, margin=(10, 30, 30, 30))
            ),
            row(live_analysis_plot, margin=(30, 30, 30, 30)),
            column(
                column(live_analysis_x_title, live_analysis_x_options, margin=(50, 5, 5, 5)),
                column(live_analysis_y_title, live_analysis_y_options, margin=(5, 5, 5, 5))
            )
        ),
        css_style
    )
)

# Add a periodic callback to be run every interval*1000 milliseconds
callback = curdoc().add_periodic_callback(update, interval * 1000)

curdoc().theme = 'dark_minimal'
curdoc().add_root(layout1)
