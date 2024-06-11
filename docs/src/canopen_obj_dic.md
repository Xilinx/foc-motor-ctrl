# Object Dictionary Information

## Electronic Datasheet Information

    Version 1, revision 1
    FOC Motor Control Device

## Product Information

|Property                | Value
|------------------------|---------------------
|Vendor Name             | Advanced Micro Devices, Inc
|Vendor ID               | 0x00000586
|Product Name            | Kria SOM
|Product Code            | 0x00000013
|Revision Number         | 0x00000011
|Order Code              | -

## Commissioning Information

|Property|Value|
|---|---|
|Node ID|0x04|
|Name||
|Bitrate|1000 kbps|
|Network Number|0|
|Network Name||
|LSS Serial Number|0x00000000|
|CANopen Manager|No|

## PDO Configuration

### Communication Parameters

|PDO|COBID|Tx Type|Inhibit Time|Event Time|Sync Start|
|---|---|---|---|---|---|
|RPDO1|$NODEID+0x00000200|255||||
|RPDO2|$NODEID+0x00000300|255||||
|RPDO3|$NODEID+0x00000400|255||||
|RPDO4|$NODEID+0x00000500|255||||
|TPDO1|$NODEID+0x40000180|1|0.0 ms|0 ms|0|
|TPDO2|$NODEID+0x40000280|1|0.0 ms|0 ms|0|
|TPDO3|$NODEID+0x40000380|1|0.0 ms|0 ms|0|
|TPDO4|$NODEID+0x40000480|1|0.0 ms|0 ms|0|

### Mappings

|PDO|Mappings|
|---|---|
|RPDO1|[0x6040,0x00] Controlword (Unsigned16)|
|RPDO2|[0x6040,0x00] Controlword (Unsigned16)|
||[0x6060,0x00] Modes of Operation (Integer8)|
|RPDO3|[0x6040,0x00] Controlword (Unsigned16)|
||[0x60FF,0x00] Target Velocity (Integer32)|
|RPDO4|[0x6040,0x00] Controlword (Unsigned16)|
||[0x6071,0x00] Target Torque (Integer16)|
|TPDO1|[0x6041,0x00] Statusword (Unsigned16)|
|TPDO2|[0x6041,0x00] Statusword (Unsigned16)|
||[0x6061,0x00] Modes of Operation display (Integer8)|
|TPDO3|[0x6041,0x00] Statusword (Unsigned16)|
||[0x606C,0x00] Velocity Actual Value (Integer32)|
|TPDO4|[0x6041,0x00] Statusword (Unsigned16)|
||[0x6064,0x00] Position Actual Value (Integer32)|

## Object Dictionary

### Overview

