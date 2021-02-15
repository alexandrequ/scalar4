/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

SWITCH_ON_CONTROL_WRITE_STOP				0
SWITCH_ON_CONTROL_WRITE_START				1
SWITCH_ON_CONTROL_READ_NONE					0
SWITCH_ON_CONTROL_READ_STARTED				1
SWITCH_ON_CONTROL_READ_IN_PROGRESS			2
SWITCH_ON_CONTROL_READ_COMPLETED			3

SWITCH_OFF_CONTROL_WRITE_STOP				0
SWITCH_OFF_CONTROL_WRITE_START				1
SWITCH_OFF_CONTROL_READ_NONE				0
SWITCH_OFF_CONTROL_READ_STARTED				1
SWITCH_OFF_CONTROL_READ_IN_PROGRESS			2
SWITCH_OFF_CONTROL_READ_COMPLETED			3

TOGGLE_CONTROL_WRITE_STOP					0
TOGGLE_CONTROL_WRITE_START					1
TOGGLE_CONTROL_READ_NONE					0
TOGGLE_CONTROL_READ_STARTED					1
TOGGLE_CONTROL_READ_IN_PROGRESS				2
TOGGLE_CONTROL_READ_COMPLETED				3

TEST_WRITE_NO_TEST							0
TEST_WRITE_QUICK_TEST						1
TEST_WRITE_DEEP_TEST						2
TEST_WRITE_ABORT_TEST						3
TEST_READ_DONE_AND_PASSED					1
TEST_READ_DONE_AND_WARNING					2
TEST_READ_DONE_AND_ERROR					3
TEST_READ_ABORTED							4
TEST_READ_IN_PROGRESS						5
TEST_READ_NO_TEST_INITIATED					6

MODULE_RESET_WRITE_NO_RESET					0
MODULE_RESET_WRITE_RESET_MODULE				1
MODULE_RESET_WRITE_RESET_MODULES_ALARMS		2
MODULE_RESET_WRITE_RESET_MODULES_COUNTERS	3

AUDIBLE_ALARM_CONTROL_DISABLED				1
AUDIBLE_ALARM_CONTROL_ENABLED				2
AUDIBLE_ALARM_CONTROL_MUTED					3

//Battery System Usages section 4.2.1 of pdcv10.pdf
SMB_BATTERY_MODE_CAPACITY_MODE_MASK					0x8000
SMB_BATTERY_MODE_CHARGER_MODE_MASK					0x4000
SMB_BATTERY_MODE_PRIMARY_BATTERY_MASK				0x0200
SMB_BATTERY_MODE_CHARGER_CONTROLLER_ENABLED_MASK	0x0100
SMB_BATTERY_MODE_CONDITIONING_FLAG_MASK				0x0080
SMB_BATTERY_MODE_PRIMARY_BATTERY_SUPPORT_MASK		0x0002
SMB_BATTERY_MODE_INTERNAL_CHARGE_CONTROLLER_MASK	0x0001