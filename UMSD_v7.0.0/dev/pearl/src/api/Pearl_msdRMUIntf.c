/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Pearl_msdRMUIntf.c
*
* DESCRIPTION:
*       API definitions for control of Remote Management Unit
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <pearl/include/api/Pearl_msdRMU.h>


/*******************************************************************************
* msdRmuGetID
*
* DESCRIPTION:
*       Get the current Device ID.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       id - DUT Id.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Pearl_msdRmuGetIDIntf
(
IN  MSD_QD_DEV *dev,
OUT MSD_U16 *id
)
{
	return Pearl_msdRmuGetID(dev, id);
}

/*******************************************************************************
* msdRmuAtuDump
*
* DESCRIPTION:
*       Dump ATU entry from the specified starting address.
*
* INPUTS:
*       startAddr - starting address to search the valid ATU entryies
*
* OUTPUTS:
*       atuEntry - array of atuEntries.
*		numOfEntry - number of returned valued entries
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Pearl_msdRmuAtuDumpIntf
(
	IN MSD_QD_DEV *dev,
	INOUT MSD_U32 *startAddr,
	OUT MSD_U32 *numOfEntry,
	OUT MSD_ATU_ENTRY **atuEntry
)
{
	PEARL_MSD_ATU_ENTRY entry[MSD_RMU_MAX_ATUS];
	PEARL_MSD_ATU_ENTRY *entryPtr = &(entry[0]);
	MSD_STATUS retVal;
	int i;

	for (i = 0; i < MSD_RMU_MAX_ATUS; i++)
	{
		msdMemSet((void*)(entry+i), 0, sizeof(PEARL_MSD_ATU_ENTRY));
	}

	retVal = Pearl_msdRmuAtuDump(dev, startAddr, numOfEntry, &entryPtr);
	if (MSD_OK != retVal)
		return retVal;

	if ((*numOfEntry > 0) && (NULL == atuEntry))
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY** in Pearl_msdRmuAtuDumpIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	for (i = 0; i < (int)(*numOfEntry); i++)
	{
		if (NULL == ((*atuEntry)+i))
		{
			MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY* for index %d in Pearl_msdRmuAtuDumpIntf is NULL. \n", i));
			return MSD_BAD_PARAM;
		}
		msdMemSet((void*)(*atuEntry + i), 0, sizeof(MSD_ATU_ENTRY));

		((*atuEntry) + i)->fid = ((entryPtr) + i)->DBNum;
		((*atuEntry) + i)->portVec = ((entryPtr) + i)->portVec;
		((*atuEntry) + i)->entryState = ((entryPtr) + i)->entryState;
		((*atuEntry) + i)->exPrio.macFPri = ((entryPtr) + i)->exPrio.macFPri;
		((*atuEntry) + i)->trunkMemberOrLAG = ((entryPtr) + i)->trunkMember;
		msdMemCpy(((*atuEntry) + i)->macAddr.arEther, ((entryPtr) + i)->macAddr.arEther, 6);
	}

	return retVal;
}


/*******************************************************************************
* msdRmuMib2Dump
*
* DESCRIPTION:
*       Dump MIB from the specified port.
*
* INPUTS:
*       enClearOnRead - indicate dump mib clear or not
*		port - request port
*
* OUTPUTS:
*       statsCounterSet - mib info.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Pearl_msdRmuMib2DumpIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_BOOL enClearOnRead,
	IN MSD_LPORT port,
	OUT MSD_STATS_COUNTER_SET *statsCounterSet
)
{
	PEARL_MSD_STATS_COUNTER_SET  statsCounter;
	MSD_STATUS    retVal;

	retVal = Pearl_msdRmuMib2Dump(dev, enClearOnRead, port, &statsCounter);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == statsCounterSet)
	{
		MSD_DBG_ERROR(("Input param MSD_STATS_COUNTER_SET in Pearl_msdRmuMib2DumpIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET));

	statsCounterSet->InGoodOctetsLo = statsCounter.InGoodOctetsLo;
	statsCounterSet->InGoodOctetsHi = statsCounter.InGoodOctetsHi;
	statsCounterSet->InBadOctets = statsCounter.InBadOctets;
	statsCounterSet->OutFCSErr = statsCounter.OutFCSErr;
	statsCounterSet->InUnicasts = statsCounter.InUnicasts;
	statsCounterSet->Deferred = statsCounter.Deferred;
	statsCounterSet->InBroadcasts = statsCounter.InBroadcasts;
	statsCounterSet->InMulticasts = statsCounter.InMulticasts;

	statsCounterSet->Octets64 = statsCounter.Octets64;
	statsCounterSet->Octets127 = statsCounter.Octets127;
	statsCounterSet->Octets255 = statsCounter.Octets255;
	statsCounterSet->Octets511 = statsCounter.Octets511;
	statsCounterSet->Octets1023 = statsCounter.Octets1023;
	statsCounterSet->OctetsMax = statsCounter.OctetsMax;
	statsCounterSet->OutOctetsLo = statsCounter.OutOctetsLo;
	statsCounterSet->OutOctetsHi = statsCounter.OutOctetsHi;
	statsCounterSet->OutUnicasts = statsCounter.OutUnicasts;
	statsCounterSet->Excessive = statsCounter.Excessive;
	statsCounterSet->OutMulticasts = statsCounter.OutMulticasts;
	statsCounterSet->OutBroadcasts = statsCounter.OutBroadcasts;
	statsCounterSet->Single = statsCounter.Single;

	statsCounterSet->OutPause = statsCounter.OutPause;
	statsCounterSet->InPause = statsCounter.InPause;
	statsCounterSet->Multiple = statsCounter.Multiple;
	statsCounterSet->InUndersize = statsCounter.InUndersize;
	statsCounterSet->InFragments = statsCounter.InFragments;
	statsCounterSet->InOversize = statsCounter.InOversize;
	statsCounterSet->InJabber = statsCounter.InJabber;
	statsCounterSet->InRxErr = statsCounter.InRxErr;
	statsCounterSet->InFCSErr = statsCounter.InFCSErr;
	statsCounterSet->Collisions = statsCounter.Collisions;
	statsCounterSet->Late = statsCounter.Late;

	statsCounterSet->InDiscards = statsCounter.InDiscards;
	statsCounterSet->InFiltered = statsCounter.InFiltered;
	statsCounterSet->InAccepted = statsCounter.InAccepted;
	statsCounterSet->InBadAccepted = statsCounter.InBadAccepted;
	statsCounterSet->InGoodAvbClassA = statsCounter.InGoodAvbClassA;
	statsCounterSet->InGoodAvbClassB = statsCounter.InGoodAvbClassB;
	statsCounterSet->InBadAvbClassA = statsCounter.InBadAvbClassA;
	statsCounterSet->InBadAvbClassB = statsCounter.InBadAvbClassB;
	statsCounterSet->TCAMCounter0 = statsCounter.TCAMCounter0;
	statsCounterSet->TCAMCounter1 = statsCounter.TCAMCounter1;
	statsCounterSet->TCAMCounter2 = statsCounter.TCAMCounter2;
	statsCounterSet->TCAMCounter3 = statsCounter.TCAMCounter3;
	statsCounterSet->InDroppedAvbA = statsCounter.InDroppedAvbA;
	statsCounterSet->InDroppedAvbB = statsCounter.InDroppedAvbB;
	statsCounterSet->InDaUnknown = statsCounter.InDaUnknown;
	statsCounterSet->InMGMT = statsCounter.InMGMT;
	statsCounterSet->OutQueue0 = statsCounter.OutQueue0;
	statsCounterSet->OutQueue1 = statsCounter.OutQueue1;
	statsCounterSet->OutQueue2 = statsCounter.OutQueue2;
	statsCounterSet->OutQueue3 = statsCounter.OutQueue3;
	statsCounterSet->OutQueue4 = statsCounter.OutQueue4;
	statsCounterSet->OutQueue5 = statsCounter.OutQueue5;
	statsCounterSet->OutQueue6 = statsCounter.OutQueue6;
	statsCounterSet->OutQueue7 = statsCounter.OutQueue7;
	statsCounterSet->OutCutThrough = statsCounter.OutCutThrough;
	statsCounterSet->InBadQbv = statsCounter.reserved_19;
	statsCounterSet->OutOctetsA = statsCounter.OutOctetsA;
	statsCounterSet->OutOctetsB = statsCounter.OutOctetsB;

	statsCounterSet->OutDroppedYel = statsCounter.OutDroppedYel;
	statsCounterSet->OutDiscards = statsCounter.OutDiscards;
	statsCounterSet->OutMGMT = statsCounter.OutMGMT;


	return retVal;
}


/*******************************************************************************
* msdRmuReadRegister
*
* DESCRIPTION:
*       single register read.
*
* INPUTS:
*       devAddr - device address
*		regAddr - register address
*
* OUTPUTS:
*       data - return register value.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Pearl_msdRmuReadRegisterIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 devAddr,
	IN MSD_U8 regAddr,
	OUT MSD_U16 *data
)
{
	return Pearl_msdRmuReadRegister(dev,devAddr,regAddr,data);
}

/*******************************************************************************
* msdRmuWriteRegister
*
* DESCRIPTION:
*       single register write.
*
* INPUTS:
*       devAddr - device address
*		regAddr - register address
*		data - register value.
*
* OUTPUTS:
*       NONE
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Pearl_msdRmuWriteRegisterIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 devAddr,
	IN MSD_U8 regAddr,
	IN MSD_U16 data
)
{
	return Pearl_msdRmuWriteRegister(dev, devAddr, regAddr, data);
}

/*******************************************************************************
* msdRmuMultiRegAccess
*
* DESCRIPTION:
*       multiple register read/write.
*
* INPUTS:
*       RegCmd - array of Read/Write requests
*		nCmd - number of register access commands
*
* OUTPUTS:
*       RegCmd - update read-back register value if it has register read request
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error .
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Pearl_msdRmuMultiRegAccessIntf
(
IN MSD_QD_DEV *dev,
INOUT MSD_RegCmd **regCmdList,
IN MSD_U32 nCmd
)
{
	return Pearl_msdRmuMultiRegAccess(dev, regCmdList, nCmd);
}