|Index|Subindex|Name|Type|Access|Default Value|
|---|---|---|---|---|---|
|0x1000|0x00|Device type|U32|CO|0xFFFF0192|
|0x1001|0x00|Error register|U8|RO|0x00|
|0x1003|0x00|Pre-defined error field|U8|RW||
||0x01|Standard error field|U32|RO||
||0x02|Standard error field|U32|RO||
||0x03|Standard error field|U32|RO||
||0x04|Standard error field|U32|RO||
||0x05|Standard error field|U32|RO||
||0x06|Standard error field|U32|RO||
||0x07|Standard error field|U32|RO||
||0x08|Standard error field|U32|RO||
||0x09|Standard error field|U32|RO||
||0x0A|Standard error field|U32|RO||
||0x0B|Standard error field|U32|RO||
||0x0C|Standard error field|U32|RO||
||0x0D|Standard error field|U32|RO||
||0x0E|Standard error field|U32|RO||
||0x0F|Standard error field|U32|RO||
||0x10|Standard error field|U32|RO||
|0x1005|0x00|COB-ID SYNC message|U32|RW|0x00000080|
|0x1006|0x00|Communication cycle period|U32|RW|0|
|0x1007|0x00|Synchronous window length|U32|RW|0|
|0x1008|0x00|Manufacture Device Name|VisStr|CO|See description|
|0x1009|0x00|Manufacture hardware version|VisStr|CO|See description|
|0x100A|0x00|Manufacture software version|U32|CO|4|
|0x1010|0x00|Store parameters|U8|RO|0x04|
||0x01|Save all parameters|U32|RW|0x00000001|
||0x02|Save communication parameters|U32|RW|0x00000001|
||0x03|Save application parameters|U32|RW|0x00000001|
||0x04|Save manufacturer defined parameters|U32|RW|0x00000001|
|0x1011|0x00|Restore default parameters|U8|RO|0x04|
||0x01|Restore all default parameters|U32|RW|0x00000001|
||0x02|Restore communication default parameters|U32|RW|0x00000001|
||0x03|Restore application default parameters|U32|RW|0x00000001|
||0x04|Restore manufacturer defined default parameters|U32|RW|0x00000001|
|0x1012|0x00|COB-ID time stamp object|U32|RW|0x00000100|
|0x1014|0x00|COB-ID EMCY|U32|RW|$NODEID+0x80|
|0x1015|0x00|Inhibit time EMCY|U16|RW|0|
|0x1016|0x00|Consumer heartbeat time|U8|CO|0x01|
||0x01|Consumer heartbeat time|U32|RW|0x00000000|
|0x1017|0x00|Producer heartbeat time|U16|RW|0|
|0x1018|0x00|Identity|U8|RO|0x04|
||0x01|Vendor-ID|U32|RO|0x00000586|
||0x02|Product code|U32|RO|0x00000013|
||0x03|Revision number|U32|RO|0x00000011|
||0x04|Serial number|U32|RO|0x00000000|
|0x1019|0x00|Synchronous counter overflow value|U8|RW|0|
|0x1200|0x00|SDO server parameter|U8|RO|2|
||0x01|COB-ID client to server (rx)|U32|RO|$NODEID+0x600|
||0x02|COB-ID server to client (tx)|U32|RO|$NODEID+0x580|
|0x1280|0x00|SDO client parameter|U8|RO|0x03|
||0x01|COB-ID client to server (tx)|U32|RW|0x80000000|
||0x02|COB-ID server to client (rx)|U32|RW|0x80000000|
||0x03|Node-ID of the SDO server|U8|RW|0x01|
|0x1400|0x00|RPDO communication parameter|U8|RO|0x05|
||0x01|COB-ID used by RPDO|U32|RW|$NODEID+0x00000200|
||0x02|Transmission type|U8|RW|255|
||0x05|Event timer|U16|RW|0|
|0x1401|0x00|RPDO communication parameter|U8|RO|0x05|
||0x01|COB-ID used by RPDO|U32|RW|$NODEID+0x00000300|
||0x02|Transmission type|U8|RW|255|
||0x05|Event timer|U16|RW|0|
|0x1402|0x00|RPDO communication parameter|U8|RO|0x05|
||0x01|COB-ID used by RPDO|U32|RW|$NODEID+0x00000400|
||0x02|Transmission type|U8|RW|255|
||0x05|Event timer|U16|RW|0|
|0x1403|0x00|RPDO communication parameter|U8|RO|0x05|
||0x01|COB-ID used by RPDO|U32|RW|$NODEID+0x00000500|
||0x02|Transmission type|U8|RW|255|
||0x05|Event timer|U16|RW|0|
|0x1600|0x00|RPDO mapping parameter|U8|CO|0x01|
||0x01|Application object 1|U32|RW|0x60400010|
|0x1601|0x00|RPDO mapping parameter|U8|CO|0x02|
||0x01|Application object 1|U32|RW|0x60400010|
||0x02|Application object 2|U32|RW|0x60600008|
|0x1602|0x00|RPDO mapping parameter|U8|CO|2|
||0x01|Application object 1|U32|RW|0x60400010|
||0x02|Application object 2|U32|RW|0x60FF0020|
|0x1603|0x00|RPDO mapping parameter|U8|RW|8|
||0x01|Application object 1|U32|RW|0x60400010|
||0x02|Application object 2|U32|RW|0x60710010|
||0x03|Application object 3|U32|RW|0x00000000|
||0x04|Application object 4|U32|RW|0x00000000|
||0x05|Application object 5|U32|RW|0x00000000|
||0x06|Application object 6|U32|RW|0x00000000|
||0x07|Application object 7|U32|RW|0x00000000|
||0x08|Application object 8|U32|RW|0x00000000|
|0x1800|0x00|TPDO communication parameter|U8|RO|0x06|
||0x01|COB-ID used by TPDO|U32|RW|$NODEID+0x40000180|
||0x02|Transmission type|U8|RW|1|
||0x03|Inhibit time|U16|RW|0|
||0x05|Event timer|U16|RW|0|
||0x06|SYNC start value|U8|RW|0|
|0x1801|0x00|TPDO communication parameter|U8|RO|0x06|
||0x01|COB-ID used by TPDO|U32|RW|$NODEID+0x40000280|
||0x02|Transmission type|U8|RW|1|
||0x03|Inhibit time|U16|RW|0|
||0x05|Event timer|U16|RW|0|
||0x06|SYNC start value|U8|RW|0|
|0x1802|0x00|TPDO communication parameter|U8|RO|0x06|
||0x01|COB-ID used by TPDO|U32|RW|$NODEID+0x40000380|
||0x02|Transmission type|U8|RW|1|
||0x03|Inhibit time|U16|RW|0|
||0x05|Event timer|U16|RW|0|
||0x06|SYNC start value|U8|RW|0|
|0x1803|0x00|TPDO communication parameter|U8|RO|0x06|
||0x01|COB-ID used by TPDO|U32|RW|$NODEID+0x40000480|
||0x02|Transmission type|U8|RW|1|
||0x03|Inhibit time|U16|RW|0|
||0x05|Event timer|U16|RW|0|
||0x06|SYNC start value|U8|RW|0|
|0x1A00|0x00|TPDO mapping parameter|U8|CO|1|
||0x01|Application object 1|U32|RW|0x60410010|
|0x1A01|0x00|TPDO mapping parameter|U8|CO|2|
||0x01|Application object 1|U32|RW|0x60410010|
||0x02|Application object 2|U32|RW|0x60610008|
|0x1A02|0x00|TPDO mapping parameter|U8|CO|2|
||0x01|Application object 1|U32|RW|0x60410010|
||0x02|Application object 2|U32|RW|0x606C0020|
|0x1A03|0x00|TPDO mapping parameter|U8|CO|2|
||0x01|Application object 1|U32|RW|0x60410010|
||0x02|Application object 2|U32|RW|0x60640020|
|0x5FFF|0x00|EmSA|VisStr|RO|See description|
|0x603F|0x00|Error Code|U16|RO||
|0x6040|0x00|Controlword|U16|RWW||
|0x6041|0x00|Statusword|U16|RO||
|0x605A|0x00|Quick Stop|I16|RW|2|
|0x605B|0x00|Shutdown|I16|RO||
|0x605D|0x00|Halt|I16|RW|1|
|0x6060|0x00|Modes of Operation|I8|RW|0|
|0x6061|0x00|Modes of Operation display|I8|RO||
|0x6064|0x00|Position Actual Value|I32|RO||
|0x606C|0x00|Velocity Actual Value|I32|RO||
|0x6071|0x00|Target Torque|I16|RW|0|
|0x6077|0x00|Torque Actual Value|I16|RO|0|
|0x60FF|0x00|Target Velocity|I32|RW||
|0x6502|0x00|Supported Drive Modes|U32|RO|0xC|

