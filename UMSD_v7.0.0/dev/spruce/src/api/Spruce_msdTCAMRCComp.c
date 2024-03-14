/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Spruce_msdTCAMRCComp.c
*
* DESCRIPTION:
*       API definitions for TCAM range check comparator
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <spruce/include/api/Spruce_msdTCAMRCComp.h>
#include <spruce/include/driver/Spruce_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* Internal TCAM structure declaration.                                    */
/****************************************************************************/

/*
*  typedef: struct SPRUCE_MSD_TCAM_RC_COMP_DATA_HW
*
*  Description: data required by Range Check TCAM RC Page - TCAM entry page 2 Block 0x06
*
*  Fields:
*
*/
typedef struct
{
	MSD_U16  frame[26];  /* RC action part */
} SPRUCE_MSD_TCAM_RC_COMP_DATA_HW;

typedef struct
{
	MSD_U8    CompPort;
	SPRUCE_MSD_TCAM_RC_COMP_DATA_HW    CompDataP;
} SPRUCE_MSD_TCAM_RC_COMP_OP_DATA;

typedef enum
{
	Spruce_TCAMRCComp_FLUSH_ALL = 0x1,
	Spruce_TCAMRCComp_FLUSH_ENTRY = 0x2,
	Spruce_TCAMRCComp_LOAD_ENTRY = 0x3,
	Spruce_TCAMRCComp_PURGE_ENTRY = 0x3,
	Spruce_TCAMRCComp_GET_NEXT_ENTRY = 0x4,
	Spruce_TCAMRCComp_READ_ENTRY = 0x5
} SPRUCE_MSD_TCAM_RC_COMP_OPERATION;

static MSD_STATUS Spruce_setTcamRCCompHWData
(
IN	const SPRUCE_MSD_TCAM_RC_COMP_DATA	*iData,
OUT	SPRUCE_MSD_TCAM_RC_COMP_DATA_HW	*oData
);
static MSD_STATUS Spruce_getTcamRCCompHWData
(
IN	const SPRUCE_MSD_TCAM_RC_COMP_DATA_HW	*iData,
OUT	SPRUCE_MSD_TCAM_RC_COMP_DATA	*oData
);

static MSD_STATUS Spruce_tcamSetPage2RCCompData(const MSD_QD_DEV *dev, const SPRUCE_MSD_TCAM_RC_COMP_DATA_HW *tcamDataP);
static MSD_STATUS Spruce_tcamGetPage2RCCompData(const MSD_QD_DEV *dev, SPRUCE_MSD_TCAM_RC_COMP_DATA_HW *tcamDataP);
static MSD_STATUS Spruce_waitTcamReady(const MSD_QD_DEV *dev);
static MSD_STATUS Spruce_tcamRCCompOperationPerform
(
IN    MSD_QD_DEV           *dev,
IN    SPRUCE_MSD_TCAM_RC_COMP_OPERATION   compOp,
INOUT SPRUCE_MSD_TCAM_RC_COMP_OP_DATA     *opData
);
static MSD_STATUS Spruce_setTcamExtensionReg(const MSD_QD_DEV *dev, MSD_U8 Block, MSD_U8 Port);


