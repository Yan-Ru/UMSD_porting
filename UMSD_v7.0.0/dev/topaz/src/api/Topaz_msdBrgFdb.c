/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Topaz_msdBrgFdb.c
*
* DESCRIPTION:
*       API definitions for Multiple Forwarding Databases 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <topaz/include/api/Topaz_msdBrgFdb.h>
#include <topaz/include/api/Topaz_msdApiInternal.h>
#include <topaz/include/driver/Topaz_msdHwAccess.h>
#include <topaz/include/driver/Topaz_msdDrvSwRegs.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
#include <utils/msdUtils.h>

typedef struct 
{
    MSD_U32	moveFrom;
    MSD_U32	moveTo;
    MSD_U32	intCause;
    MSD_U32	reserved;
} TOPAZ_MSD_EXTRA_OP_DATA;


/*
* typedef: enum MSD_ATU_STATS_OP
*
* Description: Enumeration of the ATU Statistics operation
*
* Enumerations:
*   MSD_ATU_STATS_ALL        - count all valid entry
*   MSD_ATU_STATS_NON_STATIC - count all vaild non-static entry
*   MSD_ATU_STATS_ALL_FID    - count all valid entry in the given DBNum(or FID)
*   MSD_ATU_STATS_NON_STATIC_FID - count all valid non-static entry in the given DBNum(or FID)
*/
typedef enum
{
    TOPAZ_MSD_ATU_STATS_ALL = 0,
    TOPAZ_MSD_ATU_STATS_NON_STATIC,
    TOPAZ_MSD_ATU_STATS_ALL_FID,
    TOPAZ_MSD_ATU_STATS_NON_STATIC_FID
} TOPAZ_MSD_ATU_STATS_OP;


/*
*  typedef: struct MSD_ATU_STAT
*
*  Description:
*        This structure is used to count ATU entries.
*
*  Fields:
*      op       - counter type
*        DBNum - required only if op is either MSD_ATU_STATS_FID or
*                MSD_ATU_STATS_NON_STATIC_FID
*/
typedef struct
{
    TOPAZ_MSD_ATU_STATS_OP    op;
    MSD_U32             DBNum;
} TOPAZ_MSD_ATU_STAT;

/****************************************************************************/
/* Forward function declaration.                                            */
/****************************************************************************/
static MSD_STATUS Topaz_atuOperationPerform
(
    IN      MSD_QD_DEV           *dev,
    IN      TOPAZ_MSD_ATU_OPERATION    atuOp,
    INOUT   TOPAZ_MSD_EXTRA_OP_DATA    *opData,
    INOUT   TOPAZ_MSD_ATU_ENTRY        *atuEntry
);

static MSD_STATUS Topaz_atuGetStats
(
IN  MSD_QD_DEV    *dev,
IN  TOPAZ_MSD_ATU_STAT    *atuStat,
OUT MSD_U32        *count
);