### Device type (0x1000)

|||
|---|---|
|Subindex|0x00|
|Data Type|Unsigned32|
|Access|Const|
|Can be mapped|No|
|Default Value|0xFFFF0192|

### Error register (0x1001)

|||
|---|---|
|Subindex|0x00|
|Data Type|Unsigned8|
|Access|ReadOnly|
|Can be mapped|Yes|
|Default Value|0x00|

### Pre-defined error field (0x1003)

|   |   |
|---|---|
|Subindex|0x00|
|Name|Number of errors|
|Data Type|Unsigned8|
|Access|ReadWrite|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x01|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x02|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x03|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x04|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x05|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x06|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x07|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x08|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x09|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x0A|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x0B|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x0C|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x0D|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x0E|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x0F|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

|   |   |
|---|---|
|Subindex|0x10|
|Name|Standard error field|
|Data Type|Unsigned32|
|Access|ReadOnly|
|Can be mapped|No|

### COB-ID SYNC message (0x1005)

|   |   |
|---|---|
|Subindex|0x00|
|Data Type|Unsigned32|
|Access|ReadWrite|
|Can be mapped|No|
|Default Value|0x00000080|

### Communication cycle period (0x1006)

|   |   |
|---|---|
|Subindex|0x00|
|Data Type|Unsigned32|
|Access|ReadWrite|
|Can be mapped|No|
|Default Value|0|

### Synchronous window length (0x1007)

|   |   |
|---|---|
|Subindex|0x00|
|Data Type|Unsigned32|
|Access|ReadWrite|
|Can be mapped|No|
|Default Value|0|

### Manufacture Device Name (0x1008)

|   |   |
|---|---|
|Subindex|0x00|
|Data Type|Visible String|
|Access|Const|
|Can be mapped|No|
|Default Value|Kria Motor Kit|

### Manufacture hardware version (0x1009)

|   |   |
|---|---|
|Subindex|0x00|
|Data Type|Visible String|
|Access|Const|
|Can be mapped|No|
|Default Value|Rev 1.0|

### Manufacture software version (0x100A)

|   |   |
|---|---|
|Subindex|0x00|
|Data Type|Unsigned32|
|Access|Const|
|Can be mapped|No|
|Default Value|4|

### Store parameters (0x1010)

|   |   |
|---|---|
|Subindex|0x00|
|Name|Highest sub-index supported|
|Data Type|Unsigned8|
|Access|ReadOnly|
|Can be mapped|No|
|Default Value|0x04|

|   |   |
|---|---|
|Subindex|0x01|
|Name|Save all parameters|
|Data Type|Unsigned32|
|Access|ReadWrite|
|Can be mapped|No|
|Default Value|0x00000001|

|   |   |
|---|---|
|Subindex|0x02|
|Name|Save communication parameters|
|Data Type|Unsigned32|
|Access|ReadWrite|
|Can be mapped|No|
|Default Value|0x00000001|

|   |   |
|---|---|
|Subindex|0x03|
|Name|Save application parameters|
|Data Type|Unsigned32|
|Access|ReadWrite|
|Can be mapped|No|
|Default Value|0x00000001|

|   |   |
|---|---|
|Subindex|0x04|
|Name|Save manufacturer defined parameters|
|Data Type|Unsigned32|
|Access|ReadWrite|
|Can be mapped|No|
|Default Value|0x00000001|