/*******************************************************************************
* Spruce_gtcamRCCompFlushAll
*
* DESCRIPTION:
*       This routine is to flush all Port Range Comparators.Based on the function
*		of Entry Delete. Set TCAM Page2 Block6 Reg1 to 0x1F.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCCompFlushAll
(
IN  MSD_QD_DEV     *dev
)
{
	MSD_STATUS           retVal;
	SPRUCE_MSD_TCAM_RC_COMP_OPERATION    op;
	SPRUCE_MSD_TCAM_RC_COMP_OP_DATA    CompOpData;

	MSD_DBG_INFO(("Spruce_gtcamRCCompFlushAll Called.\n"));

	/* Program Tuning register */
	op = Spruce_TCAMRCComp_FLUSH_ALL;
	retVal = Spruce_tcamRCCompOperationPerform(dev, op, &CompOpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Spruce_tcamRCCompOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
	}

	MSD_DBG_INFO(("Spruce_gtcamRCCompFlushAll Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gtcamRCCompFlushPort
*
* DESCRIPTION:
*       This routine is to flush a single Range Checker entry.
*
* INPUTS:
*       portNum			- Logical port number to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCCompFlushPort
(
IN  MSD_QD_DEV     *dev,
IN  MSD_LPORT      portNum
)
{
	MSD_STATUS           retVal;
	MSD_U8				 phyPort;        /* Physical port. */
	SPRUCE_MSD_TCAM_RC_COMP_OPERATION    op;
	SPRUCE_MSD_TCAM_RC_COMP_OP_DATA    CompOpData;

	MSD_DBG_INFO(("Spruce_gtcamRCCompFlushPort Called.\n"));

	phyPort = MSD_LPORT_2_PORT(portNum);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Spruce_TCAMRCComp_FLUSH_ENTRY;
		CompOpData.CompPort = phyPort;
		retVal = Spruce_tcamRCCompOperationPerform(dev, op, &CompOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_tcamRCCompOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Spruce_gtcamRCCompFlushPort Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gtcamRCCompLoad
*
* DESCRIPTION:
*       This routine configures a Range Check Compatator for the specified port.
*
* INPUTS:
*       portNum			- Logical port number to set
*       tcamRCCompData	- Range Check Comparator Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCCompLoad
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT portNum,
IN  SPRUCE_MSD_TCAM_RC_COMP_DATA *tcamRCCompData
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           phyPort;        /* Physical port.               */
	SPRUCE_MSD_TCAM_RC_COMP_OPERATION    op;
	SPRUCE_MSD_TCAM_RC_COMP_OP_DATA     CompOpData;

	MSD_DBG_INFO(("Spruce_gtcamRCCompLoad Called.\n"));
	phyPort = MSD_LPORT_2_PORT(portNum);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Spruce_TCAMRCComp_LOAD_ENTRY;
		CompOpData.CompPort = phyPort;
		retVal = Spruce_setTcamRCCompHWData(tcamRCCompData, &CompOpData.CompDataP);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_setTcamRCCompHWData returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Spruce_tcamRCCompOperationPerform(dev, op, &CompOpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_tcamRCCompOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Spruce_gtcamRCCompLoad Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gtcamRCCompRead
*
* DESCRIPTION:
*       This routine reads a Range Check Compatator for the specified port.
*
* INPUTS:
*       portNum			- Logical port number to set
*
* OUTPUTS:
*       tcamRCCompData	- Range Check Comparator Data
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCCompRead
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT portNum,
OUT SPRUCE_MSD_TCAM_RC_COMP_DATA *tcamRCCompData
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           phyPort;        /* Physical port.               */
	SPRUCE_MSD_TCAM_RC_COMP_OPERATION    op;
	SPRUCE_MSD_TCAM_RC_COMP_OP_DATA     CompOpData;

	MSD_DBG_INFO(("Spruce_gtcamRCCompRead Called.\n"));
	phyPort = MSD_LPORT_2_PORT(portNum);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Spruce_TCAMRCComp_READ_ENTRY;
		CompOpData.CompPort = phyPort;
		retVal = Spruce_tcamRCCompOperationPerform(dev, op, &CompOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_tcamRCCompOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Spruce_getTcamRCCompHWData(&CompOpData.CompDataP, tcamRCCompData);
		}
	}

	MSD_DBG_INFO(("Spruce_gtcamRCCompRead Exit.\n"));
	return retVal;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/

static MSD_STATUS Spruce_setTcamRCCompHWData
(
IN	const SPRUCE_MSD_TCAM_RC_COMP_DATA	*iData,
OUT	SPRUCE_MSD_TCAM_RC_COMP_DATA_HW	*oData
)
{
	msdMemSet(oData, 0, sizeof(SPRUCE_MSD_TCAM_RC_COMP_DATA_HW));

	/*Action*/
	MSD_BF_SET(oData->frame[0], iData->Comp0Sel, 12, 3);
	MSD_BF_SET(oData->frame[0], (MSD_U16)(iData->Comp0Op), 11, 1);
	MSD_BF_SET(oData->frame[1], iData->Comp0LoLimit , 0, 16);
	MSD_BF_SET(oData->frame[2], iData->Comp0HiLimit , 0, 16);

	MSD_BF_SET(oData->frame[3], iData->Comp1Sel, 12, 3);
	MSD_BF_SET(oData->frame[3], (MSD_U16)(iData->Comp1Op), 11, 1);
	MSD_BF_SET(oData->frame[4], iData->Comp1LoLimit, 0, 16);
	MSD_BF_SET(oData->frame[5], iData->Comp1HiLimit, 0, 16);

	MSD_BF_SET(oData->frame[6], iData->Comp2Sel, 12, 3);
	MSD_BF_SET(oData->frame[6], (MSD_U16)(iData->Comp2Op), 11, 1);
	MSD_BF_SET(oData->frame[7], iData->Comp2LoLimit, 0, 16);
	MSD_BF_SET(oData->frame[8], iData->Comp2HiLimit, 0, 16);

	MSD_BF_SET(oData->frame[9], iData->Comp3Sel, 12, 3);
	MSD_BF_SET(oData->frame[9], (MSD_U16)(iData->Comp3Op), 11, 1);
	MSD_BF_SET(oData->frame[10], iData->Comp3LoLimit, 0, 16);
	MSD_BF_SET(oData->frame[11], iData->Comp3HiLimit, 0, 16);

	MSD_BF_SET(oData->frame[12], iData->Comp4Sel, 12, 3);
	MSD_BF_SET(oData->frame[12], (MSD_U16)(iData->Comp4Op), 11, 1);
	MSD_BF_SET(oData->frame[13], iData->Comp4LoLimit, 0, 16);
	MSD_BF_SET(oData->frame[14], iData->Comp4HiLimit, 0, 16);

	MSD_BF_SET(oData->frame[15], iData->Comp5Sel, 12, 3);
	MSD_BF_SET(oData->frame[15], (MSD_U16)(iData->Comp5Op), 11, 1);
	MSD_BF_SET(oData->frame[16], iData->Comp5LoLimit, 0, 16);
	MSD_BF_SET(oData->frame[17], iData->Comp5HiLimit, 0, 16);

	MSD_BF_SET(oData->frame[18], iData->Comp6Sel, 12, 3);
	MSD_BF_SET(oData->frame[18], (MSD_U16)(iData->Comp6Op), 11, 1);
	MSD_BF_SET(oData->frame[19], iData->Comp6LoLimit, 0, 16);
	MSD_BF_SET(oData->frame[20], iData->Comp6HiLimit, 0, 16);

	MSD_BF_SET(oData->frame[21], iData->Comp7Sel, 12, 3);
	MSD_BF_SET(oData->frame[21], (MSD_U16)(iData->Comp7Op), 11, 1);
	MSD_BF_SET(oData->frame[22], iData->Comp7LoLimit, 0, 16);
	MSD_BF_SET(oData->frame[23], iData->Comp7HiLimit, 0, 16);

	return MSD_OK;
}

static MSD_STATUS Spruce_getTcamRCCompHWData
(
IN	const SPRUCE_MSD_TCAM_RC_COMP_DATA_HW	*iData,
OUT	SPRUCE_MSD_TCAM_RC_COMP_DATA	*oData
)
{
	
	oData->Comp0Sel = (MSD_U8)MSD_BF_GET(iData->frame[0], 12, 3);
	oData->Comp0Op = MSD_BF_GET(iData->frame[0], 11, 1);
	oData->Comp0LoLimit = (MSD_U16)MSD_BF_GET(iData->frame[1], 0, 16);
	oData->Comp0HiLimit = (MSD_U16)MSD_BF_GET(iData->frame[2], 0, 16);

	oData->Comp1Sel = (MSD_U8)MSD_BF_GET(iData->frame[3], 12, 3);
	oData->Comp1Op = MSD_BF_GET(iData->frame[3], 11, 1);
	oData->Comp1LoLimit = (MSD_U16)MSD_BF_GET(iData->frame[4], 0, 16);
	oData->Comp1HiLimit = (MSD_U16)MSD_BF_GET(iData->frame[5], 0, 16);

	oData->Comp2Sel = (MSD_U8)MSD_BF_GET(iData->frame[6], 12, 3);
	oData->Comp2Op = MSD_BF_GET(iData->frame[6], 11, 1);
	oData->Comp2LoLimit = (MSD_U16)MSD_BF_GET(iData->frame[7], 0, 16);
	oData->Comp2HiLimit = (MSD_U16)MSD_BF_GET(iData->frame[8], 0, 16);

	oData->Comp3Sel = (MSD_U8)MSD_BF_GET(iData->frame[9], 12, 3);
	oData->Comp3Op = MSD_BF_GET(iData->frame[9], 11, 1);
	oData->Comp3LoLimit = (MSD_U16)MSD_BF_GET(iData->frame[10], 0, 16);
	oData->Comp3HiLimit = (MSD_U16)MSD_BF_GET(iData->frame[11], 0, 16);

	oData->Comp4Sel = (MSD_U8)MSD_BF_GET(iData->frame[12], 12, 3);
	oData->Comp4Op = MSD_BF_GET(iData->frame[12], 11, 1);
	oData->Comp4LoLimit = (MSD_U16)MSD_BF_GET(iData->frame[13], 0, 16);
	oData->Comp4HiLimit = (MSD_U16)MSD_BF_GET(iData->frame[14], 0, 16);

	oData->Comp5Sel = (MSD_U8)MSD_BF_GET(iData->frame[15], 12, 3);
	oData->Comp5Op = MSD_BF_GET(iData->frame[15], 11, 1);
	oData->Comp5LoLimit = (MSD_U16)MSD_BF_GET(iData->frame[16], 0, 16);
	oData->Comp5HiLimit = (MSD_U16)MSD_BF_GET(iData->frame[17], 0, 16);

	oData->Comp6Sel = (MSD_U8)MSD_BF_GET(iData->frame[18], 12, 3);
	oData->Comp6Op = MSD_BF_GET(iData->frame[18], 11, 1);
	oData->Comp6LoLimit = (MSD_U16)MSD_BF_GET(iData->frame[19], 0, 16);
	oData->Comp6HiLimit = (MSD_U16)MSD_BF_GET(iData->frame[20], 0, 16);

	oData->Comp7Sel = (MSD_U8)MSD_BF_GET(iData->frame[21], 12, 3);
	oData->Comp7Op = MSD_BF_GET(iData->frame[21], 11, 1);
	oData->Comp7LoLimit = (MSD_U16)MSD_BF_GET(iData->frame[22], 0, 16);
	oData->Comp7HiLimit = (MSD_U16)MSD_BF_GET(iData->frame[23], 0, 16);

	return MSD_OK;
}

/*******************************************************************************
* Spruce_tcamRCCompOperationPerform
*
* DESCRIPTION:
*       This function accesses TCAM Table
*
* INPUTS:
*       compOp   - The port range comparators operation
*       opData   - address and data to be written into comparator
*
* OUTPUTS:
*       opData	 - data read from port range comparators pointed by address
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MSD_STATUS Spruce_tcamRCCompOperationPerform
(
IN    MSD_QD_DEV           *dev,
IN    SPRUCE_MSD_TCAM_RC_COMP_OPERATION   compOp,
INOUT SPRUCE_MSD_TCAM_RC_COMP_OP_DATA     *opData
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          data;     /* temporary Data storage */

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	/* Wait until the tcam in ready. */
	retVal = Spruce_waitTcamReady(dev);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/*Change Block to 6, indicate Port Range Comparators Page */
	retVal = Spruce_setTcamExtensionReg(dev, (MSD_U8)6, (MSD_U8)0);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set the TCAM Operation register */
	switch (compOp)
	{
	case Spruce_TCAMRCComp_FLUSH_ALL:
	{
		/* Wait until the tcam in ready. */
		retVal = Spruce_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/*Set Port to 0x1F, Flush all Range Check entries */
		retVal = Spruce_setTcamExtensionReg(dev, (MSD_U8)6, (MSD_U8)0x1f);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		compOp = Spruce_TCAMRCComp_FLUSH_ENTRY;
		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)compOp << 12) | (MSD_U16)((MSD_U16)2 << 10));
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}
		break;

	case Spruce_TCAMRCComp_FLUSH_ENTRY:
	{
		/* Wait until the tcam in ready. */
		retVal = Spruce_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/*Set Port to 0x1F, Flush all Range Check entries */
		retVal = Spruce_setTcamExtensionReg(dev, (MSD_U8)6, opData->CompPort);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)compOp << 12) | (MSD_U16)((MSD_U16)2 << 10));
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}
		break;

	case Spruce_TCAMRCComp_LOAD_ENTRY:
		/*    case Spruce_TCAM_PURGE_ENTRY: */
	{
		/* load Page 2 Block 6 */
		/* Wait until the tcam in ready. */
		retVal = Spruce_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/*Access Range Check from TCAM Range Frame */
		retVal = Spruce_setTcamExtensionReg(dev, (MSD_U8)6, opData->CompPort);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Spruce_tcamSetPage2RCCompData(dev, &opData->CompDataP);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Read page 2 */
		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)3 << 12) | (MSD_U16)((MSD_U16)2 << 10));
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Spruce_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

	}
		break;

	case Spruce_TCAMRCComp_GET_NEXT_ENTRY:
	case Spruce_TCAMRCComp_READ_ENTRY:
	{
		/* Wait until the tcam in ready. */
		retVal = Spruce_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/*Change Block to 6, indicate Range Check Page */
		retVal = Spruce_setTcamExtensionReg(dev, (MSD_U8)6, opData->CompPort);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Read page 2 */
		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)5 << 12) | (MSD_U16)((MSD_U16)2 << 10));
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Spruce_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Spruce_tcamGetPage2RCCompData(dev, &opData->CompDataP);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		retVal = Spruce_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

	}
		break;

	default:
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
		break;
	}
	/*Change Block to 0, in case other misoperation*/
	retVal = Spruce_setTcamExtensionReg(dev, 0, 0);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Wait until the tcam in ready. */
	retVal = Spruce_waitTcamReady(dev);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;
}