/*******************************************************************************
* Topaz_gfdbGetAtuEntryNext
*
* DESCRIPTION:
*       Gets next lexicographic MAC address from the specified Mac Addr.
*
* INPUTS:
*       atuEntry - the Mac Address to start the search.
*
* OUTPUTS:
*       atuEntry - match Address translate unit entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Search starts from atu.macAddr[xx:xx:xx:xx:xx:xx] specified by the
*       user.
*
*       DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetAtuEntryNext
(
    IN MSD_QD_DEV    *dev,
    INOUT TOPAZ_MSD_ATU_ENTRY  *atuEntry
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_ENTRY    entry;

    MSD_DBG_INFO(("Topaz_gfdbGetAtuEntryNext Called.\n"));
	
	if(atuEntry->DBNum > 0xff)
	{
		MSD_DBG_ERROR(("Bad DBNum %d.\n", atuEntry->DBNum));
        return MSD_BAD_PARAM;
	}
    msdMemCpy(entry.macAddr.arEther,atuEntry->macAddr.arEther,6);

    entry.DBNum = atuEntry->DBNum;

    retVal = Topaz_atuOperationPerform(dev,TOPAZ_GET_NEXT_ENTRY,NULL,&entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Topaz_atuOperationPerform TOPAZ_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if(TOPAZ_IS_BROADCAST_MAC(entry.macAddr))
    {
        if(entry.entryState == 0)
        {
            MSD_DBG_INFO(("No more valid Entry found!.\n"));
            MSD_DBG_INFO(("Topaz_gfdbGetAtuEntryNext Exit.\n"));
            return MSD_NO_SUCH;
        }
    }

    msdMemCpy(atuEntry->macAddr.arEther,entry.macAddr.arEther,6);
    atuEntry->portVec   = MSD_PORTVEC_2_LPORTVEC(entry.portVec);
    atuEntry->trunkMember = entry.trunkMember;
    atuEntry->exPrio.macFPri = entry.exPrio.macFPri;

	atuEntry->entryState = entry.entryState;
    MSD_DBG_INFO(("Topaz_gfdbGetAtuEntryNext Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbFlush
*
* DESCRIPTION:
*       This routine flush all or unblocked addresses from the MAC Address
*       Table.
*
* INPUTS:
*       flushCmd - the flush operation type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbFlush
(
    IN MSD_QD_DEV    *dev,
    IN TOPAZ_MSD_FLUSH_CMD flushCmd
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_ENTRY    entry;

    MSD_DBG_INFO(("Topaz_gfdbFlush Called.\n"));

    entry.DBNum = 0;
    entry.entryState = 0;

    if(flushCmd == TOPAZ_MSD_FLUSH_ALL)
        retVal = Topaz_atuOperationPerform(dev,TOPAZ_FLUSH_ALL,NULL,&entry);
    else if(flushCmd == TOPAZ_MSD_FLUSH_ALL_NONSTATIC)
        retVal = Topaz_atuOperationPerform(dev,TOPAZ_FLUSH_NONSTATIC,NULL,&entry);
    else
    {
        MSD_DBG_ERROR(("Bad Param: TOPAZ_MSD_FLUSH_CMD is %d\n", flushCmd));
        return MSD_FAIL;
    }

    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_atuOperationPerform TOPAZ_FLUSH returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbFlush Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Topaz_gfdbFlushInDB
*
* DESCRIPTION:
*       This routine flush all or unblocked addresses from the particular
*       ATU Database (DBNum). If multiple address databases are being used, this
*       API can be used to flush entries in a particular DBNum database.
*
* INPUTS:
*       flushCmd  - the flush operation type.
*       DBNum     - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbFlushInDB
(
    IN MSD_QD_DEV    *dev,
    IN TOPAZ_MSD_FLUSH_CMD flushCmd,
    IN MSD_U32 DBNum
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_ENTRY    entry;

    MSD_DBG_INFO(("Topaz_gfdbFlushInDB Called.\n"));
	
	if(DBNum > 0xff)
	{
		MSD_DBG_ERROR(("Bad DBNum: %d.\n", (MSD_U16)DBNum));
        return MSD_BAD_PARAM;
	}

    entry.DBNum = (MSD_U16)DBNum;
    entry.entryState = 0;

    if(flushCmd == TOPAZ_MSD_FLUSH_ALL)
        retVal = Topaz_atuOperationPerform(dev,TOPAZ_FLUSH_ALL_IN_DB,NULL,&entry);
    else if(flushCmd == TOPAZ_MSD_FLUSH_ALL_NONSTATIC)
        retVal = Topaz_atuOperationPerform(dev,TOPAZ_FLUSH_NONSTATIC_IN_DB,NULL,&entry);
    else
    {
        MSD_DBG_ERROR(("Bad Param: TOPAZ_MSD_FLUSH_CMD is %d\n", flushCmd));
        return MSD_FAIL;
    }

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Topaz_atuOperationPerform TOPAZ_FLUSH_IN_DB returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbFlushInDB Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbMove
*
* DESCRIPTION:
*        This routine moves all or unblocked addresses from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbMove
(
    IN MSD_QD_DEV    *dev,
    IN TOPAZ_MSD_MOVE_CMD  moveCmd,
    IN MSD_LPORT        moveFrom,
    IN MSD_LPORT        moveTo
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_ENTRY    entry;
    TOPAZ_MSD_EXTRA_OP_DATA    opData;

    MSD_DBG_INFO(("Topaz_gfdbMove Called.\n"));

    entry.DBNum = 0;
    entry.entryState = 0xF;
    if (moveTo == 0xF)
        opData.moveTo = moveTo;
    else
        opData.moveTo = (MSD_U32)MSD_LPORT_2_PORT(moveTo);
    opData.moveFrom = (MSD_U32)MSD_LPORT_2_PORT(moveFrom);

    if((opData.moveTo == MSD_INVALID_PORT) || (opData.moveFrom == MSD_INVALID_PORT))
    {
		MSD_DBG_ERROR(("Bad Port: moveto %u, moveFrom %u .\n", (unsigned int)(opData.moveTo), (unsigned int)(opData.moveFrom)));
        return MSD_BAD_PARAM;
    }

	/* move and flush share the same command value, but with different entryState.*/
    if(moveCmd == TOPAZ_MSD_MOVE_ALL)
        retVal = Topaz_atuOperationPerform(dev,TOPAZ_FLUSH_ALL,&opData,&entry);
    else if (moveCmd == TOPAZ_MSD_MOVE_ALL_NONSTATIC)
        retVal = Topaz_atuOperationPerform(dev,TOPAZ_FLUSH_NONSTATIC,&opData,&entry);
    else
    {
        MSD_DBG_ERROR(("Bad Param: TOPAZ_MSD_MOVE_CMD is %d\n", moveCmd));
        return MSD_FAIL;
    }

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Topaz_atuOperationPerform TOPAZ_MOVE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbMove Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Topaz_gfdbMoveInDB
*
* DESCRIPTION:
*        This routine move all or unblocked addresses which are in the particular
*        ATU Database (DBNum) from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        DBNum         - ATU MAC Address Database Number.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*     None
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		 none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbMoveInDB
(
    IN MSD_QD_DEV     *dev,
    IN TOPAZ_MSD_MOVE_CMD   moveCmd,
    IN MSD_U32        DBNum,
    IN MSD_LPORT      moveFrom,
    IN MSD_LPORT      moveTo
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_ENTRY    entry;
    TOPAZ_MSD_EXTRA_OP_DATA    opData;

    MSD_DBG_INFO(("Topaz_gfdbMoveInDB Called.\n"));

	if(DBNum > 0xff)
	{
		MSD_DBG_ERROR(("Bad DBNum: %d.\n", (MSD_U16)DBNum));
        return MSD_BAD_PARAM;
	}
    entry.DBNum = (MSD_U16)DBNum;
    entry.entryState = 0xF;

    if (moveTo == 0xF)
        opData.moveTo = moveTo;
    else
        opData.moveTo = (MSD_U32)MSD_LPORT_2_PORT(moveTo);
    opData.moveFrom = (MSD_U32)MSD_LPORT_2_PORT(moveFrom);

    if((opData.moveTo == MSD_INVALID_PORT) || (opData.moveFrom == MSD_INVALID_PORT))
    {
		MSD_DBG_ERROR(("Bad Port: moveto %u, moveFrom %u .\n", (unsigned int)(opData.moveTo), (unsigned int)(opData.moveFrom)));
        return MSD_BAD_PARAM;
    }

    if(moveCmd == TOPAZ_MSD_MOVE_ALL)
        retVal = Topaz_atuOperationPerform(dev,TOPAZ_FLUSH_ALL_IN_DB,&opData,&entry);
    else if (moveCmd == TOPAZ_MSD_MOVE_ALL_NONSTATIC)
        retVal = Topaz_atuOperationPerform(dev,TOPAZ_FLUSH_NONSTATIC_IN_DB,&opData,&entry);
    else
    {
        MSD_DBG_ERROR(("Bad Param: TOPAZ_MSD_MOVE_CMD is %d\n", moveCmd));
        return MSD_FAIL;
    }

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Topaz_atuOperationPerform TOPAZ_MOVE_IN_DB returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbMoveInDB Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbAddMacEntry
*
* DESCRIPTION:
*       Creates the new entry in MAC address table.
*
* INPUTS:
*       macEntry    - mac address entry to insert to the ATU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       DBNum in macEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbAddMacEntry
(
    IN MSD_QD_DEV    *dev,
    IN TOPAZ_MSD_ATU_ENTRY *macEntry
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_ENTRY    entry;

    MSD_DBG_INFO(("Topaz_gfdbAddMacEntry Called.\n"));

	if(macEntry->DBNum > 0xff)
	{
		MSD_DBG_ERROR(("Bad DBNum: %d.\n", (MSD_U16)macEntry->DBNum));
        return MSD_BAD_PARAM;
	}
    /* If this is trunk entry, we need to check trunkId range, it should be within [0, 0xF]*/
    if (MSD_TRUE == macEntry->trunkMember && 0 == TOPAZ_IS_TRUNK_ID_VALID(dev, macEntry->portVec))
    {
        MSD_DBG_ERROR(("Bad TrunkId: %d. It should be within [0, 15].\n", (MSD_U16)macEntry->portVec));
        return MSD_BAD_PARAM;
    }

    msdMemCpy(entry.macAddr.arEther,macEntry->macAddr.arEther,6);
    entry.DBNum        = macEntry->DBNum;
    entry.portVec     = MSD_LPORTVEC_2_PORTVEC(macEntry->portVec);
    if(entry.portVec == MSD_INVALID_PORT_VEC)
    {
		MSD_DBG_ERROR(("Bad PortVec %x.\n", (unsigned int)macEntry->portVec));
        return MSD_BAD_PARAM;
    }
	
	entry.exPrio.macFPri = macEntry->exPrio.macFPri;
    entry.trunkMember = macEntry->trunkMember;

	entry.entryState = macEntry->entryState;

    if (entry.entryState == 0)
    {
		MSD_DBG_ERROR(("Bad entry state, Entry State should not be ZERO.\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Topaz_atuOperationPerform(dev,TOPAZ_LOAD_PURGE_ENTRY,NULL,&entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Topaz_atuOperationPerform TOPAZ_LOAD_PURGE_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbAddMacEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbDelAtuEntry
*
* DESCRIPTION:
*       Deletes ATU entry.
*
* INPUTS:
*       macEntry - the ATU entry to be deleted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK           - on success
*       MSD_FAIL         - on error
*       MSD_NO_SUCH      - if specified address entry does not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbDelAtuEntry
(
    IN MSD_QD_DEV     *dev,
    IN MSD_ETHERADDR  *macAddr,
    IN MSD_U32   fid
)
{
    TOPAZ_MSD_ATU_ENTRY    entry;
    MSD_STATUS retVal;

    MSD_DBG_INFO(("Topaz_gfdbDelAtuEntry Called.\n"));

    if (NULL == macAddr)
    {
        MSD_DBG_ERROR(("Input param MSD_ETHERADDR in Topaz_gfdbDelAtuEntry is NULL. \n"));
        return MSD_BAD_PARAM;
    }
    msdMemCpy(entry.macAddr.arEther, macAddr, 6);
    entry.DBNum = (MSD_U16)fid;
    entry.portVec = 0;
    entry.entryState = 0;
    entry.trunkMember = MSD_FALSE;
    entry.exPrio.macFPri = 0;

    retVal = Topaz_atuOperationPerform(dev,TOPAZ_LOAD_PURGE_ENTRY,NULL,&entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Topaz_atuOperationPerform TOPAZ_LOAD_PURGE_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    MSD_DBG_INFO(("Topaz_gfdbDelAtuEntry Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbGetViolation
*
* DESCRIPTION:
*       Get ATU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       atuIntStatus - interrupt cause, source portID, and vid.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetViolation
(
    IN  MSD_QD_DEV         *dev,
    OUT TOPAZ_MSD_ATU_INT_STATUS *atuIntStatus
)
{
    MSD_U16              intCause;
    MSD_STATUS           retVal;
    TOPAZ_MSD_ATU_ENTRY        entry;
    TOPAZ_MSD_EXTRA_OP_DATA    opData;

    MSD_DBG_INFO(("Topaz_gfdbGetViolation Called.\n"));

    /* check which Violation occurred */
    retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_GLOBAL_STATUS,3,1,&intCause);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("ERROR to read ATU OPERATION Register.\n"));
        return retVal;
    }

    if (!intCause)
    {
        /* No Violation occurred. */
        atuIntStatus->atuIntCause = 0;
		MSD_DBG_INFO(("Topaz_gfdbGetViolation Exit, No Violation occurred.\n"));
        return MSD_OK;
    }

    entry.DBNum = 0;

    retVal = Topaz_atuOperationPerform(dev,TOPAZ_SERVICE_VIOLATIONS,&opData,&entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Topaz_atuOperationPerform TOPAZ_SERVICE_VIOLATIONS returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    msdMemCpy(atuIntStatus->macAddr.arEther,entry.macAddr.arEther,6);

    atuIntStatus->atuIntCause = (MSD_U16)opData.intCause;
    atuIntStatus->spid = entry.entryState;
    atuIntStatus->dbNum = msdShort2Char(entry.DBNum);

    if(atuIntStatus->spid != 0xF)
        atuIntStatus->spid = (MSD_U8)MSD_PORT_2_LPORT(atuIntStatus->spid);

    MSD_DBG_INFO(("Topaz_gfdbGetViolation Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbFindAtuMacEntry
*
* DESCRIPTION:
*       Find FDB entry for specific MAC address from the ATU.
*
* INPUTS:
*       atuEntry - the Mac address to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       atuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM  - on bad parameter
*
* COMMENTS:
*        DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbFindAtuMacEntry
(
	IN MSD_QD_DEV    *dev,
	INOUT TOPAZ_MSD_ATU_ENTRY  *atuEntry,
	OUT MSD_BOOL         *found
)
{
	MSD_STATUS       retVal;
	TOPAZ_MSD_ATU_ENTRY    entry;
	int           i;

	MSD_DBG_INFO(("Topaz_gfdbFindAtuMacEntry Called.\n"));

	if (NULL == found)
	{
		MSD_DBG_ERROR(("Input param MSD_BOOL found is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	*found = MSD_FALSE;
	msdMemCpy(entry.macAddr.arEther, atuEntry->macAddr.arEther, 6);
	entry.DBNum = atuEntry->DBNum;
	/* Decrement 1 from mac address.    */
	for (i = 5; i >= 0; i--)
	{
		if (entry.macAddr.arEther[i] != 0)
		{
			entry.macAddr.arEther[i] -= 1;
			break;
		}
		else
			entry.macAddr.arEther[i] = 0xFF;
	}

	retVal = Topaz_gfdbGetAtuEntryNext(dev, &entry);
    if (MSD_NO_SUCH == retVal)
    {
        MSD_DBG_INFO(("Topaz_gfdbFindAtuMacEntry Exit, Not found valid entry.\n"));
        return MSD_OK;
    }

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_gfdbGetAtuEntryNext returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* If no valid entry found, INOUT param atuEntry will remain the original value, not overrided with ff:ff:ff:ff:ff:ff(entryState=0) */
	if (msdMemCmp((char*)atuEntry->macAddr.arEther, (char*)entry.macAddr.arEther, MSD_ETHERNET_HEADER_SIZE))
	{
        MSD_DBG_INFO(("Topaz_gfdbFindAtuMacEntry Exit, Not found this entry.\n"));
        return MSD_OK;
	}

	atuEntry->portVec = MSD_PORTVEC_2_LPORTVEC(entry.portVec);
	atuEntry->trunkMember = entry.trunkMember;
	atuEntry->exPrio.macFPri = entry.exPrio.macFPri;
	atuEntry->entryState = entry.entryState;

	*found = MSD_TRUE;
	MSD_DBG_INFO(("Topaz_gfdbFindAtuMacEntry Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbSetAgingTimeout
*
* DESCRIPTION:
*        Sets the timeout period in milliseconds for aging out dynamically learned
*        forwarding information. The standard recommends 300 sec.
*        For Topaz, the time-base is 3750 milliseconds, so it can be set to 3750,
*        3750*2, 3750*3,...3750*0xff (almost 16 minites)
*        Since in this API, parameter 'timeout' is milliseconds based,we will set the value
*        rounded to the nearest supported value smaller than the given timeout.
*        If the given timeout is less than 3750, minimum timeout value
*        3750 will be used instead. E.g.) 3800 becomes 3750 and 3000 becomes 3750.
*
* INPUTS:
*       timeout - aging time in seconds.
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
MSD_STATUS Topaz_gfdbSetAgingTimeout
(
	IN MSD_QD_DEV    *dev,
	IN MSD_U32 timeout
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U32       timeBase;

	MSD_DBG_INFO(("Topaz_gfdbSetAgingTimeout Called.\n"));

	timeBase = 3750;

    if (timeout > (0xFF * timeBase))
    {
        MSD_DBG_ERROR(("Bad Param: timeout is %d mSec. It should be within 0xFF*%d mSec\n", timeout, timeBase));
        return MSD_BAD_PARAM;
    }

	if ((timeout < timeBase) && (timeout != 0))
	{
		data = 1;
	}
	else
	{
		data = (MSD_U16)(timeout / timeBase);
		if (data & 0xFF00)
			data = 0xFF;
	}

	/* Set the Time Out value.              */
	retVal = msdSetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR, TOPAZ_QD_REG_ATU_CTRL_REG, 4, 8, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyRegField returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Topaz_gfdbSetAgingTimeout Exit.\n"));
	return MSD_OK;
}

/* Begin New Add */
/*******************************************************************************
* Topaz_gfdbGetAgingTimeout
*
* DESCRIPTION:
*        Gets the timeout period in milliseconds for aging out dynamically learned
*        forwarding information. The returned value may not be the same as the value
*        programmed with <gfdbSetAgingTimeout>. Please refer to the description of
*        <gfdbSetAgingTimeout>.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       timeout - aging time in milliseconds.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetAgingTimeout
(
IN  MSD_QD_DEV  *dev,
IN  MSD_U32  *timeout
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32       timeBase;

    MSD_DBG_INFO(("Topaz_gfdbGetAgingTimeout Called.\n"));

    timeBase = 3750;

    /* Set the Time Out value.              */
    retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL1_DEV_ADDR, TOPAZ_QD_REG_ATU_CTRL_REG, 4, 8, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdSetAnyRegField returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    *timeout = (MSD_U32)(data*timeBase);

    MSD_DBG_INFO(("Topaz_gfdbGetAgingTimeout Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Topaz_gfdbGetLearn2All
*
* DESCRIPTION:
*        When more than one Marvell device is used to form a single 'switch', it
*        may be desirable for all devices in the 'switch' to learn any address this
*        device learns. When this bit is set to a one all other devices in the
*        'switch' learn the same addresses this device learns. When this bit is
*        cleared to a zero, only the devices that actually receive frames will learn
*        from those frames. This mode typically supports more active MAC addresses
*        at one time as each device in the switch does not need to learn addresses
*        it may nerver use.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode  - MSD_TRUE if Learn2All is enabled, MSD_FALSE otherwise
*
* RETURNS:
*        MSD_OK           - on success
*        MSD_FAIL         - on error
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetLearn2All
(
IN  MSD_QD_DEV  *dev,
OUT MSD_BOOL  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gfdbGetLearn2All Called.\n"));

    /* Get the Learn2All. */
    data = 0;
    retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL1_DEV_ADDR, TOPAZ_QD_REG_ATU_CTRL_REG, 3, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdGetAnyRegField for TOPAZ_QD_REG_ATU_CTRL_REG returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Topaz_gfdbGetLearn2All Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Topaz_gfdbSetLearn2All
*
* DESCRIPTION:
*        Enable or disable Learn2All mode.
*
* INPUTS:
*        mode - MSD_TRUE to set Learn2All, MSD_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbSetLearn2All
(
IN  MSD_QD_DEV  *dev,
IN  MSD_BOOL  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Topaz_gfdbSetLearn2All Called.\n"));

    /* Set the Learn2All. */
    MSD_BOOL_2_BIT(mode, data);
    retVal = msdSetAnyRegField(dev->devNum, TOPAZ_GLOBAL1_DEV_ADDR, TOPAZ_QD_REG_ATU_CTRL_REG, 3, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdSetAnyRegField for TOPAZ_QD_REG_ATU_CTRL_REG returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbSetLearn2All Exit.\n"));
    return retVal;
}


/*******************************************************************************
* Topaz_gfdbSetPortLearnLimit
*
* DESCRIPTION:
*        Set auto learning limit for specified port of a specified device.
*
*        When the limit is non-zero value, the number of unicast MAC addresses that
*        can be learned on this port are limited to the specified value. When the
*        learn limit has been reached any frame that ingresses this port with a
*        source MAC address not already in the address database that is associated
*        with this port will be discarded. Normal auto-learning will resume on the
*        port as soon as the number of active unicast MAC addresses associated to this
*        port is less than the learn limit.
*
*        When the limit is non-zero value, normal address learning and frame policy occurs.
*
*        This feature will not work when this port is configured as a LAG port.
*
*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports
*
* INPUTS:
*        port - the logical port number
*        limit - auto learning limit ( 0 ~ 255 )
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbSetPortLearnLimit
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  portNum,
IN  MSD_U32  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_BOOL         isLAG;
    MSD_U16          data;
    MSD_BOOL         isKeepOldLearnLimit;

    MSD_DBG_INFO(("Topaz_gfdbSetPortLearnLimit Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port %u.\n", (unsigned int)portNum));
        return MSD_BAD_PARAM;
    }

    /* Check if limit is out of range */
    if (limit > TOPAZ_MAX_ATU_PORT_LEARNLIMIT)
    {
        MSD_DBG_ERROR(("Learn Limit %u our of range. It should be within [0, %u].\n", (unsigned int)limit, TOPAZ_MAX_ATU_PORT_LEARNLIMIT));
        return MSD_BAD_PARAM;
    }

    /* Check if port is in LAG mode, if so, not support port learnLimit */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr,TOPAZ_QD_REG_PORT_CONTROL1, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdGetAnyRegField for TOPAZ_QD_REG_PORT_CONTROL1 returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, isLAG);

    if (MSD_TRUE == isLAG)
    {
        MSD_DBG_ERROR(("Port %u is in LAG Mode, LearnLimit not work! \n", (unsigned int)portNum));
        return MSD_FAIL;
    }

    /* read the old keepOldLearnLimit bit. */
    retVal = msdGetAnyReg(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdGetAnyReg for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    isKeepOldLearnLimit = (MSD_BOOL)(MSD_BF_GET(data, 12, 1));

    /* Set ReadLearnCnt (bit15) to 0; Set KeepOldLearnLimit (bit12) to 0; set limit*/
    data &= 0x6000;
    data |= limit;
    retVal = msdSetAnyReg(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdSetAnyReg for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* Set back KeepOldLearnLimit */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, 12, 1, (MSD_U16)isKeepOldLearnLimit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdSetAnyRegField for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbSetPortLearnLimit Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbGetPortLearnLimit
*
* DESCRIPTION:
*        Get auto learning limit for specified port of a specified device.
*
*        When the limit is non-zero value, the number of unicast MAC addresses that
*        can be learned on this port are limited to the specified value. When the
*        learn limit has been reached any frame that ingresses this port with a
*        source MAC address not already in the address database that is associated
*        with this port will be discarded. Normal auto-learning will resume on the
*        port as soon as the number of active unicast MAC addresses associated to this
*        port is less than the learn limit.
*
*        When the limit is non-zero value, normal address learning and frame policy occurs.
*
*        This feature will not work when this port is configured as a LAG port.
*
*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports        
*
* INPUTS:
*        port - the logical port number
*        limit - auto learning limit ( 0 ~ 255 )
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetPortLearnLimit
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  portNum,
OUT MSD_U32  *limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;
    MSD_BOOL         isKeepOldLearnLimit;

    MSD_DBG_INFO(("Topaz_gfdbGetPortLearnLimit Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port %u.\n", (unsigned int)portNum));
        return MSD_BAD_PARAM;
    }

    /* read the old keepOldLearnLimit bit. */
    retVal = msdGetAnyReg(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdGetAnyReg for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    isKeepOldLearnLimit = (MSD_BOOL)(MSD_BF_GET(data, 12, 1));

    /* Set ReadLearnCnt (bit15) to 0; Set KeepOldLearnLimit (bit12) to 1;*/
    data &= 0x6000;
    data |= 0x1000;
    retVal = msdSetAnyReg(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdSetAnyReg for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    /* Get back limit*/
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, 0, 10, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdGetAnyRegField for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    *limit = data;

    /* Set back KeepOldLearnLimit */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, 12, 1, (MSD_U16)isKeepOldLearnLimit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdSetAnyRegField for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbGetPortLearnLimit Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Topaz_gfdbGetPortLearnCount
*
* DESCRIPTION:
*        Read the current number of active unicast MAC addresses associated with
*        the given port. This counter (LearnCnt) is held at zero if learn limit
*        (gfdbSetPortAtuLearnLimit API) is set to zero.
*
* INPUTS:
*       port  - logical port number
*
* OUTPUTS:
*       count - current auto learning count
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetPortLearnCount
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT  portNum,
IN  MSD_U32  *count
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;

    MSD_DBG_INFO(("Topaz_gfdbGetPortLearnCount Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = TOPAZ_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port %u.\n", (unsigned int)portNum));
        return MSD_BAD_PARAM;
    }

	/* Set ReadLearnCnt 1 to access LearnCnt*/
	retVal = msdSetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, 15, 1, 1);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyRegField for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
    /* Get back learnCnt*/
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, TOPAZ_QD_REG_PORT_ATU_CONTROL, 0, 10, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdGetAnyRegField for TOPAZ_QD_REG_PORT_ATU_CONTROL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    *count = data;

    MSD_DBG_INFO(("Topaz_gfdbGetPortLearnCount Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Topaz_gfdbGetEntryCount
*
* DESCRIPTION:
*       Counts all entries in the Address Translation Unit.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetEntryCount
(
IN  MSD_QD_DEV  *dev,
OUT MSD_U32  *count
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_STAT        atuStat;

    MSD_DBG_INFO(("Topaz_gfdbGetEntryCount Called.\n"));

    atuStat.op = TOPAZ_MSD_ATU_STATS_ALL;
    retVal = Topaz_atuGetStats(dev, &atuStat, count);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_atuGetStats returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbGetEntryCount Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbGetEntryCountPerFid
*
* DESCRIPTION:
*       Counts all entries in the specified fid in Address Translation Unit.
*
* INPUTS:
*       fid - DBNum
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetEntryCountPerFid
(
IN  MSD_QD_DEV  *dev,
IN  MSD_U32  fid,
OUT MSD_U32  *count
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_STAT        atuStat;

    MSD_DBG_INFO(("Topaz_gfdbGetEntryCountPerFid Called.\n"));

    atuStat.op = TOPAZ_MSD_ATU_STATS_ALL_FID;
    atuStat.DBNum = fid;
    retVal = Topaz_atuGetStats(dev, &atuStat, count);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_atuGetStats returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbGetEntryCountPerFid Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Topaz_gfdbGetNonStaticEntryCount
*
* DESCRIPTION:
*       Counts all non-static entries in the Address Translation Unit.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetNonStaticEntryCount
(
IN  MSD_QD_DEV  *dev,
OUT MSD_U32  *count
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_STAT        atuStat;

    MSD_DBG_INFO(("Topaz_gfdbGetNonStaticEntryCount Called.\n"));

    atuStat.op = TOPAZ_MSD_ATU_STATS_NON_STATIC;
    retVal = Topaz_atuGetStats(dev, &atuStat, count);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_atuGetStats returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbGetNonStaticEntryCount Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Topaz_gfdbGetNonStaticEntryCountPerFid
*
* DESCRIPTION:
*       Counts all non-static entries in the specified fid in the Address Translation Unit.
*
* INPUTS:
*       fid - DBNum
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gfdbGetNonStaticEntryCountPerFid
(
IN  MSD_QD_DEV  *dev,
IN  MSD_U32  fid,
OUT MSD_U32  *count
)
{
    MSD_STATUS       retVal;
    TOPAZ_MSD_ATU_STAT        atuStat;

    MSD_DBG_INFO(("Topaz_gfdbGetNonStaticEntryCountPerFid Called.\n"));

    atuStat.op = TOPAZ_MSD_ATU_STATS_NON_STATIC_FID;
    atuStat.DBNum = fid;
    retVal = Topaz_atuGetStats(dev, &atuStat, count);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Topaz_atuGetStats returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Topaz_gfdbGetNonStaticEntryCountPerFid Exit.\n"));
    return MSD_OK;
}

/* End New Add */

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* Topaz_atuOperationPerform
*
* DESCRIPTION:
*       This function is used by all ATU control functions, and is responsible
*       to write the required operation into the ATU registers.
*
* INPUTS:
*       atuOp       - The ATU operation bits to be written into the ATU
*                     operation register.
*       DBNum       - ATU Database Number for CPU accesses
*       entryPri    - The EntryPri field in the ATU Data register.
*       portVec     - The portVec field in the ATU Data register.
*       entryState  - The EntryState field in the ATU Data register.
*       atuMac      - The Mac address to be written to the ATU Mac registers.
*
* OUTPUTS:
*       entryPri    - The EntryPri field in case the atuOp is GetNext.
*       portVec     - The portVec field in case the atuOp is GetNext.
*       entryState  - The EntryState field in case the atuOp is GetNext.
*       atuMac      - The returned Mac address in case the atuOp is GetNext.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*       1.  if atuMac == NULL, nothing needs to be written to ATU Mac registers.
*
*******************************************************************************/
static MSD_STATUS Topaz_atuOperationPerform
(
    IN       MSD_QD_DEV           *dev,
    IN       TOPAZ_MSD_ATU_OPERATION    atuOp,
    INOUT    TOPAZ_MSD_EXTRA_OP_DATA    *opData,
    INOUT    TOPAZ_MSD_ATU_ENTRY        *entry
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* Data to be set into the      */
                                    /* register.                    */
    MSD_U16          opcodeData;           /* Data to be set into the      */
                                    /* register.                    */
    MSD_U8           i;
    MSD_U16          portMask;

    msdSemTake(dev->devNum, dev->atuRegsSem,OS_WAIT_FOREVER);

    portMask = (1 << dev->maxPorts) - 1;

    /* Wait until the ATU in ready. */
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
    }

    opcodeData = 0;

    switch (atuOp)
    {
        case TOPAZ_LOAD_PURGE_ENTRY:
            /* portVec represents trunk ID */
			if(entry->trunkMember)
				data = (MSD_U16)( 0x8000 | (((entry->portVec) & portMask) << 4) |
					 (((entry->entryState) & 0xF)) );
			else
				data = (MSD_U16)((((entry->portVec) & portMask) << 4) |
					 (((entry->entryState) & 0xF)) );
			opcodeData |= (entry->exPrio.macFPri & 0x7) << 8;
			retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_DATA_REG,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->atuRegsSem);
				return retVal;
			}
			/* pass thru */

        case TOPAZ_GET_NEXT_ENTRY:
			for(i = 0; i < 3; i++)
			{
				data=(entry->macAddr.arEther[2*i] << 8)|(entry->macAddr.arEther[1 + 2*i]);
				retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,(MSD_U8)(TOPAZ_QD_REG_ATU_MAC_BASE+i),data);
				if(retVal != MSD_OK)
				{
					msdSemGive(dev->devNum, dev->atuRegsSem);
					return retVal;
				}
			}

			break;

        case TOPAZ_FLUSH_ALL:
        case TOPAZ_FLUSH_NONSTATIC:
        case TOPAZ_FLUSH_ALL_IN_DB:
        case TOPAZ_FLUSH_NONSTATIC_IN_DB:
			if (entry->entryState == 0xF)
			{
				data = (MSD_U16)(0xF | ((opData->moveFrom & 0xF) << 4) | ((opData->moveTo & 0xF) << 8)); 
			}
			else
			{
				data = 0;
			}
			retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_DATA_REG,data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->atuRegsSem);
				return retVal;
			}
			break;

        case TOPAZ_SERVICE_VIOLATIONS:
			break;

        default:
			msdSemGive(dev->devNum, dev->atuRegsSem);
            return MSD_FAIL;
    }

    /* Set DBNum */
	retVal = msdSetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_FID_REG,0,8,(MSD_U16)(entry->DBNum & 0xFF));
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->atuRegsSem);
		return retVal;
	}

    /* Set the ATU Operation register in addtion to DBNum setup  */
	retVal = msdGetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_OPERATION,&data);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->atuRegsSem);
		return retVal;
	}
	data &= 0x0fff;
	if(atuOp == TOPAZ_LOAD_PURGE_ENTRY)
	{
		data &= 0x0ff;
	}
	opcodeData |= ((1 << 15) | (atuOp << 12) | data);

    retVal = msdSetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_OPERATION,opcodeData);
    if(retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    /* If the operation is to service violation operation wait for the response   */
    if(atuOp == TOPAZ_SERVICE_VIOLATIONS)
    {
        /* Wait until the VTU in ready. */
        data = 1;
        while(data == 1)
        {
            retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_OPERATION,15,1,&data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return retVal;
            }
        }


        /* get the Interrupt Cause */
        retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_OPERATION,4,4,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

        switch (data)
        {
            case 8:    /* Age Interrupt */
                opData->intCause = TOPAZ_MSD_ATU_AGE_OUT_VIOLATION;
                break;
            case 4:    /* Member Violation */
                opData->intCause = TOPAZ_MSD_ATU_MEMBER_VIOLATION;
                break;
            case 2:    /* Miss Violation */
                opData->intCause = TOPAZ_MSD_ATU_MISS_VIOLATION;
                break;
            case 1:    /* Full Violation */
                opData->intCause = TOPAZ_MSD_ATU_FULL_VIOLATION;
                break;
            default:
                opData->intCause = 0;
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return MSD_OK;
        }

        /* get the DBNum that was involved in the violation */

        entry->DBNum = 0;
		
		retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_FID_REG,0,8,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->atuRegsSem);
			return retVal;
		}
		entry->DBNum = (MSD_U16)data;


        /* get the Source Port ID that was involved in the violation */

        retVal = msdGetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_DATA_REG,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

        entry->entryState = (MSD_U8)(data & 0xF);

        /* Get the Mac address  */
        for(i = 0; i < 3; i++)
        {
            retVal = msdGetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,(MSD_U8)(TOPAZ_QD_REG_ATU_MAC_BASE+i),&data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return retVal;
            }
            entry->macAddr.arEther[2*i] = (MSD_U8)(data >> 8);
            entry->macAddr.arEther[1 + 2*i] = (MSD_U8)(data & 0xFF);
        }


    } /* end of service violations */
	
    /* If the operation is a gen next operation wait for the response   */
    if(atuOp == TOPAZ_GET_NEXT_ENTRY)
    {
        entry->trunkMember = MSD_FALSE;
        entry->exPrio.macFPri = 0;

        /* Wait until the ATU in ready. */
        data = 1;
        while(data == 1)
        {
            retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_OPERATION,15,1,&data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return retVal;
            }
        }

        /* Get the Mac address  */
        for(i = 0; i < 3; i++)
        {
            retVal = msdGetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,(MSD_U8)(TOPAZ_QD_REG_ATU_MAC_BASE+i),&data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return retVal;
            }
            entry->macAddr.arEther[2*i] = (MSD_U8)(data >> 8);
            entry->macAddr.arEther[1 + 2*i] = (MSD_U8)(data & 0xFF);
        }

        retVal = msdGetAnyReg(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_DATA_REG,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

        /* Get the Atu data register fields */
		entry->trunkMember = (data & 0x8000)?MSD_TRUE:MSD_FALSE;

		entry->portVec = (data >> 4) & portMask;
		entry->entryState = (MSD_U8)(data & 0xF);
		retVal = msdGetAnyRegField(dev->devNum,  TOPAZ_GLOBAL1_DEV_ADDR,TOPAZ_QD_REG_ATU_OPERATION,8,3,&data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->atuRegsSem);
			return retVal;
		}
        entry->exPrio.macFPri = (MSD_U8)data;
    }

    msdSemGive(dev->devNum, dev->atuRegsSem);
    return MSD_OK;
}


static MSD_STATUS Topaz_atuGetStats
(
IN  MSD_QD_DEV    *dev,
IN  TOPAZ_MSD_ATU_STAT    *atuStat,
OUT MSD_U32        *count
)
{
    MSD_U32          numOfEntries, dbNum;
    TOPAZ_MSD_ATU_ENTRY    entry;
    MSD_U16          data, mode, bin;
    MSD_STATUS       retVal;


    switch (atuStat->op)
    {
    case TOPAZ_MSD_ATU_STATS_ALL:
    case TOPAZ_MSD_ATU_STATS_NON_STATIC:
        dbNum = 0;
        break;
    case TOPAZ_MSD_ATU_STATS_ALL_FID:
    case TOPAZ_MSD_ATU_STATS_NON_STATIC_FID:
        dbNum = atuStat->DBNum;
        break;
    default:
        return MSD_FAIL;
    }

    numOfEntries = 0;
    mode = atuStat->op;

    for (bin = 0; bin<4; bin++)
    {
        data = (bin << 14) | (mode << 12);

        retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_ATU_STATS, data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }

        entry.DBNum = (MSD_U16)dbNum;
        msdMemSet(entry.macAddr.arEther, 0, sizeof(MSD_ETHERADDR));

        retVal = Topaz_atuOperationPerform(dev, TOPAZ_GET_NEXT_ENTRY, NULL, &entry);
        if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH))
        {
            return retVal;
        }

        retVal = msdSetAnyRegField(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_ATU_STATS, 14, 2, bin);
        if (retVal != MSD_OK)
        {
            return retVal;
        }

        retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, TOPAZ_QD_REG_ATU_STATS, 0, 12, &data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }

        numOfEntries += (data & 0xFFF);
    }

    *count = numOfEntries;

    return MSD_OK;
}
