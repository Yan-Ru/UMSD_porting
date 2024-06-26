/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Agate_msdBrgStu.c
*
* DESCRIPTION:
*       API definitions for SID (VTU 802.1s Port State Information Database)
*        Translation Unit.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <agate/include/api/Agate_msdBrgStu.h>
#include <agate/include/api/Agate_msdApiInternal.h>
#include <agate/include/driver/Agate_msdHwAccess.h>
#include <agate/include/driver/Agate_msdDrvSwRegs.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
#include <utils/msdUtils.h>

/****************************************************************************/
/* Forward function declaration.                                            */
/****************************************************************************/

typedef enum
{
	AGATE_LOAD_PURGE_STU_ENTRY = 5,
	AGATE_GET_NEXT_STU_ENTRY = 6
} AGATE_MSD_STU_OPERATION;

static MSD_STATUS Agate_stuOperationPerform
(
IN        MSD_QD_DEV           *dev,
IN      AGATE_MSD_STU_OPERATION    stuOp,
INOUT   MSD_U8               *valid,
INOUT     AGATE_MSD_STU_ENTRY        *stuEntry
);

/*******************************************************************************
* Agate_gstuGetEntryNext
*
* DESCRIPTION:
*       Gets next valid STU entry from the specified SID.
*
* INPUTS:
*       stuEntry - the SID to start the search.
*
* OUTPUTS:
*       stuEntry - next STU entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_gstuGetEntryNext
(
IN  MSD_QD_DEV       *dev,
INOUT AGATE_MSD_STU_ENTRY  *stuEntry
)
{
	MSD_U8               valid;
	MSD_STATUS           retVal;
	MSD_U8               port;
	MSD_LPORT               lport;
	AGATE_MSD_STU_ENTRY        entry;

	MSD_DBG_INFO(("Agate_gstuGetEntryNext Called.\n"));

	if (stuEntry->sid > 0x3F)
	{
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
		return MSD_BAD_PARAM;
	}

	entry.sid = stuEntry->sid;
	valid = 0;

	retVal = Agate_stuOperationPerform(dev, AGATE_GET_NEXT_STU_ENTRY, &valid, &entry);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_stuOperationPerform AGATE_GET_NEXT_STU_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* retrieve the value from the operation */

	if ((entry.sid == 0x3F) && (valid == 0))
	{
		MSD_DBG_INFO(("Agate_gstuGetEntryNext Exit, MSD_NO_SUCH\n"));
		return MSD_NO_SUCH;
	}

	stuEntry->sid = entry.sid;

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		port = MSD_LPORT_2_PORT(lport);
		if(port>=MSD_MAX_SWITCH_PORTS )
        {
            MSD_DBG_ERROR(("Port Error Number:%d.\n",port));
			return MSD_BAD_PARAM;
        }
		stuEntry->portState[lport] = entry.portState[port];
	}

	MSD_DBG_INFO(("Agate_gstuGetEntryNext Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gstuAddEntry
*
* DESCRIPTION:
*       Creates or update the entry in STU table based on SID.
*
* INPUTS:
*       stuEntry    - stu entry to insert to the STU.
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
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_gstuAddEntry
(
IN  MSD_QD_DEV       *dev,
IN  AGATE_MSD_STU_ENTRY    *stuEntry
)
{
	MSD_U8               valid;
	MSD_STATUS           retVal;
	MSD_U8           port;
	MSD_LPORT           lport;
	AGATE_MSD_STU_ENTRY        entry;

	MSD_DBG_INFO(("Agate_gstuAddEntry Called.\n"));

	if (stuEntry->sid > 0x3F)
	{
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
		return MSD_BAD_PARAM;
	}

	entry.sid = stuEntry->sid;

	valid = 1; /* for load operation */

	for (port = 0; port<dev->maxPorts; port++)
	{
		lport = MSD_PORT_2_LPORT(port);
		if (lport == MSD_INVALID_PORT)
			entry.portState[port] = 0;
		else if (lport < MSD_MAX_SWITCH_PORTS)
			entry.portState[port] = stuEntry->portState[lport];
		else
		{
			MSD_DBG_ERROR(("Lport number error: %d.\n", lport));
			return MSD_BAD_PARAM;
		}
		
	}

	retVal = Agate_stuOperationPerform(dev, AGATE_LOAD_PURGE_STU_ENTRY, &valid, &entry);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_stuOperationPerform add entry returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gstuAddEntry Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Agate_gstuDelEntry
*
* DESCRIPTION:
*       Deletes STU entry specified by SID.
*
* INPUTS:
*       stuEntry - the STU entry to be deleted
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_gstuDelEntry
(
IN  MSD_QD_DEV       *dev,
IN  AGATE_MSD_STU_ENTRY    *stuEntry
)
{
	MSD_U8               valid;
	MSD_STATUS           retVal;
	AGATE_MSD_STU_ENTRY        entry;

	MSD_DBG_INFO(("Agate_gstuDelEntry Called.\n"));

	if (stuEntry->sid > 0x3F)
	{
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
		return MSD_BAD_PARAM;
	}

	entry.sid = stuEntry->sid;
	valid = 0; /* for delete operation */

	retVal = Agate_stuOperationPerform(dev, AGATE_LOAD_PURGE_STU_ENTRY, &valid, &entry);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_stuOperationPerform Delete entry returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	MSD_DBG_INFO(("Agate_gstuDelEntry Exit.\n"));
	return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

static MSD_STATUS Agate_stuSetSTUData
(
IN    MSD_QD_DEV           *dev,
IN    AGATE_MSD_STU_ENTRY        *entry
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data1, data2, data3;           /* Data to be set into the      */
	MSD_U16            nStuData = 0;

	data1 = data2 = data3 = 0;

	switch (dev->maxPorts)
	{
	case 11:
		data3 |= (entry->portState[10] & 3) << 10;
		/* pass through */
	case 10:
		data3 |= (entry->portState[9] & 3) << 6;
		/* pass through */
	case 9:
		data3 |= (entry->portState[8] & 3) << 2;
		nStuData++;

		/* pass through */
	case 8:
		data2 |= (entry->portState[7] & 3) << 14;
		/* pass through */
	case 7:
		data2 |= (entry->portState[6] & 3) << 10;
		/* pass through */
	case 6:
		data2 |= (entry->portState[5] & 3) << 6;
		/* pass through */
	case 5:
		data2 |= (entry->portState[4] & 3) << 2;
		nStuData++;

		/* pass through */
	case 4:
		data1 |= (entry->portState[3] & 3) << 14;
		/* pass through */
	case 3:
		data1 |= (entry->portState[2] & 3) << 10;
		/* pass through */
	case 2:
		data1 |= (entry->portState[1] & 3) << 6;
		/* pass through */
	case 1:
		data1 |= (entry->portState[0] & 3) << 2;
		nStuData++;
		break;

	default:
		return MSD_FAIL;
	}

	switch (nStuData)
	{
	case 3:
		retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_DATA3_REG, data3);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
		/* pass through */
	case 2:
		retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_DATA2_REG, data2);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
		/* pass through */
	case 1:
		retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_DATA1_REG, data1);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
		break;
	default:
		return MSD_FAIL;
	}

	return retVal;
}

static MSD_STATUS Agate_stuGetSTUData
(
IN     MSD_QD_DEV           *dev,
OUT    AGATE_MSD_STU_ENTRY        *entry
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data1, data2, data3;           /* Data to be set into the      */
	MSD_U16            nStuData = 0;

	data1 = data2 = data3 = 0;

	msdMemSet((void*)entry->portState, 0, sizeof(entry->portState));

	switch (dev->maxPorts)
	{
	case 11:
	case 10:
	case 9:
		nStuData = 3;
		break;

	case 8:
	case 7:
	case 6:
	case 5:
		nStuData = 2;
		break;

	case 4:
	case 3:
	case 2:
	case 1:
		nStuData = 1;
		break;

	default:
		return MSD_FAIL;
	}

	switch (nStuData)
	{
	case 3:
		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_DATA3_REG, &data3);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
		/* pass through */
	case 2:
		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_DATA2_REG, &data2);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
		/* pass through */
	case 1:
		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_DATA1_REG, &data1);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
		break;
	default:
		return MSD_FAIL;
	}
	switch (dev->maxPorts)
	{
	case 11:
		entry->portState[10] = (data3 >> 10) & 3;
		/* pass through */
	case 10:
		entry->portState[9] = (data3 >> 6) & 3;
		/* pass through */
	case 9:
		entry->portState[8] = (data3 >> 2) & 3;
		/* pass through */
	case 8:
		entry->portState[7] = (data2 >> 14) & 3;
		/* pass through */
	case 7:
		entry->portState[6] = (data2 >> 10) & 3;
		/* pass through */
	case 6:
		entry->portState[5] = (data2 >> 6) & 3;
		/* pass through */
	case 5:
		entry->portState[4] = (data2 >> 2) & 3;
		/* pass through */
	case 4:
		entry->portState[3] = (data1 >> 14) & 3;
		/* pass through */
	case 3:
		entry->portState[2] = (data1 >> 10) & 3;
		/* pass through */
	case 2:
		entry->portState[1] = (data1 >> 6) & 3;
		/* pass through */
	case 1:
		entry->portState[0] = (data1 >> 2) & 3;
		break;

	default:
		return MSD_FAIL;
	}
	return MSD_OK;
}

