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

# Known issues

* Application does not allow users to set negative speed set point.
* QEI reported speed is jumping around. The QEI presently reports an unfiltered
  value for speed and needs a low-pass filter to eliminate non-significant bit
  count changes from the raw encoder interface.
* Application deb package does not automatically install dependencies
* Handle Q-scaling in FOC driver for read operations (16-bit precision)
* Motor is not spinning with low rpms below ~250 (depends on motor)
* Rotor lock over-current protection not functional. If rotor is mechanically
  locked for a sustained period of time, it can cause damage to the motor due
  to lack of over-current protection. Therefore do not intentionally lock rotor
  mechanically.
* Incorrect RPM is reported with negative torque set point in Torque mode.

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