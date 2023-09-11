<table class="sphinxhide">
 <tr>
   <td align="center"><img src="../../media/xilinx-logo.png" width="30%"/><h1> Kria&trade; KD240 Drives Starter Kit <br>FOC Motor Control Application Tutorial</h1>
   </td>
 </tr>
 <tr>
 <td align="center"><h1>Known Issues</h1>

 </td>
 </tr>
</table>

# Known issues & limitations

* QEI reported speed is showing glitches. QEI library computation of the speed 
has a known issue and will be addressed with a QEI library update. 
* Motor lower speed mode control capability is apprxomately 250 rpms. 
* Default motor tuning values are provided for the KD240 Motor Accessory Kit 
based the Anaheim motor with the plastic disk visual load. This load disk 
primarily acts as an inertial load and thus if users are connecting other loads
to their motor they should plan on conducting their own tuning process and 
defining custom tuning defaults based on their load testing.
* Open-loop control mode is intended as a test mode and thus does not implement
any SW based ramp control. Users will see fault protections triggered if immediately
trying to set open-loop mode control with a Vq voltage > ~10V. If using open-loop
user is responsible for incrementally stepping up the Vq command to their desired 
voltage setting.  Typical use-cases of open-loop mode have Vd set to 0V.

<!---

Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License.

You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

-->

<p class="sphinxhide" align="center">Copyright&copy; 2023 Advanced Micro Devices, Inc</p>
