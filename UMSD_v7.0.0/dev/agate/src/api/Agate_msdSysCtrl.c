/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Agate_msdSysCtrl.c
*
* DESCRIPTION:
*       API definitions for system global control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <agate/include/api/Agate_msdSysCtrl.h>
#include <agate/include/api/Agate_msdApiInternal.h>
#include <agate/include/driver/Agate_msdHwAccess.h>
#include <agate/include/driver/Agate_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
/*******************************************************************************
* Agate_gsysSetTrunkMaskTable
*
* DESCRIPTION:
*       This function sets Trunk Mask for the given Mask Number.
*
* INPUTS:
*       maskNum - Mask Number (0~7).
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
MSD_STATUS Agate_gsysSetTrunkMaskTable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       maskNum,
    IN MSD_U32       trunkMask
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32            mask;
    int count=0x10;

    MSD_DBG_INFO(("Agate_gsysSetTrunkMaskTable Called.\n"));

	if (maskNum >= AGATE_MAX_LAG_MASK_NUM)
    {
		MSD_DBG_ERROR(("MSD_BAD_PARAM (maskNum: %u). It should be within [0, %d]\n", (unsigned int)maskNum, (AGATE_MAX_LAG_MASK_NUM - 1)));
        return MSD_BAD_PARAM;
    }

    mask = (1 << dev->numOfPorts) - 1;
    if(trunkMask > mask)
    {
        MSD_DBG_ERROR(("MSD_BAD_PARAM (trunkMask: 0x%x). It should be within [0, 0x%x]\n", trunkMask, mask));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_TRUNK_MASK_TBL, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mask tbl register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G2 trunk mask tbl register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (data & 0x8000);

	/* Set related register */
	data &= 0x0800; /* maintain existing HasHash bit.*/
	mask = MSD_LPORTVEC_2_PORTVEC(trunkMask);
    data = (MSD_U16)(0x8000 | data |  (maskNum << 12) | mask);   
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_TRUNK_MASK_TBL,data);

	msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gsysSetTrunkMaskTable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysGetTrunkMaskTable
*
* DESCRIPTION:
*       This function Agate_gets Trunk Mask for the given Trunk Number.
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
MSD_STATUS Agate_gsysGetTrunkMaskTable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       maskNum,
    OUT MSD_U32       *trunkMask
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32            mask;
    int count=0x10;

    MSD_DBG_INFO(("Agate_gsysGetTrunkMaskTable Called.\n"));

	if (maskNum >= AGATE_MAX_LAG_MASK_NUM)
	{
		MSD_DBG_ERROR(("MSD_BAD_PARAM (maskNum: %u). It should be within [0, %d]\n", (unsigned int)maskNum, (AGATE_MAX_LAG_MASK_NUM - 1)));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_TRUNK_MASK_TBL,&data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mask tbl register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G2 trunk mask tbl register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (data & 0x8000);

	/* Set related register */
    data &= 0x0800;
    data = (MSD_U16)(data | (maskNum << 12));
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_TRUNK_MASK_TBL,data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
    }

	/* read-back value */
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_TRUNK_MASK_TBL,&data);
    msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
    }

    mask = (1 << dev->maxPorts) - 1;
    *trunkMask = MSD_PORTVEC_2_LPORTVEC(data & mask);
            
    MSD_DBG_INFO(("Agate_gsysGetTrunkMaskTable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysSetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed toAgate_gether to 
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
MSD_STATUS Agate_gsysSetHashTrunk
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16            data;

    MSD_DBG_INFO(("Agate_gsysSetHashTrunk Called.\n"));

	MSD_BOOL_2_BIT(en, data);

    /* Set related bit */
	retVal = msdSetAnyRegField(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_TRUNK_MASK_TBL,11,1,data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gsysSetHashTrunk Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysGetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed toAgate_gether to 
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
MSD_STATUS Agate_gsysGetHashTrunk
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_DBG_INFO(("Agate_gsysGetHashTrunk Called.\n"));

    /* Get related bit */
    retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,AGATE_QD_REG_TRUNK_MASK_TBL,11,1,&data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyRegField for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Agate_gsysGetHashTrunk Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysSetTrunkRouting
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
MSD_STATUS Agate_gsysSetTrunkRouting
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       trunkId,
    IN MSD_U32       trunkRoute
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32            mask;

    int count=0x10;

    MSD_DBG_INFO(("Agate_gsysSetTrunkRouting Called.\n"));

	/* Check if trunkId is out of range [0,0xf] */
	if (!AGATE_IS_TRUNK_ID_VALID(dev, trunkId))
    {
		MSD_DBG_INFO(("MSD_BAD_PARAM (trunkId: %u)\n", (unsigned int)trunkId));
        return MSD_BAD_PARAM;
    }

	/* Check if trunkRoute is our of range [0,0x7ff] */
    mask = (1 << dev->numOfPorts) - 1;
    if(trunkRoute > mask)
    {
		MSD_DBG_ERROR(("MSD_BAD_PARAM (trunkRoute: 0x%x). It should be within [0, 0x%x]\n", trunkRoute, mask));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_TRUNK_ROUTING, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mapping register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G2 trunk mapping register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (data & 0x8000);

	/* Write TrunkId[4] in Global 2, Misc Register 0x1D, bit 13 */
	data = (MSD_U16)((trunkId&0x10)>>4);
	retVal = msdSetAnyRegField(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MISC, 13, 1, data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 misc register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set related register */
	mask = MSD_LPORTVEC_2_PORTVEC(trunkRoute);
	data = (MSD_U16)(0x8000 | ((trunkId&0xf) << 11) | mask);
	retVal = msdSetAnyReg(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_TRUNK_ROUTING, data);

	msdSemGive(dev->devNum, dev->tblRegsSem);

	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    MSD_DBG_INFO(("Agate_gsysSetTrunkRouting Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysGetTrunkRouting
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
MSD_STATUS Agate_gsysGetTrunkRouting
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       trunkId,
    OUT MSD_U32       *trunkRoute
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32            mask;
    int count=0x10;

    MSD_DBG_INFO(("Agate_gsysGetTrunkRouting Called.\n"));

	/* Check if trunkId is out of range [0,0x1f] */
	if (!AGATE_IS_TRUNK_ID_VALID(dev, trunkId))
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (trunkId: %u)\n", (unsigned int)trunkId));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_TRUNK_ROUTING, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mapping register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait G2 trunk mapping register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (data & 0x8000);


	/* Write TrunkId[4] in Global 2, Misc Register 0x1D, bit 13 */
	data = (MSD_U16)((trunkId&0x10)>>4);
	retVal = msdSetAnyRegField(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MISC, 13, 1, data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 misc register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set related register */
	data = (MSD_U16)((trunkId&0xf) << 11);
	retVal = msdSetAnyReg(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_TRUNK_ROUTING, data);
    if(retVal != MSD_OK)
    {
	    MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
    }

	/* Read-back value */
	retVal = msdGetAnyReg(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_TRUNK_ROUTING, &data);

	msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
	    MSD_DBG_ERROR(("msdGetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    mask = (1 << dev->maxPorts) - 1;
    *trunkRoute = MSD_PORTVEC_2_LPORTVEC(data & mask);

    MSD_DBG_INFO(("Agate_gsysGetTrunkRouting Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysSetFloodBC
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
MSD_STATUS Agate_gsysSetFloodBC
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16            data;

    MSD_DBG_INFO(("Agate_gsysSetFloodBC Called.\n"));

	MSD_BOOL_2_BIT(en, data);

    /* Set related bit */
	retVal = msdSetAnyRegField(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MANAGEMENT, 12, 1, data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 management register returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Agate_gsysSetFloodBC Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysGetFloodBC
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
MSD_STATUS Agate_gsysGetFloodBC
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_DBG_INFO(("Agate_gsysGetFloodBC Called.\n"));

    /* Get related bit */
	retVal = msdGetAnyRegField(dev->devNum,  AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MANAGEMENT, 12, 1, &data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyRegField for G2 management register returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	MSD_BIT_2_BOOL(data, *en);
    MSD_DBG_INFO(("Agate_gsysGetFloodBC Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysSetCPUDest
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
*       will also Agate_get copied to this port.
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
MSD_STATUS Agate_gsysSetCPUDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16			data;

    MSD_DBG_INFO(("Agate_gsysSetCPUDest Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);

    /* translate LPORT to hardware port */
    if(port == 0xF)
        hwPort = (MSD_U8)port;
    else
    {
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(port);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", (unsigned int)port));
			return MSD_BAD_PARAM;
		}
    }
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, &data);
	
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Agate_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	data = ((port & 0xF) << 4) | (data & 0xFF0F);
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, data);	
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_gsysSetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	
    MSD_DBG_INFO(("Agate_gsysSetCPUDest Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Agate_gsysGetCPUDest
*
* DESCRIPTION:
*       This routine Agate_gets CPU Destination Port. CPU Destination port indicates the
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
*       will also Agate_get copied to this port.
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
MSD_STATUS Agate_gsysGetCPUDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Agate_gsysGetCPUDest Called.\n"));

	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, &data);
    if(retVal != MSD_OK)
	{
	    MSD_DBG_ERROR(("Agate_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	    return retVal;
	}
	*port = (data & 0x00F0) >> 4;

    MSD_DBG_INFO(("Agate_gsysGetCPUDest Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysSetIngressMonitorDest
*
* DESCRIPTION:
*       This routine sets Ingress Monitor Destination Port. Frames that are 
*       tarAgate_geted toward an Ingress Monitor Destination go out the port number 
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Ingress Monitor type, and frames received on a Network port that 
*       is enabled to be the Ingress Monitor Source Port.
*       If the Ingress Monitor Destination Port resides in this device these bits 
*       should point to the Network port where these frames are to egress. If the 
*       Ingress Monitor Destination Port resides in another device these bits 
*       should point to the Marvell Tag port in this device that is used to Agate_get 
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
MSD_STATUS Agate_gsysSetIngressMonitorDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16			data;

    MSD_DBG_INFO(("Agate_gsysSetIngressMonitorDest Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);

    /* translate LPORT to hardware port */
    if(port == 0xF)
        hwPort = (MSD_U8)port;
    else
    {
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(port);
        if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", (unsigned int)port));
			return MSD_BAD_PARAM;
		}
    }

	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, &data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	data = ((hwPort & 0xF) << 12) | (data & 0xFFF);
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	return retVal;
}

/*******************************************************************************
* Agate_gsysGetIngressMonitorDest
*
* DESCRIPTION:
*       This routine Agate_gets Ingress Monitor Destination Port.
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
MSD_STATUS Agate_gsysGetIngressMonitorDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Agate_gsysGetIngressMonitorDest Called.\n"));

	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	*port = (data & 0xF000) >> 12;

    MSD_DBG_INFO(("Agate_gsysGetIngressMonitorDest Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysSetEgressMonitorDest
*
* DESCRIPTION:
*       This routine sets Egress Monitor Destination Port. Frames that are 
*       tarAgate_geted toward an Egress Monitor Destination go out the port number 
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Egress Monitor type, and frames transmitted on a Network port that 
*       is enabled to be the Egress Monitor Source Port.
*       If the Egress Monitor Destination Port resides in this device these bits 
*       should point to the Network port where these frames are to egress. If the 
*       Egress Monitor Destination Port resides in another device these bits 
*       should point to the Marvell Tag port in this device that is used to Agate_get 
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
MSD_STATUS Agate_gsysSetEgressMonitorDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;
    MSD_DBG_INFO(("Agate_gsysSetEgressMonitorDest Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);

    if(port == 0xF)
        hwPort = (MSD_U8)port;
    else
    {
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(port);
        if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", (unsigned int)port));
			return MSD_BAD_PARAM;
		}
    }

	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, &data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	data = ((hwPort & 0xF) << 8) | (data & 0xF0FF);
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	
    MSD_DBG_INFO(("Agate_gsysSetEgressMonitorDest Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gsysGetEgressMonitorDest
*
* DESCRIPTION:
*       This routine Agate_gets Egress Monitor Destination Port.
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
MSD_STATUS Agate_gsysGetEgressMonitorDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Agate_gsysGetEgressMonitorDest Called.\n"));
		
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_MONITOR_MGMT_CTRL, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	data = (data & 0x0F00) >> 8;
	if (data != 0xf)
		*port = MSD_PORT_2_LPORT((MSD_U8)data);
	else
		*port = (MSD_U8)data;
    MSD_DBG_INFO(("Agate_gsysGetEgressMonitorDest Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Agate_gsysSetRsvd2CpuEnables
*
* DESCRIPTION:
*       Reserved DA Enables. When the Rsvd2Cpu(Agate_gsysSetRsvd2Cpu) is set to a one, 
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
MSD_STATUS Agate_gsysSetRsvd2CpuEnables
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U16        enBits
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16		Data;
    MSD_DBG_INFO(("Agate_gsysSetRsvd2CpuEnables Called.\n"));
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MANAGEMENT, &Data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	Data = (Data & 0xfff7) | 0x0008;
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MANAGEMENT, Data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_msdSetAnyReg for MGMT Pri returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MGMT_EN, enBits);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_msdSetAnyReg for point 0 returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	MSD_DBG_INFO(("Agate_gsysSetRsvd2CpuEnables Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Agate_gsysGetRsvd2CpuEnables
*
* DESCRIPTION:
*       Reserved DA Enables. When the Rsvd2Cpu(Agate_gsysSetRsvd2Cpu) is set to a one, 
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
MSD_STATUS Agate_gsysGetRsvd2CpuEnables
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_U16      *enBits
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */	
	MSD_U16 dataw;

    MSD_DBG_INFO(("Agate_gsysGetRsvd2CpuEnables Called.\n"));

	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MGMT_EN, &dataw);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_msdSetAnyReg for point 0 returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	*enBits = dataw;

	MSD_DBG_INFO(("Agate_gsysGetRsvd2CpuEnables Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Agate_gsysSetMGMTPri
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
MSD_STATUS Agate_gsysSetMGMTPri
(
IN MSD_QD_DEV    *dev,
IN MSD_U16        pri
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			data;

	MSD_DBG_INFO(("Agate_gsysSetMGMTPri Called.\n"));

	if (pri > 0x7)
	{
		MSD_DBG_ERROR(("MSD_BAD_PARAM (pri: %u). It should be within [0, 7].\n", (unsigned int)pri));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MANAGEMENT, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* Set related bit */
	data = (MSD_U16)((data & 0xFFF8) | (pri & 0x7));
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MANAGEMENT, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gsysSetMGMTPri Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysGetMGMTPri
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
MSD_STATUS Agate_gsysGetMGMTPri
(
IN  MSD_QD_DEV    *dev,
OUT MSD_U16      *pri
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			data;

	MSD_DBG_INFO(("Agate_gsysGetMGMTPri Called.\n"));

	/* Get related bit */
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_MANAGEMENT, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	*pri = data & 0x07;

	MSD_DBG_INFO(("Agate_gsysGetMGMTPri Exit.\n"));
	return MSD_OK;
}


MSD_STATUS Agate_gsysGlobal1RegDump
(
IN  MSD_QD_DEV    *dev
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16	data;
	MSD_U8 i = 0;
	char desc[32][48] = 
	{
		{ "Switch Global Status(0x0)" },
		{ "ATU FID Register(0x1)" },
		{ "VTU FID Register(0x2)" },
		{ "VTU SID Register(0x3)" },
		{ "Switch Global Control Register(0x4)" },
		{ "VTU Operation Register(0x5)" },
		{ "VTU VID Register(0x6)" },
		{ "VTU/STU Data Register Ports 0 to 7(0x7)" },
		{ "VTU/STU Data Register Ports 8 to 10(0x8)" },
		{ "Reserved(0x9)" },
		{ "ATU Control Register(0xa)" },
		{ "ATU Operation Register(0xb)" },
		{ "ATU Data Register(0xc)" },
		{ "Switch MAC Address Register Bytes 0 & 1(0xd)" },
		{ "Switch MAC Address Register Bytes 2 & 3(0xe)" },
		{ "Switch MAC Address Register Bytes 4 & 5(0xf)" },
		{ "IP-PRI Mapping 0(0x10)" },
		{ "IP-PRI Mapping 1(0x11)" },
		{ "IP-PRI Mapping 2(0x12)" },
		{ "IP-PRI Mapping 3(0x13)" },
		{ "IP-PRI Mapping 4(0x14)" },
		{ "IP-PRI Mapping 5(0x15)" },
		{ "IP-PRI Mapping 6(0x16)" },
		{ "IP-PRI Mapping 7(0x17)" },
		{ "IEEE FPri to QPri Mapping(0x18)" },
		{ "IP PRI Mapping Table(0x19)" },
		{ "Monitor Control(0x1a)" },
		{ "Free Size & G Debug(0x1b)" },
		{ "Global Control 2(0x1c)" },
		{ "Stats Operation Register(0x1d)" },
		{ "Stats Counter Register Byte 3 & 2(0x1e)" },
		{ "Stats Counter Register Byte 1 & 0(0x1f)" }
	};

	MSD_DBG_INFO(("Agate_gsysGlobal1RegDump Called.\n"));

	MSG(("\n------------------------------------------------------\n"));
	for (i = 0; i < 32; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, i, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		MSG(("%-48s%04x\n", desc[i], data));
	}

	MSD_DBG_INFO(("Agate_gsysGlobal1RegDump Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Agate_gsysGlobal2RegDump
(
IN  MSD_QD_DEV    *dev
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16	data;
	MSD_U8 i = 0;
	char desc[32][48] =
	{
		{ "Interrupt Source Register(0x0)" },
		{ "Interupt Mask Register(0x1)" },
		{ "MGMT Enables 2x(0x2)" },
		{ "MGMT Enables 0x(0x3)" },
		{ "Flow Control Delay Register(0x4)" },
		{ "Switch Management Register(0x5)" },
		{ "Device Mapping Table Register(0x6)" },
		{ "LAG Mask Table Register(0x7)" },
		{ "LAG Mapping Register(0x8)" },
		{ "Ingress Rate Command Register(0x9)" },
		{ "Ingress Rate Data Register(0xa)" },
		{ "Cross Chip Port VLAN Addr Register(0xb)" },
		{ "Cross Chip Port VLAN Data Register(0xc)" },
		{ "Switch MAC/WoL/WoF register(0xd)" },
		{ "ATU Stats(0xe)" },
		{ "Priority Override Table(0xf)" },
		{ "Reserved(0x10)" },
		{ "Reserved(0x11)" },
		{ "Reserved(0x12)" },
		{ "Reserved(0x13)" },
		{ "EEPROM Command(0x14)" },
		{ "EEPROM Addr(0x15)" },
		{ "AVB/TSN Command Register(0x16)" },
		{ "AVB/TSN Data Register(0x17)" },
		{ "SMI PHY Command for SMI Access(0x18)" },
		{ "SMI PHY Data Register(0x19)" },
		{ "Scratch and Misc(0x1a)" },
		{ "Watch Dog Control Register(0x1b)" },
		{ "QoS Weights Register(0x1c)" },
		{ "Misc Register(0x1d)" },
		{ "Reserved(0x1e)" },
		{ "Reserved(0x1f)" }
	};

	MSD_DBG_INFO(("Agate_gsysGlobal2RegDump Called.\n"));

	MSG(("\n------------------------------------------------\n"));
	for (i = 0; i < 32; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, i, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		MSG(("%-42s%04x\n", desc[i], data));
	}

	MSD_DBG_INFO(("Agate_gsysGlobal2RegDump Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysSetRMUMode
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
MSD_STATUS Agate_gsysSetRMUMode
(
IN MSD_QD_DEV    *dev,
IN MSD_U16        data
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Agate_gsysSetRMUMode Called.\n"));

	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_GLOBAL_CONTROL2, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 global control2 register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set related bit */
	tmpdata = (MSD_U16)((tmpdata & 0xCFFF) | ((data & 0x3) << 12));

	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_GLOBAL_CONTROL2, tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write G1 global control2 register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gsysSetRMUMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysGetRMUMode
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
MSD_STATUS Agate_gsysGetRMUMode
(
IN  MSD_QD_DEV    *dev,
OUT MSD_U16      *data
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Agate_gsysGetRMUMode Called.\n"));

	/* Get related bit */
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_GLOBAL_CONTROL2, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 global control2 register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set related bit */
	*data = (MSD_U16)((tmpdata & 0x3000) >> 12);

	MSD_DBG_INFO(("Agate_gsysGetRMUMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysDevIntEnableSet
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
MSD_STATUS Agate_gsysDevIntEnableSet
(
IN MSD_QD_DEV    *dev,
IN MSD_U16       en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */

	MSD_DBG_INFO(("Agate_gsysDevIntEnableSet Called.\n"));

	/*set G2 0x01 register, interrupt mask*/
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_INT_MASK, en);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write G2 interrupt mask register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gsysDevIntEnableSet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysDevIntEnableGet
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
MSD_STATUS Agate_gsysDevIntEnableGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Agate_gsysDevIntEnableGet Called.\n"));

	/*set G2 0x01 register, interrupt mask*/
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_INT_MASK, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G2 interrupt mask register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	*en = (MSD_U16)tmpdata;

	MSD_DBG_INFO(("Agate_gsysDevIntEnableGet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysDevIntStatusGet
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
MSD_STATUS Agate_gsysDevIntStatusGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *status
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Agate_gsysDevIntStatusGet Called.\n"));

	/*set G2 0x01 register, interrupt mask*/
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_INT_SOURCE, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G2 interrupt source register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	*status = (MSD_U16)tmpdata;

	MSD_DBG_INFO(("Agate_gsysDevIntStatusGet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysActiveIntEnableSet
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
MSD_STATUS Agate_gsysActiveIntEnableSet
(
IN MSD_QD_DEV    *dev,
IN MSD_U16       en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Agate_gsysActiveIntEnableSet Called.\n"));

	/*get G1 0x04 register, interrupt to drive the device��s INTn low pin.*/
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_GLOBAL_CONTROL, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 Global Control Register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	tmpdata = (tmpdata & 0xFC00) | en;

	/*set G1 0x04 register, interrupt to drive the device��s INTn low pin.*/
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_GLOBAL_CONTROL, tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write G1 Global Control Register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gsysActiveIntEnableSet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysActiveIntEnableGet
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
MSD_STATUS Agate_gsysActiveIntEnableGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Agate_gsysActiveIntEnableGet Called.\n"));

	/*get G1 0x04 register, interrupt to drive the device��s INTn low pin.*/
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_GLOBAL_CONTROL, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 Global Control Register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	*en = (MSD_U16)(tmpdata & 0x3FF);

	MSD_DBG_INFO(("Agate_gsysActiveIntEnableGet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gsysActiveIntStatusGet
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
MSD_STATUS Agate_gsysActiveIntStatusGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16      *status
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Agate_gsysActiveIntStatusGet Called.\n"));

	/*set G1 0x00 register, Switch Global Control Register*/
	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_GLOBAL_STATUS, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 Global Control Register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	*status = (MSD_U16)(tmpdata & 0x1FF);

	MSD_DBG_INFO(("Agate_gsysActiveIntStatusGet Exit.\n"));
	return MSD_OK;
}