static MSD_STATUS Spruce_tcamSetPage2RCCompData(const MSD_QD_DEV *dev, const SPRUCE_MSD_TCAM_RC_COMP_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal = MSD_OK;    /* Functions return value */
	MSD_16 i;

	for (i = 2; i < 26; i++)
	{
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataP->frame[i - 2]);
		if (retVal != MSD_OK)
		{
			break;
		}
	}

	return retVal;
}

static MSD_STATUS Spruce_tcamGetPage2RCCompData(const MSD_QD_DEV *dev, SPRUCE_MSD_TCAM_RC_COMP_DATA_HW *tcamDataP)
{
	MSD_STATUS       retVal = MSD_OK;    /* Functions return value */
	MSD_16 i;

	for (i = 2; i < 26; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataP->frame[i - 2]);
		if (retVal != MSD_OK)
		{
			break;
		}
	}

	return retVal;
}

static MSD_STATUS Spruce_waitTcamReady(const MSD_QD_DEV *dev)
{
	MSD_STATUS       retVal = MSD_OK;    /* Functions return value */

	MSD_U16          tmpdata;     /* temporary Data storage */

	tmpdata = 1U;
	while (tmpdata == 1U)
	{
		retVal = msdGetAnyRegField(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, (MSD_U8)15, (MSD_U8)1, &tmpdata);
		if (retVal != MSD_OK)
		{
			break;
		}
	}

	return retVal;
}

static MSD_STATUS Spruce_setTcamExtensionReg(const MSD_QD_DEV *dev, MSD_U8 Block, MSD_U8 Port)
{
	MSD_STATUS    retVal;    /* Functions return value */
	MSD_U16       tmpdata = 0;

	tmpdata = (MSD_U16)(((MSD_U16)Block & (MSD_U16)0xF) << 12) | (MSD_U16)((MSD_U16)Port & (MSD_U16)0x1F);

	retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, (MSD_U8)1, tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Spruce_setTcamExtensionReg returned: %s.\n", msdDisplayStatus(retVal)));
	}

	return retVal;
}