/*******************************************************************************
* Agate_stuOperationPerform
*
* DESCRIPTION:
*       This function is used by all STU control functions, and is responsible
*       to write the required operation into the STU registers.
*
* INPUTS:
*       stuOp       - The STU operation bits to be written into the STU
*                     operation register.
*       sid         - sid
*       valid       - valid bit
*       stuData     - STU Data with port state information
*
* OUTPUTS:
*       sid         - sid
*       valid       - valid bit
*       stuData     - STU Data with port state information
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MSD_STATUS Agate_stuOperationPerform
(
IN        MSD_QD_DEV           *dev,
IN      AGATE_MSD_STU_OPERATION    stuOp,
INOUT   MSD_U8               *valid,
INOUT    AGATE_MSD_STU_ENTRY        *entry
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* Data to be set into the      */
	/* register.                    */

	msdSemTake(dev->devNum, dev->vtuRegsSem, OS_WAIT_FOREVER);

	/* Wait until the VTU in ready. */
	data = 1;
	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_OPERATION, 15, 1, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}
	}
	/* Set the VTU data register if Load operation is required. */
	if (stuOp == AGATE_LOAD_PURGE_STU_ENTRY)
	{
		if (*valid == 1)
		{
			/* set the Port State for all the ports */
			retVal = Agate_stuSetSTUData(dev, entry);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}

			/* Set the valid bit (QD_REG_VTU_VID_REG) */
			data = *valid << 12;
			retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, (MSD_U8)(AGATE_QD_REG_VTU_VID_REG), data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}
		else
		{
			/* Clear the valid bit (QD_REG_VTU_VID_REG) */
			data = 0;
			retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, (MSD_U8)(AGATE_QD_REG_VTU_VID_REG), data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}
	}

	/* Set the SID register (QD_REG_STU_SID_REG) */
	data = (entry->sid) & 0x3F;
	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, (MSD_U8)(AGATE_QD_REG_STU_SID_REG), data);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->vtuRegsSem);
		return retVal;
	}

	data = (MSD_U16)((1 << 15) | (stuOp << 12));

	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_OPERATION, data);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->vtuRegsSem);
		return retVal;
	}

	/* If the operation is a get next operation wait for the response   */
	if (stuOp == AGATE_GET_NEXT_STU_ENTRY)
	{
		/* Wait until the STU in ready. */
		/* Wait until the VTU in ready. */
		data = 1;
		while (data == 1)
		{
			retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_OPERATION, 15, 1, &data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}

		/****************** get the valid bit *******************/
		retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_VTU_VID_REG, 12, 1, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		*valid = (MSD_U8)data;

		/****************** get the sid *******************/

		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL1_DEV_ADDR, AGATE_QD_REG_STU_SID_REG, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* the sid is bits 0-5 */
		entry->sid = data & 0x3F;

		if (*valid)
		{
			/* get the Port State for all the ports */
			retVal = Agate_stuGetSTUData(dev, entry);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}

		} /* entry is valid */

	} /* end of get next entry */

	msdSemGive(dev->devNum, dev->vtuRegsSem);
	return MSD_OK;
}

