/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Peridot_msdSysCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell SysCtrl functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Peridot_msdSysCtrl_h
#define __Peridot_msdSysCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported SysCtrl Types			 			                                   */
/****************************************************************************/
#define PERIDOT_MAX_LAG_MASK_NUM    8

/****************************************************************************/
/* Exported SysCtrl Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* Peridot_gsysSetFloodBC
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination 
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*       other switch policy still applies to these Broadcast frames. 
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's 
*       Egress Floods mode.
*
* INPUTS:
*       en - MSD_TRUE to enable Flood Broadcast, MSD_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetFloodBC
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
);

/*******************************************************************************
* Peridot_gsysGetFloodBC
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination 
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*       other switch policy still applies to these Broadcast frames. 
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's 
*       Egress Floods mode.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE if Flood Broadcast is enabled, MSD_FALSE otherwise.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetFloodBC
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
);

/*******************************************************************************
* Peridot_gsysSetTrunkMaskTable
*
* DESCRIPTION:
*       This function sets Trunk Mask for the given Mask Number.
*
* INPUTS:
*       maskNum - Mask Number.
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetTrunkMaskTable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       maskNum,
    IN MSD_U32       trunkMask
);

/*******************************************************************************
* Peridot_gsysGetTrunkMaskTable
*
* DESCRIPTION:
*       This function gets Trunk Mask for the given Trunk Number.
*
* INPUTS:
*       maskNum - Mask Number.
*
* OUTPUTS:
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if maskNum > 0x7)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetTrunkMaskTable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       maskNum,
    OUT MSD_U32       *trunkMask
);


/*******************************************************************************
* Peridot_gsysSetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to 
*       select the TrunkMask to use.
*
* INPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetHashTrunk
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
);


/*******************************************************************************
* Peridot_gsysGetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to 
*       select the TrunkMask to use.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetHashTrunk
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
);

/*******************************************************************************
* Peridot_gsysSetTrunkRouting
*
* DESCRIPTION:
*       This function sets routing information for the given Trunk ID.
*
* INPUTS:
*       trunkId - Trunk ID.
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if trunkId > 0x1F)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetTrunkRouting
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       trunkId,
    IN MSD_U32       trunkRoute
);

/*******************************************************************************
* Peridot_gsysGetTrunkRouting
*
* DESCRIPTION:
*       This function retrieves routing information for the given Trunk ID.
*
* INPUTS:
*       trunkId - Trunk ID.
*
* OUTPUTS:
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if trunkId > 0x1F)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetTrunkRouting
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       trunkId,
    OUT MSD_U32       *trunkRoute
);

/*******************************************************************************
* Peridot_gsysSetMonitorMgmtCtrl
*
* DESCRIPTION:
*       Set Monitor and mgmt control data. 
*         
* INPUTS:
*       point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*       data  - Monitor and mgmt Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetMonitorMgmtCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_U8             point,
    IN  MSD_U8            data
);

/*******************************************************************************
* Peridot_gsysGetMonitorMgmtCtrl
*
* DESCRIPTION:
*       Get Monitor and mgmt control data. 
*         
* INPUTS:
*       point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*
* OUTPUTS:
*       data  - Monitor and mgmt Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetMonitorMgmtCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_U8             point,
    OUT  MSD_U8            *data
);

/*******************************************************************************
* Peridot_gsysSetCPUDest
*
* DESCRIPTION:
*       This routine sets CPU Destination Port. CPU Destination port indicates the
*       port number on this device where the CPU is connected (either directly or
*       indirectly through another Marvell switch device).
*
*       Many modes of frame processing need to know where the CPU is located.
*       These modes are:
*        1. When IGMP/MLD frame is received and Snooping is enabled
*        2. When the port is configured as a DSA port and it receives a To_CPU frame
*        3. When a Rsvd2CPU frame enters the port
*        4. When the port's SA Filtering mode is Drop to CPU
*        5. When any of the port's Policy Options trap the frame to the CPU
*        6. When the ingressing frame is an ARP and ARP mirroring is enabled in the
*           device
*
*       In all cases, except for ARP, the frames that meet the enabled criteria 
*       are mapped to the CPU Destination port, overriding where the frame would 
*       normally go. In the case of ARP, the frame will be mapped normally and it 
*       will also get copied to this port.
*       Frames that filtered or discarded will not be mapped to the CPU Destination 
*       port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*       If CPUDest = 0x1F, the remapped frames will be discarded, no ARP mirroring 
*       will occur and ingressing To_CPU frames will be discarded.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetCPUDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
);

/*******************************************************************************
* Peridot_gsysGetCPUDest
*
* DESCRIPTION:
*       This routine gets CPU Destination Port. CPU Destination port indicates the
*       port number on this device where the CPU is connected (either directly or
*       indirectly through another Marvell switch device).
*
*       Many modes of frame processing need to know where the CPU is located.
*       These modes are:
*        1. When IGMP/MLD frame is received and Snooping is enabled
*        2. When the port is configured as a DSA port and it receives a To_CPU frame
*        3. When a Rsvd2CPU frame enters the port
*        4. When the port's SA Filtering mode is Drop to CPU
*        5. When any of the port's Policy Options trap the frame to the CPU
*        6. When the ingressing frame is an ARP and ARP mirroring is enabled in the
*           device
*
*       In all cases, except for ARP, the frames that meet the enabled criteria 
*       are mapped to the CPU Destination port, overriding where the frame would 
*       normally go. In the case of ARP, the frame will be mapped normally and it 
*       will also get copied to this port.
*       Frames that filtered or discarded will not be mapped to the CPU Destination 
*       port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*       If CPUDest = 0x1F, the remapped frames will be discarded, no ARP mirroring 
*       will occur and ingressing To_CPU frames will be discarded.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetCPUDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
);

/*******************************************************************************
* Peridot_gsysSetIngressMonitorDest
*
* DESCRIPTION:
*       This routine sets Ingress Monitor Destination Port. Frames that are 
*       targeted toward an Ingress Monitor Destination go out the port number 
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Ingress Monitor type, and frames received on a Network port that 
*       is enabled to be the Ingress Monitor Source Port.
*       If the Ingress Monitor Destination Port resides in this device these bits 
*       should point to the Network port where these frames are to egress. If the 
*       Ingress Monitor Destination Port resides in another device these bits 
*       should point to the Marvell Tag port in this device that is used to get 
*       to the device that contains the Ingress Monitor Destination Port.
*
* INPUTS:
*       port  - the logical port number.when port = 0x1F,IMD is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetIngressMonitorDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
);

/*******************************************************************************
* Peridot_gsysGetIngressMonitorDest
*
* DESCRIPTION:
*       This routine gets Ingress Monitor Destination Port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetIngressMonitorDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
);

/*******************************************************************************
* Peridot_gsysSetEgressMonitorDest
*
* DESCRIPTION:
*       This routine sets Egress Monitor Destination Port. Frames that are 
*       targeted toward an Egress Monitor Destination go out the port number 
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Egress Monitor type, and frames transmitted on a Network port that 
*       is enabled to be the Egress Monitor Source Port.
*       If the Egress Monitor Destination Port resides in this device these bits 
*       should point to the Network port where these frames are to egress. If the 
*       Egress Monitor Destination Port resides in another device these bits 
*       should point to the Marvell Tag port in this device that is used to get 
*       to the device that contains the Egress Monitor Destination Port.
*
* INPUTS:
*       port  - the logical port number.If port = 0x1F, EMD is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetEgressMonitorDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
);

/*******************************************************************************
* Peridot_gsysGetEgressMonitorDest
*
* DESCRIPTION:
*       This routine gets Egress Monitor Destination Port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetEgressMonitorDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
);

/*******************************************************************************
* Peridot_gsysSetRsvd2CpuEnables
*
* DESCRIPTION:
*       Reserved DA Enables. When the Rsvd2Cpu(gsysSetRsvd2Cpu) is set to a one, 
*       the 16 reserved multicast DA addresses, whose bit in this register are 
*       also set to a one, are treadted as MGMT frames. All the reserved DA's 
*       take the form 01:80:C2:00:00:0x. When x = 0x0, bit 0 of this register is 
*       tested. When x = 0x2, bit 2 of this field is tested and so on.
*       If the tested bit in this register is cleared to a zero, the frame will 
*       be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*       enBits - bit vector of enabled Reserved Multicast.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetRsvd2CpuEnables
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U16        enBits
);

/*******************************************************************************
* Peridot_gsysGetRsvd2CpuEnables
*
* DESCRIPTION:
*       Reserved DA Enables. When the Rsvd2Cpu(gsysSetRsvd2Cpu) is set to a one, 
*       the 16 reserved multicast DA addresses, whose bit in this register are 
*       also set to a one, are treadted as MGMT frames. All the reserved DA's 
*       take the form 01:80:C2:00:00:0x. When x = 0x0, bit 0 of this register is 
*       tested. When x = 0x2, bit 2 of this field is tested and so on.
*       If the tested bit in this register is cleared to a zero, the frame will 
*       be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       enBits - bit vector of enabled Reserved Multicast.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetRsvd2CpuEnables
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_U16      *enBits
);

/*******************************************************************************
* Peridot_gsysSetMGMTPri
*
* DESCRIPTION:
*       These bits are used as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       pri - PRI[2:0] bits (should be less than 8)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (If pri is not less than 8)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetMGMTPri
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U16        pri
);


/*******************************************************************************
* Peridot_gsysGetMGMTPri
*
* DESCRIPTION:
*       These bits are used as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       pri - PRI[2:0] bits (should be less than 8)
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetMGMTPri
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_U16      *pri
);

/*******************************************************************************
* Peridot_gsysSetSMISetup
*
* DESCRIPTION:
*       This routine is to set SMI PHY Setup
*
* INPUTS:
*       pointer - [9:0] bits (should be less than 0x400)
*		data	- SMI PHY Setup.
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (If pri is not less than 8)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetSMISetup
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT  portNum,
IN MSD_U16		  data
);

/*******************************************************************************
* Peridot_gsysGetSMISetup
*
* DESCRIPTION:
*       This routine is to get SMI PHY Setup
*
* INPUTS:
*       portNum - [9:0] bits (should be less than 0x400)
*
* OUTPUTS:
*       data	- SMI PHY Setup.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetSMISetup
(
IN  MSD_QD_DEV		*dev,
IN  MSD_LPORT		portNum,
OUT MSD_U16			*data
);

MSD_STATUS Peridot_gsysGlobal1RegDump
(
IN  MSD_QD_DEV    *dev
);

MSD_STATUS Peridot_gsysGlobal2RegDump
(
IN  MSD_QD_DEV    *dev
);

/*******************************************************************************
* Peridot_gsysSetRMUMode
*
* DESCRIPTION:
*       These routine sets Remote Management Unit Mode.
*
* INPUTS:
*       data - RMU mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (If pri is not less than 8)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysSetRMUMode
(
IN MSD_QD_DEV    *dev,
IN MSD_U16        data
);

/*******************************************************************************
* Peridot_gsysGetRMUMode
*
* DESCRIPTION:
*       These routine gets Remote Management Unit Mode..
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       data - RMU mode
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysGetRMUMode
(
IN  MSD_QD_DEV    *dev,
OUT MSD_U16      *data
);

/*******************************************************************************
* Peridot_gsysQbvWrite
*
* DESCRIPTION:
*       These routine write QBV registers.
*
* INPUTS:
*       qbvPort    - QBV port (0x1F - Qbv global registers)
*       qbvRegAddr - QBV register address
*       data       - QBV data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysQbvWrite
(
IN MSD_QD_DEV    *dev,
IN MSD_U8        qbvPort,
IN MSD_U8        qbvRegAddr,
IN MSD_U16       data
);

/*******************************************************************************
* Peridot_gsysQbvRead
*
* DESCRIPTION:
*       These routine gets QBV register value.
*
* INPUTS:
*       qbvPort    - QBV port (0x1F - Qbv global registers)
*       qbvRegAddr - QBV register address
*
* OUTPUTS:
*       data - QBV data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysQbvRead
(
IN MSD_QD_DEV    *dev,
IN MSD_U8        qbvPort,
IN MSD_U8        qbvRegAddr,
OUT MSD_U16      *data
);

/*******************************************************************************
* Peridot_gsysDevIntEnableSet
*
* DESCRIPTION:
*       These routine sets interrupt enable or disable Interrupt Mask to drive the
*       DeviceInt bit in the Switch Global Status register.
*
* INPUTS:
*       dev    - device structure
*       en     - interrupt mask, any combination of the folowing:
*                MSD_WATCH_DOG_INT			0x8000
*                MSD_JAM_LIMIT_INT			0x4000
*                MSD_DUPLEX_MISMATCH_INT	0x2000
*                MSD_WAKE_EVENT_INT			0x1000
*                MSD_PHY_INT_10 			0x400
*                MSD_PHY_INT_9 				0x200
*                MSD_PHY_INT_8 				0x100
*                MSD_PHY_INT_7 				0x80
*                MSD_PHY_INT_6 				0x40
*                MSD_PHY_INT_5 				0x20
*                MSD_PHY_INT_4 				0x10
*                MSD_PHY_INT_3 				0x8
*                MSD_PHY_INT_2 				0x4
*                MSD_PHY_INT_1 				0x2
*                MSD_PHY_INT_0 				0x1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysDevIntEnableSet
(
IN MSD_QD_DEV    *dev,
IN MSD_U16       en
);

/*******************************************************************************
* Peridot_gsysDevIntEnableGet
*
* DESCRIPTION:
*       These routine gets interrupt enable or disable Interrupt Mask to drive the
*       DeviceInt bit in the Switch Global Status register.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt mask
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysDevIntEnableGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *en
);

/*******************************************************************************
* Peridot_gsysDevIntStatusGet
*
* DESCRIPTION:
*       These routine gets interrupt source.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt source
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysDevIntStatusGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *status
);

/*******************************************************************************
* Peridot_gsysActiveIntEnableSet
*
* DESCRIPTION:
*       These routine sets interrupt enable or disable to drive the device��s INTn pin low.
*
* INPUTS:
*       dev    - device structure
*       en     - interrupt data, any combination of the folowing:
*                MSD_DEVICE2_INT	0x200
*                MSD_AVB_INT 		0x100
*                MSD_DEVICE_INT		0x80
*                MSD_STATS_DONE		0x40
*                MSD_VTU_PROB		0x20
*                MSD_VTU_DONE		0x10
*                MSD_ATU_PROB		0x8
*                MSD_ATU_DONE 		0x4
*                MSD_TCAM_INT		0x2
*                MSD_EE_INT			0x1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysActiveIntEnableSet
(
IN MSD_QD_DEV    *dev,
IN MSD_U16       en
);

/*******************************************************************************
* Peridot_gsysActiveIntEnableGet
*
* DESCRIPTION:
*       These routine gets interrupt enable or disable to drive the device��s INTn pin low.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysActiveIntEnableGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *en
);

/*******************************************************************************
* Peridot_gsysActiveIntStatusGet
*
* DESCRIPTION:
*       This routine reads an hardware driven event status.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt status
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Peridot_gsysActiveIntStatusGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16      *status
);

#ifdef __cplusplus
}
#endif

#endif /* __Peridot_msdSysCtrl_h */