/*******************************************************************************
* Agate_gstuFindSidEntry
*
* DESCRIPTION:
*       Find STU entry for a specific SID, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       stuEntry - contains the SID to searche for
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       stuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no such entry.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Valid SID is 0 ~ 63.
*
*******************************************************************************/
MSD_STATUS Agate_gstuFindSidEntry
(
IN  MSD_QD_DEV       *dev,
INOUT AGATE_MSD_STU_ENTRY  *stuEntry,
OUT MSD_BOOL         *found
)
{
	MSD_U8               valid;
	MSD_STATUS           retVal;
	MSD_U8               port;
	MSD_LPORT            lport;
	AGATE_MSD_STU_ENTRY        entry;

	MSD_DBG_INFO(("Agate_gstuFindSidEntry Called.\n"));

	if (stuEntry->sid > 0x3F)
	{
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
		return MSD_BAD_PARAM;
	}

	if (NULL == found)
	{
		MSD_DBG_ERROR(("Input param MSD_BOOL found is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	*found = MSD_FALSE;

	/* Decrement 1 from sid */
	if (stuEntry->sid == 0)
	{
		entry.sid = 0x3F;
	}
	else
	{
		entry.sid = stuEntry->sid - 1;
	}

	valid = 0; /* valid is not used as input in this operation */

	retVal = Agate_stuOperationPerform(dev, AGATE_GET_NEXT_STU_ENTRY, &valid, &entry);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Agate_stuOperationPerform AGATE_GET_NEXT_STU_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* retrive the value from the operation */
	if ((entry.sid != stuEntry->sid) || (valid == 0))
	{
		MSD_DBG_INFO(("Agate_gstuFindSidEntry Exit, MSD_NO_SUCH\n"));
		return MSD_OK;
	}

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		port = MSD_LPORT_2_PORT(lport);
		if(port>=MSD_MAX_SWITCH_PORTS )
        {
            MSD_DBG_ERROR(("Port Error Number:%d.\n",port));
			return MSD_BAD_PARAM;
        }
		stuEntry->portState[lport] = entry.portState[port];
	}

	*found = MSD_TRUE;

	MSD_DBG_INFO(("Agate_gstuFindSidEntry Exit.\n"));
	return MSD_OK;
}
MSD_STATUS Agate_gstuDump
(
IN MSD_QD_DEV    *dev
)
{
	MSD_STATUS status;
	AGATE_MSD_STU_ENTRY entry;
	int temp;

	msdMemSet(&entry, 0, sizeof(AGATE_MSD_STU_ENTRY));
	entry.sid = 0x3f;

	/* Print out ATU entry field */
	MSG(("\n-----------------\n"));
	MSG(("SID  PORTSTATE   \n"));
	MSG(("-----------------\n"));

	temp = 1;
	while (1 == temp)
	{
		status = Agate_gstuGetEntryNext(dev, &entry);
		if (MSD_NO_SUCH == status)
			break;

		if (MSD_OK != status)
		{
			return status;
		}

		MSG(("%-5x%x %x %x %x %x %x %x   \n", entry.sid,
			entry.portState[0], entry.portState[1], entry.portState[2], entry.portState[3], entry.portState[4], entry.portState[5],
			entry.portState[6]));

		if (entry.sid == 0x3f)
			break;

	}

	MSG(("\n"));
	return MSD_OK;
}
