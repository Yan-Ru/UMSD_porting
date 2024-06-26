/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Bonsai_msdTCAM.c
*
* DESCRIPTION:
*       API definitions for control of Ternary Content Addressable Memory
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsai/include/api/Bonsai_msdTCAM.h>
#include <bonsai/include/api/Bonsai_msdApiInternal.h>
#include <bonsai/include/driver/Bonsai_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
/****************************************************************************/
/* Internal TCAM structure declaration.                                    */
/****************************************************************************/

/*
 *  typedef: struct BONSAI_MSD_TCAM_PG_DATA
 *
 *  Description: data required by Ingress TCAM entry page 
 *
 *  Fields:
 *      
 */
typedef struct
{
	MSD_U16  frame[26];  /* key&mask, 0-21 bytes of frame */
					  
} BONSAI_MSD_TCAM_PG_DATA;

/*
 *  typedef: struct BONSAI_MSD_TCAM_DATA_HW
 *
 *  Description: data required by Ingress TCAM (Ternary Content Addressable Memory) Operation
 *
 *  Fields:
 *      pg0 - page 0 registers  of TCAM
 *      pg1 - page 1 registers  of TCAM
 *      pg2 - page 2 registers  of TCAM
 */
typedef struct 
{
	/* Pg0 registers */
	BONSAI_MSD_TCAM_PG_DATA pg0;
	/* Pg1 registers */
	BONSAI_MSD_TCAM_PG_DATA pg1;
	/* Pg2 registers */
	BONSAI_MSD_TCAM_PG_DATA pg2;
} BONSAI_MSD_TCAM_DATA_HW;

/*
 *  typedef: struct BONSAI_MSD_TCAM_EGR_DATA_HW
 *
 *  Description: data required by Egress TCAM Action Page - TCAM entry page 3
 *
 *  Fields:
 *      
 */
typedef struct
{
	MSD_U16  frame[4];  /* action part */

} BONSAI_MSD_TCAM_EGR_DATA_HW;

typedef struct
{
    MSD_U32    tcamEntry;
    BONSAI_MSD_TCAM_DATA_HW    tcamDataP; 
} BONSAI_MSD_TCAM_OP_DATA;

typedef struct 
{
	MSD_U8  port;
	MSD_U32	  tcamEntry;
	BONSAI_MSD_TCAM_EGR_DATA_HW	tcamDataP;
} BONSAI_MSD_TCAM_EGR_OP_DATA;

typedef enum
{
	Bonsai_TCAM_FLUSH_ALL = 0x1,
	Bonsai_TCAM_FLUSH_ENTRY = 0x2,
	Bonsai_TCAM_LOAD_ENTRY = 0x3,
	Bonsai_TCAM_PURGE_ENTRY = 0x3,
	Bonsai_TCAM_GET_NEXT_ENTRY = 0x4,
	Bonsai_TCAM_READ_ENTRY = 0x5
} BONSAI_MSD_TCAM_OPERATION;

/****************************************************************************/
/* TCAM operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Bonsai_tcamOperationPerform
(
    IN   MSD_QD_DEV             *dev,
    IN   BONSAI_MSD_TCAM_OPERATION    tcamOp,
    INOUT BONSAI_MSD_TCAM_OP_DATA    *opData
);

static MSD_STATUS Bonsai_tcamEgrOperationPerform
(
    IN   MSD_QD_DEV             *dev,
    IN   BONSAI_MSD_TCAM_OPERATION    tcamOp,
    INOUT BONSAI_MSD_TCAM_EGR_OP_DATA    *opData
);
static MSD_STATUS Bonsai_setTcamHWData
(
    IN	const BONSAI_MSD_TCAM_DATA	*iData,
	OUT	BONSAI_MSD_TCAM_DATA_HW	*oData
);
static MSD_STATUS Bonsai_getTcamHWData
(
	IN	const BONSAI_MSD_TCAM_DATA_HW	*iData,
	OUT	BONSAI_MSD_TCAM_DATA	*oData
);
static MSD_STATUS Bonsai_setTcamEgrHWData
(
	IN	const BONSAI_MSD_TCAM_EGR_DATA	*iData,
	OUT	BONSAI_MSD_TCAM_EGR_DATA_HW	*oData
);
static MSD_STATUS Bonsai_getTcamEgrHWData
(
	IN	const BONSAI_MSD_TCAM_EGR_DATA_HW	*iData,
	OUT	BONSAI_MSD_TCAM_EGR_DATA	*oData
);
static void displayTcamData(BONSAI_MSD_TCAM_DATA *tcamData);
static void mappingIpv4ToTcam(BONSAI_MSD_TCAM_DATA *tcamData, const MSD_TCAM_ENT_IPV4_UNT *pIpv4Data, const MSD_TCAM_ENT_IPV4_UNT *pIpv4Mask);
static void mappingUdpToTcam(MSD_16 startIndex, BONSAI_MSD_TCAM_DATA *tcamData, const MSD_TCAM_ENT_UDP_UNT *pUdpData, const MSD_TCAM_ENT_UDP_UNT *pUdpMask);
static void mappingActionToTcam(BONSAI_MSD_TCAM_DATA *tcamData, const BONSAI_MSD_TCAM_ENT_ACT *actionPtr);
static void mappingIpv6ToTcam(BONSAI_MSD_TCAM_DATA *tcamData, BONSAI_MSD_TCAM_DATA *tcamData2, const MSD_TCAM_ENT_IPV6_UNT *pIpv6, const MSD_TCAM_ENT_IPV6_UNT *pIpv6Mask);
static void mappingTcpOverIpv4ToTcam(BONSAI_MSD_TCAM_DATA *tcamData, BONSAI_MSD_TCAM_DATA *tcamData2, const MSD_TCAM_ENT_TCP_UNT *pTcpData, const MSD_TCAM_ENT_TCP_UNT *pTcpMask);
static void mappingTcpOverIpv6ToTcam(BONSAI_MSD_TCAM_DATA *tcamData2, const MSD_TCAM_ENT_TCP_UNT *pTcpData, const MSD_TCAM_ENT_TCP_UNT *pTcpMask);
static MSD_BOOL checkEntry2Used(const BONSAI_MSD_TCAM_DATA *tcamData2);
static MSD_STATUS Bonsai_setTcamExtensionReg(const MSD_QD_DEV *dev,MSD_U8 block, MSD_U8 port);

static MSD_STATUS Bonsai_tcamSetPage0Data(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamSetPage1Data(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamSetPage2Data(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamSetPage3Data(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_EGR_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamGetPage0Data(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamGetPage1Data(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamGetPage2Data(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamGetPage3Data(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_EGR_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_waitTcamReady(const MSD_QD_DEV *dev);


/*******************************************************************************
* Bonsai_gtcamFlushAll
*
* DESCRIPTION:
*       This routine is to flush all entries. A Flush All command will initialize 
*       TCAM Pages 0 and 1, offsets 0x02 to 0x1B to 0x0000, and TCAM Page 2 offset
*       0x02 to 0x1B to 0x0000 for all TCAM entries with the exception that TCAM 
*       Page 0 offset 0x02 will be initialized to 0x00FF.
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Bonsai_gtcamFlushAll
(
    IN  MSD_QD_DEV     *dev
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Bonsai_gtcamFlushAll Called.\n"));

    /* Program Tuning register */
    op = Bonsai_TCAM_FLUSH_ALL;
    retVal = Bonsai_tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("tcamOperationPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
    }

    MSD_DBG_INFO(("Bonsai_gtcamFlushAll Exit.\n"));
	return retVal;

}

/*******************************************************************************
* Bonsai_gtcamFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single entry. A Flush a single TCAM entry command 
*       will write the same values to a TCAM entry as a Flush All command, but it is
*       done to the selected single TCAM entry only.
*
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
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
MSD_STATUS Bonsai_gtcamFlushEntry
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Bonsai_gtcamFlushEntry Called.\n"));

    /* check if the given pointer is valid */
	if (tcamPointer >= BONSAI_MAX_IGR_TCAM_ENTRY)
	{
		MSD_DBG_ERROR(("Bad tcamPointer %u. It should be within [0, %d].\n", tcamPointer, BONSAI_MAX_IGR_TCAM_ENTRY - 1));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAM_FLUSH_ENTRY;
		tcamOpData.tcamEntry = tcamPointer;
		retVal = Bonsai_tcamOperationPerform(dev, op, &tcamOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("tcamOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamFlushEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamLoadEntry
*
* DESCRIPTION:
*       This routine loads a TCAM entry.
*       The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       3 pages of data. All 3 pages need to loaded in a particular order for the TCAM 
*       to operate correctly while frames are flowing through the switch. 
*       If the entry is currently valid, it must first be flushed. Then page 2 needs 
*       to be loaded first, followed by page 1 and then finally page 0. 
*       Each page load requires its own write TCAMOp with these TCAM page bits set 
*       accordingly. 
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*        tcamData    - Tcam entry Data
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
MSD_STATUS Bonsai_gtcamLoadEntry
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer,
    IN  BONSAI_MSD_TCAM_DATA        *tcamData
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Bonsai_gtcamLoadEntry Called.\n"));

    /* check if the given pointer is valid */
	if ((tcamPointer >= BONSAI_MAX_IGR_TCAM_ENTRY) || (tcamData == NULL))
	{
		MSD_DBG_ERROR(("Bad tcamPointer %u out of range OR tcamData is NULL.\n", tcamPointer));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAM_LOAD_ENTRY;
		tcamOpData.tcamEntry = tcamPointer;
		/*tcamOpData.tcamDataP = tcamData;*/
		retVal = Bonsai_setTcamHWData(tcamData, &tcamOpData.tcamDataP);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_setTcamHWData returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Bonsai_tcamOperationPerform(dev, op, &tcamOpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("tcamOperationPerform LOAD_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamLoadEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamReadTCAMData
*
* DESCRIPTION:
*       This routine reads the global 3 offsets 0x02 to 0x1B registers with 
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 6:0 and 11:10 respectively.
*
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
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
MSD_STATUS Bonsai_gtcamReadTCAMData
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer,
    OUT BONSAI_MSD_TCAM_DATA        *tcamData
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Bonsai_gtcamReadTCAMData Called.\n"));


    /* check if the given pointer is valid */
	if ((tcamPointer >= BONSAI_MAX_IGR_TCAM_ENTRY) || (tcamData == NULL))
	{
		MSD_DBG_ERROR(("Bad tcamPointer %u out of range OR tcamData is NULL.\n", tcamPointer));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAM_READ_ENTRY;
		tcamOpData.tcamEntry = tcamPointer;
		/*tcamOpData.tcamDataP = tcamData;*/
		retVal = Bonsai_tcamOperationPerform(dev, op, &tcamOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("tcamOperationPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Bonsai_getTcamHWData(&tcamOpData.tcamDataP, tcamData);
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamReadTCAMData Exit.\n"));
	return retVal;

}

/*******************************************************************************
* Bonsai_gtcamGetNextTCAMData
*
* DESCRIPTION:
*       This routine  finds the next higher TCAM Entry number that is valid (i.e.,
*       any entry whose Page 0 offset 0x02 is not equal to 0x00FF). The TCAM Entry 
*       register (bits 6:0) is used as the TCAM entry to start from. To find 
*       the lowest number TCAM Entry that is valid, start the Get Next operation 
*       with TCAM Entry set to 0xFF.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM (0 ~ 255)
*        tcamData    - Tcam entry Data
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
MSD_STATUS Bonsai_gtcamGetNextTCAMData
(
    IN     MSD_QD_DEV     *dev,
    INOUT  MSD_U32        *tcamPointer,
    OUT    BONSAI_MSD_TCAM_DATA  *tcamData
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Bonsai_gtcamGetNextTCAMData Called.\n"));

    /* check if the given pointer is valid */
	if (tcamPointer == NULL)
	{
		MSD_DBG_ERROR(("tcamPointer is NULL"));
		retVal = MSD_BAD_PARAM;
	}
	else if ((*tcamPointer >= BONSAI_MAX_IGR_TCAM_ENTRY) || (tcamData == NULL))
	{
		MSD_DBG_ERROR(("tcamPointer is NULL or Bad tcamPointer value %u out of range OR tcamData is NULL.\n", *tcamPointer));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAM_GET_NEXT_ENTRY;
		tcamOpData.tcamEntry = *tcamPointer;
		/*tcamOpData.tcamDataP = tcamData;*/
		retVal = Bonsai_tcamOperationPerform(dev, op, &tcamOpData);
		if (retVal == MSD_NO_SUCH)
		{
			MSD_DBG_INFO(("No higher valid entry founded return MSD_NO_SUCH"));
		}
		else if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("tcamOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			*tcamPointer = tcamOpData.tcamEntry;
			retVal = Bonsai_getTcamHWData(&tcamOpData.tcamDataP, tcamData);
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamGetNextTCAMData Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamFindEntry
*
* DESCRIPTION:
*       Find the specified valid tcam entry in ingress TCAM Table.
*
* INPUTS:
*       tcamPointer - the tcam entry index to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       tcamData - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM    - on bad parameter
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Bonsai_gtcamFindEntry
(
    IN	MSD_QD_DEV     *dev,
    IN	MSD_U32        tcamPointer,
    OUT	BONSAI_MSD_TCAM_DATA  *tcamData,
	OUT	MSD_BOOL		  *found
)
{
    MSD_STATUS       retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Bonsai_gtcamFindEntry Called.\n"));

    /* check if the given pointer is valid */
	if ((tcamPointer >= BONSAI_MAX_IGR_TCAM_ENTRY) || (tcamData == NULL) || (found == NULL))
	{
		MSD_DBG_ERROR(("Bad tcamPointer %u out of range OR tcamData is NULL or found is NULL.\n", tcamPointer));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		*found = MSD_FALSE;

		/* Program Tuning register */
		op = Bonsai_TCAM_GET_NEXT_ENTRY;
		tcamOpData.tcamEntry = tcamPointer == 0U ? (MSD_U32)0xff : (tcamPointer - 1U);
		retVal = Bonsai_tcamOperationPerform(dev, op, &tcamOpData);
		if (retVal == MSD_NO_SUCH)
		{
			MSD_DBG_INFO(("Not found this entry, Bonsai_gtcamFindEntry Exit.\n"));
			retVal = MSD_OK;
		}
		else
		{
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("tcamOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				if (tcamOpData.tcamEntry != tcamPointer)
				{
					MSD_DBG_INFO(("Not found this entry, Bonsai_gtcamFindEntry Exit.\n"));
					retVal = MSD_OK;
				}
				else
				{
					*found = MSD_TRUE;
					retVal = Bonsai_getTcamHWData(&tcamOpData.tcamDataP, tcamData);
				}
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamFindEntry Exit.\n"));
	return retVal;
}
/*******************************************************************************
* Bonsai_gtcamSetMode
*
* DESCRIPTION:
*       Set the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*       Note: Do not chage the value while frames are flowing into this port,
*             You must first put the port in the Disable Port State, then change
*             TCAM mode, and then re-enable the port's Port State again.
*
* INPUTS:
*       portNum  - the logical port number
*       mode     - tcam mode to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Bonsai_gtcamSetMode
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT portNum,
    IN BONSAI_MSD_TCAM_MODE mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Bonsai_gtcamSetMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = BONSAI_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if ((mode < BONSAI_MSD_TCAM_MODE_DISABLE) || (mode > BONSAI_MSD_TCAM_MODE_ENABLE_96))
		{
			MSD_DBG_ERROR(("Failed (Bad Tcam Mode).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, (MSD_U8)0x0d, (MSD_U8)0, (MSD_U8)2, (MSD_U16)mode);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write Register, phyAddr: %x, regAddr:%x.\n", phyAddr, 0x19));
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamSetMode Exit.\n"));
	return retVal;
}
/*******************************************************************************
* Bonsai_gtcamGetMode
*
* DESCRIPTION:
*       Set the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*
* INPUTS:
*       portNum  - the logical port number
*
* OUTPUTS:
*       mode     - get back tcam mode
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Bonsai_gtcamGetMode
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    OUT BONSAI_MSD_TCAM_MODE *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Bonsai_gtcamGetMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = BONSAI_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* Get the DefaultForward. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, (MSD_U8)0x0d, (MSD_U8)0, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read BONSAI_QD_REG_PORT_CONTROL Register.\n"));
		}
		else
		{
			*mode = (BONSAI_MSD_TCAM_MODE)data;
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamGetMode Exit.\n"));
	return retVal;
}
/*******************************************************************************
* Bonsai_gtcamAddEntryPort
*
* DESCRIPTION:
*       Add a port to an existing tcam entry. If the tcam entry does not exist
*       the API should return MSD_NO_SUCH
*
* INPUTS:
*       devNum   - physical device number
*       tcamPointer - pointer to the desired entry of TCAM (Bonsai:0 ~ 255, Topaz:0~127)
*       portNum  - the logical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - the entry is not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Bonsai_gtcamAddEntryPort
(
    IN MSD_QD_DEV *dev,
    IN MSD_U32   tcamPointer,
    IN MSD_LPORT portNum
)
{
    MSD_STATUS retVal;
    MSD_BOOL found;
    BONSAI_MSD_TCAM_DATA  tcamData;
    MSD_U8 hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Bonsai_gtcamAddEntryPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Bonsai_gtcamFindEntry(dev, tcamPointer, &tcamData, &found);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_gtcamFindEntry in Bonsai_gtcamAddEntryPort returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			if (found == MSD_FALSE)
			{
				MSD_DBG_ERROR(("Do not find the tcam entry with tcamPointer:%u\n", tcamPointer));
				retVal = MSD_NO_SUCH;
			}
			else
			{
				tcamData.spvMask = tcamData.spvMask & (MSD_U16)~(MSD_U16)((MSD_U16)1 << hwPort);
				tcamData.spv = tcamData.spv & (MSD_U16)~(MSD_U16)((MSD_U16)1 << hwPort);

				retVal = Bonsai_gtcamLoadEntry(dev, tcamPointer, &tcamData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Bonsai_gtcamLoadEntry in Bonsai_gtcamAddEntryPort returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamAddEntryPort Exit.\n"));
    return retVal;
}
/*******************************************************************************
* Bonsai_gtcamRemoveEntryPort
*
* DESCRIPTION:
*       Remove a port to an existing tcam entry. If the tcam entry does not exist
*       the API should return MSD_NO_SUCH
*
* INPUTS:
*       devNum   - physical device number
*       tcamPointer - pointer to the desired entry of TCAM (Bonsai:0 ~ 255, Topaz:0~127)
*       portNum  - the logical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - the entry is not exist
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Bonsai_gtcamRemoveEntryPort
(
    IN MSD_QD_DEV *dev,
    IN MSD_U32   tcamPointer,
    IN MSD_LPORT portNum
)
{
    MSD_STATUS retVal;
    MSD_BOOL found;
    BONSAI_MSD_TCAM_DATA  tcamData;
    MSD_U8 hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Bonsai_gtcamRemoveEntryPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Bonsai_gtcamFindEntry(dev, tcamPointer, &tcamData, &found);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_gtcamFindEntry in Bonsai_gtcamRemoveEntryPort returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			if (found == MSD_FALSE)
			{
				MSD_DBG_ERROR(("Do not find the tcam entry with tcamPointer:%u\n", tcamPointer));
				retVal = MSD_NO_SUCH;
			}
			else
			{
				tcamData.spvMask = tcamData.spvMask | (MSD_U16)((MSD_U16)1 << hwPort);
				tcamData.spv = tcamData.spv & (MSD_U16)~(MSD_U16)((MSD_U16)1 << hwPort);

				retVal = Bonsai_gtcamLoadEntry(dev, tcamPointer, &tcamData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Bonsai_gtcamLoadEntry in Bonsai_gtcamRemoveEntryPort returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamRemoveEntryPort Exit.\n"));
    return retVal;
}


/*******************************************************************************
* Bonsai_gtcamEgrFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single egress entry for a particular port. A
*       Flush a single Egress TCAM entry command will write the same values to a 
*       Egress TCAM entry as a Flush All command, but it is done to the selected 
*		single egress TCAM entry of the selected single port only.
*
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
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
MSD_STATUS Bonsai_gtcamEgrFlushEntry
(
    IN  MSD_QD_DEV     *dev,
	IN  MSD_LPORT      port,
    IN  MSD_U32        tcamPointer
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_EGR_OP_DATA     tcamOpData;
	MSD_U8			hwPort;

    MSD_DBG_INFO(("Bonsai_gtcamEgrFlushEntry Called.\n"));

    /* check if the given port is valid */
    hwPort = MSD_LPORT_2_PORT(port);
    if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* check if the given pointer is valid */
		if ((tcamPointer > (MSD_U32)0x3F) || (tcamPointer == (MSD_U32)0))
		{
			MSD_DBG_ERROR(("Bad tcamPointer: %u. It should be within (0,63].\n", tcamPointer));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			/* Program Tuning register */
			op = Bonsai_TCAM_FLUSH_ENTRY;
			tcamOpData.port = hwPort;
			tcamOpData.tcamEntry = tcamPointer;
			retVal = Bonsai_tcamEgrOperationPerform(dev, op, &tcamOpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Bonsai_tcamEgrOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamEgrFlushEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamEgrFlushEntryAllPorts
*
* DESCRIPTION:
*       This routine is to flush a single egress entry for all switch ports. 
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
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
MSD_STATUS Bonsai_gtcamEgrFlushEntryAllPorts
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_EGR_OP_DATA     tcamOpData;

    MSD_DBG_INFO(("Bonsai_gtcamEgrFlushEntryAllPorts Called.\n"));

    /* check if the given pointer is valid */
    if ((tcamPointer > (MSD_U32)0x3F) || (tcamPointer == (MSD_U32)0))
    {
		MSD_DBG_ERROR(("Bad tcamPointer: %u. It should be within (0,63].\n", tcamPointer));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAM_FLUSH_ENTRY;
		tcamOpData.port = (MSD_U8)0x1F;
		tcamOpData.tcamEntry = tcamPointer;
		retVal = Bonsai_tcamEgrOperationPerform(dev, op, &tcamOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_tcamEgrOperationPerform FLUSH_ENTRY for all returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamEgrFlushEntryAllPorts Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamEgrLoadEntry
*
* DESCRIPTION:
*       This routine loads a single egress TCAM entry for a specific port.
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*       tcamData    - Tcam entry Data
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
MSD_STATUS Bonsai_gtcamEgrLoadEntry
(
    IN  MSD_QD_DEV     *dev,
	IN  MSD_LPORT      port,
    IN  MSD_U32        tcamPointer,
    IN  BONSAI_MSD_TCAM_EGR_DATA  *tcamData
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_EGR_OP_DATA     tcamOpData;
    MSD_U8			hwPort;

    MSD_DBG_INFO(("Bonsai_gtcamEgrLoadEntry Called.\n"));

    /* check if the given port is valid */
    hwPort = MSD_LPORT_2_PORT(port);
    if(hwPort == (MSD_U32)MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* check if the given pointer is valid */
		if ((tcamPointer > (MSD_U32)0x3F) || (tcamPointer == (MSD_U32)0) || (tcamData == NULL))
		{
			MSD_DBG_ERROR(("tcamPointer: %u out of range (0,63] OR tcamData is NULL.\n", tcamPointer));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			/* Program Tuning register */
			op = Bonsai_TCAM_LOAD_ENTRY;
			tcamOpData.port = hwPort;
			tcamOpData.tcamEntry = tcamPointer;
			/*tcamOpData.tcamDataP = tcamData;*/
			retVal = Bonsai_setTcamEgrHWData(tcamData, &tcamOpData.tcamDataP);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Bonsai_setTcamEgrHWData returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				retVal = Bonsai_tcamEgrOperationPerform(dev, op, &tcamOpData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Bonsai_tcamEgrOperationPerform LOAD_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamEgrLoadEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamEgrGetNextTCAMData
*
* DESCRIPTION:
*       This routine finds the next higher Egress TCAM Entry number that is valid 
*       (i.e.,any entry that is non-zero). The TCAM Entry register (bits 5:0) is
*       used as the Egress TCAM entry to start from. To find the lowest number
*       Egress TCAM Entry that is valid, start the Get Next operation with Egress
*       TCAM Entry set to 0x3F.
*
* INPUTS:
*		 port		 - switch port
*        tcamPointer - pointer to start search TCAM 
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Bonsai_gtcamEgrGetNextTCAMData
(
    IN     MSD_QD_DEV     *dev,
    IN	   MSD_LPORT      port,
    INOUT  MSD_U32        *tcamPointer,
    OUT    BONSAI_MSD_TCAM_EGR_DATA  *tcamData
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_EGR_OP_DATA     tcamOpData;
    MSD_U8			hwPort;

    MSD_DBG_INFO(("Bonsai_gtcamEgrGetNextTCAMData Called.\n"));

    /* check if the given port is valid */
    hwPort = MSD_LPORT_2_PORT(port);
    if(hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* check if the given pointer is valid */
		if (tcamPointer == NULL)
		{
			MSD_DBG_ERROR(("tcamPointer is NULL.\n"));
			retVal = MSD_BAD_PARAM;
		}
		else if ((*tcamPointer > (MSD_U32)0x3F) || (tcamData == NULL))
		{
			MSD_DBG_ERROR(("tcamPointer value: %u larger than 63 OR tcamData is NULL.\n", *tcamPointer));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			/* Program Tuning register */
			op = Bonsai_TCAM_GET_NEXT_ENTRY;
			tcamOpData.port = hwPort;
			tcamOpData.tcamEntry = *tcamPointer;
			/*tcamOpData.tcamDataP = tcamData;*/
			retVal = Bonsai_tcamEgrOperationPerform(dev, op, &tcamOpData);
			if (retVal == MSD_NO_SUCH)
			{
				MSD_DBG_INFO(("No higher valid entry founded return MSD_NO_SUCH"));
			}
			else if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Bonsai_tcamEgrOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				*tcamPointer = tcamOpData.tcamEntry;
				retVal = Bonsai_getTcamEgrHWData(&tcamOpData.tcamDataP, tcamData);
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamEgrGetNextTCAMData Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamEgrReadTCAMData
*
* DESCRIPTION:
*       This routine reads a single Egress TCAM entry for a specific port.
*
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
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
MSD_STATUS Bonsai_gtcamEgrReadTCAMData
(
    IN  MSD_QD_DEV     *dev,
	IN	MSD_LPORT	  port,
    IN  MSD_U32        tcamPointer,
    OUT BONSAI_MSD_TCAM_EGR_DATA  *tcamData
)
{
    MSD_STATUS           retVal;
    BONSAI_MSD_TCAM_OPERATION    op;
    BONSAI_MSD_TCAM_EGR_OP_DATA     tcamOpData;
	MSD_U8			hwPort;

    MSD_DBG_INFO(("Bonsai_gtcamEgrReadTCAMData Called.\n"));

	/* check if the given port is valid */
	hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* check if the given pointer is valid */
		if ((tcamPointer > (MSD_U32)0x3F) || (tcamPointer == (MSD_U32)0) || (tcamData == NULL))
		{
			MSD_DBG_ERROR(("tcamPointer: %u out of range (0,63] OR tcamData is NULL.\n", tcamPointer));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			/* Program Tuning register */
			op = Bonsai_TCAM_READ_ENTRY;
			tcamOpData.port = hwPort;
			tcamOpData.tcamEntry = tcamPointer;
			/*tcamOpData.tcamDataP = tcamData;*/
			retVal = Bonsai_tcamEgrOperationPerform(dev, op, &tcamOpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Bonsai_tcamEgrOperationPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				retVal = Bonsai_getTcamEgrHWData(&tcamOpData.tcamDataP, tcamData);
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamEgrReadTCAMData Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamDump
*
* DESCRIPTION:
*       Finds all valid TCAM entries and print it out.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Bonsai_gtcamDump
(
    IN  MSD_QD_DEV *dev
)
{
    MSD_STATUS status = MSD_OK;
    MSD_U32    tcamPointer;
    BONSAI_MSD_TCAM_DATA  tcamData;

    MSD_DBG_INFO(("Bonsai_gtcamDump Called.\n"));

    tcamPointer = (MSD_U32)0xff;

    while (status == MSD_OK)
    {
		status = Bonsai_gtcamGetNextTCAMData(dev, &tcamPointer, &tcamData);
        MSG(("find the next valid TCAM entry number is %u\n", tcamPointer));
        displayTcamData(&tcamData);
        if ((tcamPointer & (MSD_U32)0x00ff) == (MSD_U32)0xff)
        {
            break;
        }
    }

    MSD_DBG_INFO(("Bonsai_gtcamDump Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Bonsai_gtcamAdvConfig
*
* DESCRIPTION:
*       This routine sets tcam entry to match standard IPv4/IPv6, TCP/UDP packets
*       with the given header data and mask.
*
* INPUTS:
*       devNum  - physical device number
*       pktType - Enumeration of TCAM mapping packet type
*       tcamPointer  - tcam entry number
*       tcamPointer2 - tcam entry2 number for cascade
*       keyMaskPtr   - mask pointer for TCAM entry key
*       keyPtr       - data pointer for TCAM entry key
*       maskPtr      - mask pointer for packet unit
*       patternPtr   - data pointer for packet unit
*       actionPtr    - action pionter
*
* OUTPUTS:
*       entry2Used - MSD_TRUE: two tcam entries been used, MSD_FALSE: only tcamPointer been used, tcamPointer2 not used.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Bonsai_gtcamAdvConfig
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_TCAM_PKT_TYPE pktType,
    IN  MSD_U32 tcamPointer,
    IN  MSD_U32 tcamPointer2,
    IN  const BONSAI_MSD_TCAM_ENT_KEY *keyMaskPtr,
    IN  const BONSAI_MSD_TCAM_ENT_KEY *keyPtr,
    IN  const MSD_TCAM_ENT_PKT_UNT *maskPtr,
    IN  const MSD_TCAM_ENT_PKT_UNT *patternPtr,
    IN  const BONSAI_MSD_TCAM_ENT_ACT *actionPtr,
    OUT MSD_BOOL *entry2Used
)
{
    MSD_STATUS retVal = MSD_OK;
    BONSAI_MSD_TCAM_DATA  tcamData;
    BONSAI_MSD_TCAM_DATA  tcamData2;
    MSD_16   i;

    MSD_DBG_INFO(("Bonsai_gtcamAdvConfig Called.\n"));

    msdMemSet(&tcamData, 0, sizeof(BONSAI_MSD_TCAM_DATA));
    msdMemSet(&tcamData2, 0, sizeof(BONSAI_MSD_TCAM_DATA));

    /*key mask*/
    tcamData.frameType = keyPtr->frameType;
	tcamData.timeKey = keyPtr->timeKey;
    tcamData.spv = keyPtr->spv;
    tcamData.ppri = keyPtr->ppri;
    tcamData.pvid = keyPtr->pvid;
    tcamData.frameTypeMask = keyMaskPtr->frameType;
	tcamData.timeKeyMask = keyMaskPtr->timeKey;
    tcamData.spvMask = keyMaskPtr->spv;
    tcamData.ppriMask = keyMaskPtr->ppri;
    tcamData.pvidMask = keyMaskPtr->pvid;

    /*DA SA VlanTag*/
    for (i = 0; i < 6; i++)
    {
        tcamData.frameOctet[i] = patternPtr->da.arEther[i];
        tcamData.frameOctet[i + 6] = patternPtr->sa.arEther[i];
        tcamData.frameOctetMask[i] = maskPtr->da.arEther[i];
        tcamData.frameOctetMask[i + 6] = maskPtr->sa.arEther[i];
    }
	tcamData.frameOctet[12] = (MSD_U8)((MSD_U32)(patternPtr->vlanTag >> 24) & (MSD_U32)0xff);
	tcamData.frameOctet[13] = (MSD_U8)((MSD_U32)(patternPtr->vlanTag >> 16) & (MSD_U32)0xff);
	tcamData.frameOctet[14] = (MSD_U8)((MSD_U32)(patternPtr->vlanTag >> 8) & (MSD_U32)0xff);
	tcamData.frameOctet[15] = (MSD_U8)(patternPtr->vlanTag & (MSD_U32)0xff);
	tcamData.frameOctetMask[12] = (MSD_U8)((MSD_U32)(maskPtr->vlanTag >> 24) & (MSD_U32)0xff);
	tcamData.frameOctetMask[13] = (MSD_U8)((MSD_U32)(maskPtr->vlanTag >> 16) & (MSD_U32)0xff);
	tcamData.frameOctetMask[14] = (MSD_U8)((MSD_U32)(maskPtr->vlanTag >> 8) & (MSD_U32)0xff);
	tcamData.frameOctetMask[15] = (MSD_U8)(maskPtr->vlanTag & (MSD_U32)0xff);

	tcamData.frameOctet[16] = (MSD_U8)((MSD_U16)(patternPtr->etherType >> 8) & (MSD_U16)0xff);
	tcamData.frameOctet[17] = (MSD_U8)(patternPtr->etherType & (MSD_U16)0xff);
	tcamData.frameOctetMask[16] = (MSD_U8)((MSD_U16)(maskPtr->etherType >> 8) & (MSD_U16)0xff);
	tcamData.frameOctetMask[17] = (MSD_U8)(maskPtr->etherType & (MSD_U16)0xff);

    /*Protcol setting*/
    switch (pktType)
    {
    case MSD_TCAM_TYPE_IPV4_TCP:
        mappingIpv4ToTcam(&tcamData, &(patternPtr->pktUnit.ipv4Udp.ipv4), &(maskPtr->pktUnit.ipv4Udp.ipv4));
        mappingTcpOverIpv4ToTcam(&tcamData, &tcamData2, &(patternPtr->pktUnit.ipv4Tcp.tcp), &(maskPtr->pktUnit.ipv4Tcp.tcp));
        break;
    case MSD_TCAM_TYPE_IPV4_UDP:
        mappingIpv4ToTcam(&tcamData, &(patternPtr->pktUnit.ipv4Udp.ipv4), &(maskPtr->pktUnit.ipv4Udp.ipv4));
        mappingUdpToTcam(38, &tcamData, &(patternPtr->pktUnit.ipv4Udp.udp), &(maskPtr->pktUnit.ipv4Udp.udp));
        break;
    case MSD_TCAM_TYPE_IPV6_TCP:
        mappingIpv6ToTcam(&tcamData, &tcamData2, &(patternPtr->pktUnit.ipv6Udp.ipv6), &(maskPtr->pktUnit.ipv6Udp.ipv6));
        mappingTcpOverIpv6ToTcam(&tcamData2, &(patternPtr->pktUnit.ipv6Tcp.tcp), &(maskPtr->pktUnit.ipv6Tcp.tcp));
        break;
    case MSD_TCAM_TYPE_IPV6_UDP:
        mappingIpv6ToTcam(&tcamData, &tcamData2, &(patternPtr->pktUnit.ipv6Udp.ipv6), &(maskPtr->pktUnit.ipv6Udp.ipv6));
        mappingUdpToTcam(10, &tcamData2, &(patternPtr->pktUnit.ipv6Udp.udp), &(maskPtr->pktUnit.ipv6Udp.udp));
        break;
    default:
        MSD_DBG_ERROR(("Input MSD_TCAM_PKT_TYPE error. \n"));
		retVal = MSD_BAD_PARAM;
		break;
    }

	if (retVal != MSD_BAD_PARAM)
	{
		if (checkEntry2Used(&tcamData2) == MSD_TRUE)
		{
			tcamData.continu = 0x4;
			tcamData.nextId = (MSD_U8)tcamPointer2;
			tcamData2.pvid = (MSD_U16)tcamPointer2;
			tcamData2.pvidMask = 0xff;
			mappingActionToTcam(&tcamData2, actionPtr);

			retVal = Bonsai_gtcamLoadEntry(dev, tcamPointer, &tcamData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Bonsai_gtcamLoadEntry for tcamData returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				retVal = Bonsai_gtcamLoadEntry(dev, tcamPointer2, &tcamData2);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Bonsai_gtcamLoadEntry for tcamData2 returned: %s.\n", msdDisplayStatus(retVal)));
				}
				else
				{
					*entry2Used = MSD_TRUE;
				}
			}
		}
		else
		{
			mappingActionToTcam(&tcamData, actionPtr);
			retVal = Bonsai_gtcamLoadEntry(dev, tcamPointer, &tcamData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Bonsai_gtcamLoadEntry for tcamData only returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				*entry2Used = MSD_FALSE;
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gtcamAdvConfig Exit.\n"));
	return retVal;
}


/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Bonsai_setTcamHWData
(
	IN	const BONSAI_MSD_TCAM_DATA	*iData,
	OUT	BONSAI_MSD_TCAM_DATA_HW	*oData
)
{
	MSD_16 i;
	
	msdMemSet(oData,0,sizeof(BONSAI_MSD_TCAM_DATA_HW));

	/*key*/
	MSD_BF_SET(oData->pg0.frame[0],iData->frameTypeMask, 14, 2);
	MSD_BF_SET(oData->pg0.frame[0],iData->frameType, 6, 2);
	MSD_BF_SET(oData->pg0.frame[0], iData->timeKeyMask, 13, 1);
	MSD_BF_SET(oData->pg0.frame[0], iData->timeKey, 5, 1);

	MSD_BF_SET(oData->pg0.frame[1],(iData->spvMask&0xff), 8, 8);
	MSD_BF_SET(oData->pg0.frame[1],(iData->spv&0xff), 0, 8);

	MSD_BF_SET(oData->pg0.frame[2],iData->ppriMask, 12, 4);
	MSD_BF_SET(oData->pg0.frame[2],iData->ppri, 4, 4);
	MSD_BF_SET(oData->pg0.frame[2],((iData->pvidMask&0xf00)>>8), 8, 4);
	MSD_BF_SET(oData->pg0.frame[2],((iData->pvid&0xf00)>>8), 0, 4);

	MSD_BF_SET(oData->pg0.frame[3],(iData->pvidMask&0xff), 8, 8);
	MSD_BF_SET(oData->pg0.frame[3],(iData->pvid&0xff), 0, 8);

	/*Page 0 (0~21) bytes*/
	for(i=4; i<26; i++)
	{
		MSD_BF_SET(oData->pg0.frame[i],iData->frameOctetMask[i-4],8,8);
		MSD_BF_SET(oData->pg0.frame[i],iData->frameOctet[i-4],0,8);
	}

	/*Page 1 (22~47) bytes*/
	for(i = 0; i < 26; i++)
	{
		MSD_BF_SET(oData->pg1.frame[i],iData->frameOctetMask[22+i],8,8);
		MSD_BF_SET(oData->pg1.frame[i],iData->frameOctet[22+i],0,8);
	}

	/*Action*/
	MSD_BF_SET(oData->pg2.frame[0],iData->continu, 13, 3);
	MSD_BF_SET(oData->pg2.frame[0],(MSD_U16)(iData->vidOverride), 12, 1);
	MSD_BF_SET(oData->pg2.frame[0],iData->vidData, 0, 12);

	MSD_BF_SET(oData->pg2.frame[1],iData->nextId, 8, 8);
	MSD_BF_SET(oData->pg2.frame[1],(MSD_U16)(iData->qpriOverride), 7, 1);
	MSD_BF_SET(oData->pg2.frame[1],iData->qpriData, 4, 3);
	MSD_BF_SET(oData->pg2.frame[1],(MSD_U16)(iData->fpriOverride), 3, 1);
	MSD_BF_SET(oData->pg2.frame[1],iData->fpriData, 0, 3);

	MSD_BF_SET(oData->pg2.frame[2],iData->dpvSF, 11, 1);
	MSD_BF_SET(oData->pg2.frame[2],(MSD_U16)(iData->dpvData), 0, 7);

	MSD_BF_SET(oData->pg2.frame[4],iData->dpvMode, 14, 2);
	MSD_BF_SET(oData->pg2.frame[4],iData->colorMode, 12, 2);
	MSD_BF_SET(oData->pg2.frame[4],(MSD_U16)(iData->vtuPageOverride), 11, 1);
	MSD_BF_SET(oData->pg2.frame[4],iData->vtuPage, 10, 1);
	MSD_BF_SET(oData->pg2.frame[4],iData->unKnownFilter, 8, 2);
	MSD_BF_SET(oData->pg2.frame[4],iData->egActPoint, 0, 6);

	MSD_BF_SET(oData->pg2.frame[5],(MSD_U16)(iData->ldBalanceOverride), 15, 1);
	MSD_BF_SET(oData->pg2.frame[5],iData->ldBalanceData, 12, 3);
	MSD_BF_SET(oData->pg2.frame[5], iData->tcamTunnel, 10, 1);
	MSD_BF_SET(oData->pg2.frame[5], iData->ipMulticast, 9, 1);
	MSD_BF_SET(oData->pg2.frame[5], iData->ip2me, 8, 1);
	MSD_BF_SET(oData->pg2.frame[5], iData->routeEntry, 7, 1);
	MSD_BF_SET(oData->pg2.frame[5],(MSD_U16)(iData->DSCPOverride), 6, 1);
	MSD_BF_SET(oData->pg2.frame[5],iData->DSCP, 0, 6);

	MSD_BF_SET(oData->pg2.frame[6],(MSD_U16)(iData->factionOverride), 15, 1);
	MSD_BF_SET(oData->pg2.frame[6],iData->factionData, 0, 15);

	MSD_BF_SET(oData->pg2.frame[10], iData->flowMeterEn, 7, 1);
	MSD_BF_SET(oData->pg2.frame[10], iData->flowMeterId, 0, 4);
	MSD_BF_SET(oData->pg2.frame[11], iData->streamFilterEn, 7, 1);
	MSD_BF_SET(oData->pg2.frame[11], iData->streamFilterId, 0, 4);

	MSD_BF_SET(oData->pg2.frame[25], iData->interrupt, 15, 1);
	MSD_BF_SET(oData->pg2.frame[25], iData->IncTcamCtr, 14, 1);
	MSD_BF_SET(oData->pg2.frame[25], iData->tcamCtr, 12, 2);
	
	return MSD_OK;
}

static MSD_STATUS Bonsai_getTcamHWData
(
	IN	const BONSAI_MSD_TCAM_DATA_HW	*iData,
	OUT	BONSAI_MSD_TCAM_DATA	*oData
)
{
	MSD_16 i;
	/*key*/
	oData->frameTypeMask = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[0],14, 2));
	oData->frameType = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[0],6, 2));
	oData->timeKeyMask = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[0], 13, 1));
	oData->timeKey = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[0], 5, 1));

    oData->spvMask = MSD_BF_GET(iData->pg0.frame[1], 8, 8);
	oData->spv = MSD_BF_GET(iData->pg0.frame[1], 0, 8);

	oData->ppriMask = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[2],12, 4));
	oData->ppri = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[2],4, 4));

	oData->pvidMask = (MSD_BF_GET(iData->pg0.frame[2], 8, 4) << 8) | MSD_BF_GET(iData->pg0.frame[3],8, 8);

	oData->pvid = (MSD_BF_GET(iData->pg0.frame[2], 0, 4) << 8) | MSD_BF_GET(iData->pg0.frame[3], 0, 8);

	/*Page 0 (0~21) bytes*/
	for(i=4; i<26; i++)
	{
		oData->frameOctetMask[i-4] = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[i],8, 8));
		oData->frameOctet[i-4] = (MSD_U8)(MSD_BF_GET(iData->pg0.frame[i],0,8));
	}

	/*Page 1 (22~47) bytes*/
	for(i = 0; i < 26; i++)
	{
		oData->frameOctetMask[22+i] = (MSD_U8)(MSD_BF_GET(iData->pg1.frame[i],8,8));
		oData->frameOctet[22+i] = (MSD_U8)(MSD_BF_GET(iData->pg1.frame[i],0,8));
	}

	/*Action*/
	oData->continu = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[0], 13, 3));
	oData->vidOverride = MSD_BF_GET(iData->pg2.frame[0], 12, 1);
	oData->vidData = MSD_BF_GET(iData->pg2.frame[0], 0, 12);

	oData->nextId = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[1], 8, 8));
	oData->qpriOverride = MSD_BF_GET(iData->pg2.frame[1], 7, 1);
	oData->qpriData = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[1], 4, 3));
	oData->fpriOverride = MSD_BF_GET(iData->pg2.frame[1], 3, 1);
	oData->fpriData = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[1], 0, 3));

	oData->dpvSF = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[2], 11, 1));
	oData->dpvData = MSD_BF_GET(iData->pg2.frame[2], 0, 7);

	oData->dpvMode = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 14, 2));
	oData->colorMode = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 12, 2));
	oData->vtuPageOverride = MSD_BF_GET(iData->pg2.frame[4], 11, 1);
	oData->vtuPage = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 10, 1));
	oData->unKnownFilter = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 8, 2));
	oData->egActPoint = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[4], 0, 6));

	oData->ldBalanceOverride = MSD_BF_GET(iData->pg2.frame[5], 15, 1);
	oData->ldBalanceData = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[5], 12, 3));
	oData->tcamTunnel = MSD_BF_GET(iData->pg2.frame[5], 10, 1);
	oData->ipMulticast = MSD_BF_GET(iData->pg2.frame[5], 9, 1);
	oData->ip2me = MSD_BF_GET(iData->pg2.frame[5], 8, 1);
	oData->routeEntry = MSD_BF_GET(iData->pg2.frame[5], 7, 1);
	oData->DSCPOverride = MSD_BF_GET(iData->pg2.frame[5], 6, 1);
	oData->DSCP = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[5], 0, 6));

	oData->factionOverride = MSD_BF_GET(iData->pg2.frame[6], 15, 1);
	oData->factionData = MSD_BF_GET(iData->pg2.frame[6], 0, 15);

	oData->flowMeterEn = MSD_BF_GET(iData->pg2.frame[10], 7, 1);
	oData->flowMeterId = MSD_BF_GET(iData->pg2.frame[10], 0, 4);
	oData->streamFilterEn = MSD_BF_GET(iData->pg2.frame[11], 7, 1);
	oData->streamFilterId = MSD_BF_GET(iData->pg2.frame[11], 0, 4);

	oData->interrupt = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[25], 15, 1));
	oData->IncTcamCtr = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[25], 14, 1));
	oData->tcamCtr = (MSD_U8)(MSD_BF_GET(iData->pg2.frame[25], 12, 2));

	return MSD_OK;
}

static MSD_STATUS Bonsai_setTcamEgrHWData
(
	IN	const BONSAI_MSD_TCAM_EGR_DATA	*iData,
	OUT	BONSAI_MSD_TCAM_EGR_DATA_HW	*oData
)
{
	msdMemSet(oData,0,sizeof(BONSAI_MSD_TCAM_EGR_DATA_HW));

	MSD_BF_SET(oData->frame[0],(MSD_U16)(iData->frameModeOverride), 14, 1);
	MSD_BF_SET(oData->frame[0],iData->frameMode, 12, 2);
	MSD_BF_SET(oData->frame[0], iData->noTtlDec, 11, 1);
	MSD_BF_SET(oData->frame[0],(MSD_U16)(iData->tagModeOverride), 10, 1);
	MSD_BF_SET(oData->frame[0],iData->tagMode, 8, 2);
	MSD_BF_SET(oData->frame[0],iData->daMode, 4, 2);
	MSD_BF_SET(oData->frame[0],iData->saMode, 0, 3);

	MSD_BF_SET(oData->frame[1],(MSD_U16)(iData->egVidModeOverride), 14, 1);
	MSD_BF_SET(oData->frame[1],iData->egVidMode, 12, 2);
	MSD_BF_SET(oData->frame[1],iData->egVidData, 0, 12);

	MSD_BF_SET(oData->frame[2],iData->egDSCPMode, 14, 2);
	MSD_BF_SET(oData->frame[2],iData->egDSCP, 8, 6);
	MSD_BF_SET(oData->frame[2],(MSD_U16)(iData->egfpriModeOverride), 6, 1);
	MSD_BF_SET(oData->frame[2],iData->egfpriMode, 4, 2);
	MSD_BF_SET(oData->frame[2],iData->egEC, 3, 1);
	MSD_BF_SET(oData->frame[2],iData->egFPRI, 0, 3);

	MSD_BF_SET(oData->frame[3], iData->egSIDOverride, 6, 1);
	MSD_BF_SET(oData->frame[3], iData->egSID, 0, 6);
	
	return MSD_OK;
}

static MSD_STATUS Bonsai_getTcamEgrHWData
(
	IN	const BONSAI_MSD_TCAM_EGR_DATA_HW	*iData,
	OUT	BONSAI_MSD_TCAM_EGR_DATA	*oData
)
{
	oData->frameModeOverride = MSD_BF_GET(iData->frame[0], 14, 1);
	oData->frameMode = (MSD_U8)(MSD_BF_GET(iData->frame[0], 12, 2));
	oData->noTtlDec = MSD_BF_GET(iData->frame[0], 11, 1);
	oData->tagModeOverride = MSD_BF_GET(iData->frame[0], 10, 1);
	oData->tagMode = (MSD_U8)(MSD_BF_GET(iData->frame[0], 8, 2));
	oData->daMode = (MSD_U8)(MSD_BF_GET(iData->frame[0], 4, 2));
	oData->saMode = (MSD_U8)(MSD_BF_GET(iData->frame[0], 0, 3));

	oData->egVidModeOverride = MSD_BF_GET(iData->frame[1], 14, 1);
	oData->egVidMode = (MSD_U8)(MSD_BF_GET(iData->frame[1], 12, 2));
	oData->egVidData = MSD_BF_GET(iData->frame[1], 0, 12);

	oData->egDSCPMode = (MSD_U8)MSD_BF_GET(iData->frame[2], 14, 2);
	oData->egDSCP = (MSD_U8)(MSD_BF_GET(iData->frame[2], 8, 6));
	oData->egfpriModeOverride = MSD_BF_GET(iData->frame[2], 6, 1);
	oData->egfpriMode = (MSD_U8)(MSD_BF_GET(iData->frame[2], 4, 2));
	oData->egEC = (MSD_U8)(MSD_BF_GET(iData->frame[2], 3, 1));
	oData->egFPRI = (MSD_U8)(MSD_BF_GET(iData->frame[2], 0, 3));

	oData->egSIDOverride = MSD_BF_GET(iData->frame[3], 6, 1);
	oData->egSID = (MSD_U8)(MSD_BF_GET(iData->frame[3], 0, 6));

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamSetPage0Data(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16 i;

	for(i = 2U; i < 0x1cU; i++)
	{
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataPtr->pg0.frame[i - 2U]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamSetPage1Data(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16 i;

	for(i = 2U; i < 0x1cU; i++)
	{
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataPtr->pg1.frame[i - 2U]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}

static MSD_STATUS Bonsai_tcamSetPage2Data(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16 i;

	for(i=2U; i<14U; i++)
	{
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataPtr->pg2.frame[i - 2U]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)0x1B, tcamDataPtr->pg2.frame[25]);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamSetPage3Data(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_EGR_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16 i;

	for(i=2U; i<6U; i++)
	{
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataPtr->frame[i - 2U]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamGetPage0Data(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16 i;


	for(i=2U; i<0x1cU; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataPtr->pg0.frame[i - 2U]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamGetPage1Data(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16 i;


	for(i=2U; i<0x1cU; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataPtr->pg1.frame[i - 2U]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamGetPage2Data(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16 i;

	for(i=2U; i<14U; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataPtr->pg2.frame[i - 2U]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)0x1B, &tcamDataPtr->pg2.frame[25]);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamGetPage3Data(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_EGR_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16 i;

	for(i=2U; i<6U; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataPtr->frame[i - 2U]);
		if(retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_waitTcamReady(const MSD_QD_DEV *dev)
{
    MSD_STATUS       retVal;    /* Functions return value */

    MSD_U16          data;     /* temporary Data storage */

	data = 1U;
	while (data == 1U)
    {
		retVal = msdGetAnyRegField(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
    }
    return MSD_OK;
}

/*******************************************************************************
* Bonsai_tcamOperationPerform
*
* DESCRIPTION:
*       This function accesses TCAM Table
*
* INPUTS:
*       tcamOp   - The tcam operation
*       tcamData - address and data to be written into TCAM
*
* OUTPUTS:
*       tcamData - data read from TCAM pointed by address
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MSD_STATUS Bonsai_tcamOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    BONSAI_MSD_TCAM_OPERATION   tcamOp,
    INOUT BONSAI_MSD_TCAM_OP_DATA     *opData
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          data;     /* temporary Data storage */

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	/* Wait until the tcam in ready. */
	retVal = Bonsai_waitTcamReady(dev);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	/* Set the TCAM Operation register */
	switch (tcamOp)
	{
		case Bonsai_TCAM_FLUSH_ALL:
		{
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)tcamOp << 12));
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		case Bonsai_TCAM_FLUSH_ENTRY:
		{
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		case Bonsai_TCAM_LOAD_ENTRY:
		/*    case Bonsai_TCAM_PURGE_ENTRY: */
		{
			/* load Page 2 */
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			/*Access Ingress Actions from TCAM Frame matches */
			retVal = Bonsai_setTcamExtensionReg(dev, 0, 0);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Bonsai_tcamSetPage2Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Bonsai_TCAM_LOAD_ENTRY << 12) | (MSD_U16)((MSD_U16)2 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			/* load Page 1 */
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			retVal = Bonsai_tcamSetPage1Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Bonsai_TCAM_LOAD_ENTRY << 12) | (MSD_U16)((MSD_U16)1 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			/* load Page 0 */
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			retVal = Bonsai_tcamSetPage0Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Bonsai_TCAM_LOAD_ENTRY << 12) | (MSD_U16)((MSD_U16)0 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

		}
		break;

		case Bonsai_TCAM_GET_NEXT_ENTRY:
		{
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, &data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			if ((data & (MSD_U16)0xff) == (MSD_U16)0xff)
			{
				MSD_U16 data1;
				retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_P0_KEYS_1, &data1);
				if(retVal != MSD_OK)
				{
					msdSemGive(dev->devNum,  dev->tblRegsSem);
					return retVal;
				}
				if (data1 == (MSD_U16)0x00ff)
				{
					/* No higher valid TCAM entry */
					msdSemGive(dev->devNum,  dev->tblRegsSem);
					return MSD_NO_SUCH;
				}
				else
				{
					/* The highest valid TCAM entry found*/
				}
			}

			/* Get next entry and read the entry */
			opData->tcamEntry = (MSD_U32)data & (MSD_U32)0xff;

		}
		case Bonsai_TCAM_READ_ENTRY:
		{
			tcamOp = Bonsai_TCAM_READ_ENTRY;

			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum,  dev->tblRegsSem);
				return retVal;
			}

			/* Read page 0 */
			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)((MSD_U16)0 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Bonsai_tcamGetPage0Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Read page 1 */
			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)((MSD_U16)1 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Bonsai_tcamGetPage1Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/*Access Ingress Actions from TCAM Frame matches*/
			retVal = Bonsai_setTcamExtensionReg(dev, 0, 0);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Read page 2 */
			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)((MSD_U16)2 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Bonsai_tcamGetPage2Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		default:
			retVal = MSD_FAIL;
			break;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;
}


static MSD_STATUS Bonsai_tcamEgrOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    BONSAI_MSD_TCAM_OPERATION   tcamOp,
    INOUT BONSAI_MSD_TCAM_EGR_OP_DATA     *opData
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          data;     /* temporary Data storage */

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	/* Wait until the tcam in ready. */
	retVal = Bonsai_waitTcamReady(dev);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set the TCAM Operation register */
	switch (tcamOp)
	{
		case Bonsai_TCAM_FLUSH_ENTRY:
		{
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/*Access Egress Actions from Egress Action Pointer */
			retVal = Bonsai_setTcamExtensionReg(dev, 0, opData->port);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_EGR_PORT, 0);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		case Bonsai_TCAM_LOAD_ENTRY:
		{
			/* load Page 2 */
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/*Access Egress Actions from Egress Action Pointer */
			retVal = Bonsai_setTcamExtensionReg(dev, 0, opData->port);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Bonsai_tcamSetPage3Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Bonsai_TCAM_LOAD_ENTRY << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

		}
		break;

		case Bonsai_TCAM_GET_NEXT_ENTRY:
		{
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/*Access Egress Actions from Egress Action Pointer */
			retVal = Bonsai_setTcamExtensionReg(dev, 0, opData->port);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, &data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			if ((data & (MSD_U16)0x3f) == (MSD_U16)0x3f)
			{
				MSD_U16 data1,data2,data3;
				retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_EGR_ACTION_1, &data1);
				if(retVal != MSD_OK)
				{
					msdSemGive(dev->devNum, dev->tblRegsSem);
					return retVal;
				}

				retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_EGR_ACTION_2, &data2);
				if(retVal != MSD_OK)
				{
					msdSemGive(dev->devNum, dev->tblRegsSem);
					return retVal;
				}

				retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_EGR_ACTION_3, &data3);
				if(retVal != MSD_OK)
				{
					msdSemGive(dev->devNum, dev->tblRegsSem);
					return retVal;
				}

				if((data1 == 0U) && (data2 == 0U) && (data3 == 0U))
				{
					/* No higher valid TCAM entry */
					msdSemGive(dev->devNum, dev->tblRegsSem);
					return MSD_NO_SUCH;
				}
				else
				{
					/* The highest valid TCAM entry found*/
				}
			}

			/* Get next entry and read the entry */
			opData->tcamEntry = (MSD_U32)(data & (MSD_U32)0xff);

		}

		case Bonsai_TCAM_READ_ENTRY:
		{
			tcamOp = Bonsai_TCAM_READ_ENTRY;

			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/*Access Egress Actions from Egress Action Pointer */
			retVal = Bonsai_setTcamExtensionReg(dev, 0, opData->port);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Read page 3 */
			data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)tcamOp << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->tcamEntry);
			retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			retVal = Bonsai_tcamGetPage3Data(dev, &opData->tcamDataP);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}

			/* Wait until the tcam in ready. */
			retVal = Bonsai_waitTcamReady(dev);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
		}
		break;

		default:
			retVal = MSD_FAIL;
			break;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;
}

static void displayTcamData(BONSAI_MSD_TCAM_DATA *tcamData)
{
	MSD_16 i;

	MSG(("----- Key & Mask -----\n"));

	MSG(("frameType         : %04x\n", tcamData->frameType));
	MSG(("frameTypeMask     : %04x\n", tcamData->frameTypeMask));
	MSG(("timeKey           : %04x\n", tcamData->timeKey));
	MSG(("timeKeyMask       : %04x\n", tcamData->timeKeyMask));
	MSG(("spv               : %04x\n", tcamData->spv));
	MSG(("spvMask           : %04x\n", tcamData->spvMask));
	MSG(("ppri              : %04x\n", tcamData->ppri));
	MSG(("ppriMask          : %04x\n", tcamData->ppriMask));
	MSG(("pvid              : %04x\n", tcamData->pvid));
	MSG(("pvidMask          : %04x\n", tcamData->pvidMask));

	MSG(("----- Ethernet Frame Content -----\n"));

	for (i = 0; i<48; i++)
	{
		MSG(("frame data Octet %2d         : %4x\n", i + 1, tcamData->frameOctet[i]));
		MSG(("frame data OctetMask %2d     : %4x\n", i + 1, tcamData->frameOctetMask[i]));
	}

	MSG(("----- Ingress Action -----\n"));

	MSG(("continu           : %04x\n", tcamData->continu));
	MSG(("interrupt         : %04x\n", tcamData->interrupt));
	MSG(("IncTcamCtr        : %04x\n", tcamData->IncTcamCtr));
	MSG(("vidOverride       : %04x\n", tcamData->vidOverride));
	MSG(("vidData           : %04x\n", tcamData->vidData));

	MSG(("nextId            : %04x\n", tcamData->nextId));
	MSG(("qpriOverride      : %04x\n", tcamData->qpriOverride));
	MSG(("qpriData          : %04x\n", tcamData->qpriData));
	MSG(("fpriOverride      : %04x\n", tcamData->fpriOverride));
	MSG(("fpriData          : %04x\n", tcamData->fpriData));

	MSG(("dpvSF             : %04x\n", tcamData->dpvSF));
	MSG(("dpvData           : %04x\n", (MSD_U32)tcamData->dpvData));
	MSG(("dpvMode           : %04x\n", tcamData->dpvMode));

	MSG(("unKnownFilter     : %04x\n", tcamData->unKnownFilter));

	MSG(("ldBalanceOverride : %04x\n", tcamData->ldBalanceOverride));
	MSG(("ldBalanceData     : %04x\n", tcamData->ldBalanceData));

	MSG(("factionOverride   : %04x\n", tcamData->factionOverride));
	MSG(("factionData       : %04x\n", tcamData->factionData));
	MSG(("colorMode         : %04x\n", tcamData->colorMode));
	MSG(("vtuPageOverride   : %04x\n", tcamData->vtuPageOverride));
	MSG(("vtuPage           : %04x\n", tcamData->vtuPage));
	MSG(("egActPoint        : %04x\n", tcamData->egActPoint));
	MSG(("DSCPOverride      : %04x\n", tcamData->DSCPOverride));
	MSG(("DSCP              : %04x\n", tcamData->DSCP));
	MSG(("ipMulticast       : %04x\n", tcamData->ipMulticast));
	MSG(("ip2me             : %04x\n", tcamData->ip2me));
	MSG(("routeEntry        : %04x\n", tcamData->routeEntry));
	MSG(("FlowMeterEn       : %04x\n", tcamData->flowMeterEn));
	MSG(("FlowMeterId       : %04x\n", tcamData->flowMeterId));
	MSG(("streamFilterEn    : %04x\n", tcamData->streamFilterEn));
	MSG(("streamFilterId    : %04x\n", tcamData->streamFilterId));
	MSG(("tcamCtr           : %04x\n", tcamData->tcamCtr));
}

static MSD_BOOL checkEntry2Used(const BONSAI_MSD_TCAM_DATA *tcamData2)
{
    MSD_BOOL retVal = MSD_FALSE;
    MSD_16 i;

    for (i = 0; i < 48; i++)
    {
		if (tcamData2->frameOctetMask[i] != (MSD_U8)0)
        {
			retVal = MSD_TRUE;
            break;
        }
    }
	return retVal;
}
static void mappingTcpOverIpv4ToTcam(BONSAI_MSD_TCAM_DATA *tcamData, BONSAI_MSD_TCAM_DATA *tcamData2, const MSD_TCAM_ENT_TCP_UNT *pTcpData, const MSD_TCAM_ENT_TCP_UNT *pTcpMask)
{
	tcamData->frameOctet[38] = (MSD_U8)((MSD_U16)(pTcpData->srcPort >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[39] = (MSD_U8)(pTcpData->srcPort & (MSD_U16)0xff);
	tcamData->frameOctet[40] = (MSD_U8)((MSD_U16)(pTcpData->destPort >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[41] = (MSD_U8)(pTcpData->destPort & (MSD_U16)0xff);
	tcamData->frameOctet[42] = (MSD_U8)((MSD_U32)(pTcpData->seqNum >> 24) & (MSD_U32)0xff);
	tcamData->frameOctet[43] = (MSD_U8)((MSD_U32)(pTcpData->seqNum >> 16) & (MSD_U32)0xff);
	tcamData->frameOctet[44] = (MSD_U8)((MSD_U32)(pTcpData->seqNum >> 8) & (MSD_U32)0xff);
	tcamData->frameOctet[45] = (MSD_U8)(pTcpData->seqNum & (MSD_U32)0xff);
	tcamData->frameOctet[46] = (MSD_U8)((MSD_U32)(pTcpData->ackNum >> 24) & (MSD_U32)0xff);
	tcamData->frameOctet[47] = (MSD_U8)((MSD_U32)(pTcpData->ackNum >> 16) & (MSD_U32)0xff);
	tcamData2->frameOctet[0] = (MSD_U8)((MSD_U32)(pTcpData->ackNum >> 8) & (MSD_U32)0xff);
	tcamData2->frameOctet[1] = (MSD_U8)(pTcpData->ackNum & (MSD_U32)0xff);
	tcamData2->frameOctet[2] = (MSD_U8)(pTcpData->offset << 4);
	tcamData2->frameOctet[3] = (MSD_U8)((pTcpData->flags) & (MSD_U16)0x3f);
	tcamData2->frameOctet[4] = (MSD_U8)((MSD_U16)(pTcpData->windowSize >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctet[5] = (MSD_U8)((pTcpData->windowSize) & (MSD_U16)0xff);
	tcamData2->frameOctet[6] = (MSD_U8)((MSD_U16)(pTcpData->chkSum >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctet[7] = (MSD_U8)((MSD_U16)pTcpData->chkSum & (MSD_U16)0xff);
	tcamData2->frameOctet[8] = (MSD_U8)((MSD_U16)(pTcpData->urgPtr >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctet[9] = (MSD_U8)(pTcpData->urgPtr & (MSD_U16)0xff);

	tcamData->frameOctetMask[38] = (MSD_U8)((MSD_U16)(pTcpMask->srcPort >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[39] = (MSD_U8)(pTcpMask->srcPort & (MSD_U16)0xff);
	tcamData->frameOctetMask[40] = (MSD_U8)((MSD_U16)(pTcpMask->destPort >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[41] = (MSD_U8)(pTcpMask->destPort & (MSD_U16)0xff);
	tcamData->frameOctetMask[42] = (MSD_U8)((MSD_U32)(pTcpMask->seqNum >> 24) & (MSD_U32)0xff);
	tcamData->frameOctetMask[43] = (MSD_U8)((MSD_U32)(pTcpMask->seqNum >> 16) & (MSD_U32)0xff);
	tcamData->frameOctetMask[44] = (MSD_U8)((MSD_U32)(pTcpMask->seqNum >> 8) & (MSD_U32)0xff);
	tcamData->frameOctetMask[45] = (MSD_U8)(pTcpMask->seqNum & (MSD_U32)0xff);
    tcamData->frameOctetMask[46] = (MSD_U8)((MSD_U32)(pTcpMask->ackNum >> 24) & (MSD_U32)0xff);
    tcamData->frameOctetMask[47] = (MSD_U8)((MSD_U32)(pTcpMask->ackNum >> 16) & (MSD_U32)0xff);
	tcamData2->frameOctetMask[0] = (MSD_U8)((MSD_U32)(pTcpMask->ackNum >> 8) & (MSD_U32)0xff);
	tcamData2->frameOctetMask[1] = (MSD_U8)((pTcpMask->ackNum) & (MSD_U32)0xff);
	tcamData2->frameOctetMask[2] = (MSD_U8)(pTcpMask->offset << 4);
	tcamData2->frameOctetMask[3] = (MSD_U8)((pTcpMask->flags) & (MSD_U16)0x3f);
	tcamData2->frameOctetMask[4] = (MSD_U8)((MSD_U16)(pTcpMask->windowSize >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[5] = (MSD_U8)((pTcpMask->windowSize) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[6] = (MSD_U8)((MSD_U16)(pTcpMask->chkSum >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[7] = (MSD_U8)((pTcpMask->chkSum) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[8] = (MSD_U8)((MSD_U16)(pTcpMask->urgPtr >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[9] = (MSD_U8)((pTcpMask->urgPtr) & (MSD_U16)0xff);
}
static void mappingTcpOverIpv6ToTcam(BONSAI_MSD_TCAM_DATA *tcamData2, const MSD_TCAM_ENT_TCP_UNT *pTcpData, const MSD_TCAM_ENT_TCP_UNT *pTcpMask)
{
	tcamData2->frameOctet[10] = (MSD_U8)((MSD_U16)(pTcpData->srcPort >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctet[11] = (MSD_U8)(pTcpData->srcPort & (MSD_U16)0xff);
	tcamData2->frameOctet[12] = (MSD_U8)((MSD_U16)(pTcpData->destPort >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctet[13] = (MSD_U8)(pTcpData->destPort & (MSD_U16)0xff);
    tcamData2->frameOctet[14] = (MSD_U8)((MSD_U32)(pTcpData->seqNum >> 24) & (MSD_U32)0xff);
    tcamData2->frameOctet[15] = (MSD_U8)((MSD_U32)(pTcpData->seqNum >> 16) & (MSD_U32)0xff);
	tcamData2->frameOctet[16] = (MSD_U8)((MSD_U32)(pTcpData->seqNum >> 8) & (MSD_U32)0xff);
	tcamData2->frameOctet[17] = (MSD_U8)(pTcpData->seqNum & (MSD_U32)0xff);
	tcamData2->frameOctet[18] = (MSD_U8)((MSD_U32)(pTcpData->ackNum >> 24) & (MSD_U32)0xff);
	tcamData2->frameOctet[19] = (MSD_U8)((MSD_U32)(pTcpData->ackNum >> 16) & (MSD_U32)0xff);
	tcamData2->frameOctet[20] = (MSD_U8)((MSD_U32)(pTcpData->ackNum >> 8) & (MSD_U32)0xff);
	tcamData2->frameOctet[21] = (MSD_U8)(pTcpData->ackNum & (MSD_U32)0xff);
	tcamData2->frameOctet[22] = (MSD_U8)(pTcpData->offset << 4);
	tcamData2->frameOctet[23] = (MSD_U8)((pTcpData->flags) & (MSD_U16)0x3f);
	tcamData2->frameOctet[24] = (MSD_U8)((MSD_U16)(pTcpData->windowSize >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctet[25] = (MSD_U8)(pTcpData->windowSize & (MSD_U16)0xff);
	tcamData2->frameOctet[26] = (MSD_U8)((MSD_U16)(pTcpData->chkSum >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctet[27] = (MSD_U8)(pTcpData->chkSum & (MSD_U16)0xff);
	tcamData2->frameOctet[28] = (MSD_U8)((MSD_U16)(pTcpData->urgPtr >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctet[29] = (MSD_U8)(pTcpData->urgPtr & (MSD_U16)0xff);

	tcamData2->frameOctetMask[10] = (MSD_U8)((MSD_U16)(pTcpMask->srcPort >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[11] = (MSD_U8)(pTcpMask->srcPort & (MSD_U16)0xff);
	tcamData2->frameOctetMask[12] = (MSD_U8)((MSD_U16)(pTcpMask->destPort >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[13] = (MSD_U8)(pTcpMask->destPort & (MSD_U16)0xff);
    tcamData2->frameOctetMask[14] = (MSD_U8)((MSD_U32)(pTcpMask->seqNum >> 24) & (MSD_U32)0xff);
    tcamData2->frameOctetMask[15] = (MSD_U8)((MSD_U32)(pTcpMask->seqNum >> 16) & (MSD_U32)0xff);
	tcamData2->frameOctetMask[16] = (MSD_U8)((MSD_U32)(pTcpMask->seqNum >> 8) & (MSD_U32)0xff);
	tcamData2->frameOctetMask[17] = (MSD_U8)(pTcpMask->seqNum & (MSD_U32)0xff);
	tcamData2->frameOctetMask[18] = (MSD_U8)((MSD_U32)(pTcpMask->ackNum >> 24) & (MSD_U32)0xff);
	tcamData2->frameOctetMask[19] = (MSD_U8)((MSD_U32)(pTcpMask->ackNum >> 16) & (MSD_U32)0xff);
	tcamData2->frameOctetMask[20] = (MSD_U8)((MSD_U32)(pTcpMask->ackNum >> 8) & (MSD_U32)0xff);
	tcamData2->frameOctetMask[21] = (MSD_U8)(pTcpMask->ackNum & (MSD_U32)0xff);
	tcamData2->frameOctetMask[22] = (MSD_U8)(pTcpMask->offset << 4);
	tcamData2->frameOctetMask[23] = (MSD_U8)(pTcpMask->flags & (MSD_U16)0x3f);
	tcamData2->frameOctetMask[24] = (MSD_U8)((MSD_U16)(pTcpMask->windowSize >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[25] = (MSD_U8)(pTcpMask->windowSize & (MSD_U16)0xff);
	tcamData2->frameOctetMask[26] = (MSD_U8)((MSD_U16)(pTcpMask->chkSum >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[27] = (MSD_U8)(pTcpMask->chkSum & (MSD_U16)0xff);
	tcamData2->frameOctetMask[28] = (MSD_U8)((MSD_U16)(pTcpMask->urgPtr >> 8) & (MSD_U16)0xff);
	tcamData2->frameOctetMask[29] = (MSD_U8)(pTcpMask->urgPtr & (MSD_U16)0xff);
}
static void mappingIpv6ToTcam(BONSAI_MSD_TCAM_DATA *tcamData, BONSAI_MSD_TCAM_DATA *tcamData2, const MSD_TCAM_ENT_IPV6_UNT *pIpv6, const MSD_TCAM_ENT_IPV6_UNT *pIpv6Mask)
{
    MSD_32 i;
    MSD_U16 data;

    /*Data for tcamData*/
	data = (MSD_U16)((MSD_U16)pIpv6->version << 12) | (MSD_U16)((MSD_U16)pIpv6->tc << 4) | (MSD_U16)(pIpv6->flowLbl >> 16);
	tcamData->frameOctet[18] = (MSD_U8)((MSD_U16)(data >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[19] = (MSD_U8)(data & (MSD_U16)0xff);
	tcamData->frameOctet[20] = (MSD_U8)((MSD_U32)(pIpv6->flowLbl >> 8) & (MSD_U32)0xff);
	tcamData->frameOctet[21] = (MSD_U8)(pIpv6->flowLbl & (MSD_U32)0xff);
	tcamData->frameOctet[22] = (MSD_U8)((MSD_U16)(pIpv6->payloadLen >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[23] = (MSD_U8)(pIpv6->payloadLen & (MSD_U16)0xff);
    tcamData->frameOctet[24] = pIpv6->nextHdr;
    tcamData->frameOctet[25] = pIpv6->hopLimit;
    for (i = 0; i < 8; i++)
    {
		tcamData->frameOctet[26 + (2 * i)] = (MSD_U8)((MSD_U16)(pIpv6->sip[i] >> 8) & (MSD_U16)0xff);
		tcamData->frameOctet[26 + (2 * i) + 1] = (MSD_U8)((pIpv6->sip[i]) & (MSD_U16)0xff);
    }
    for (i = 0; i < 3; i++)
    {
		tcamData->frameOctet[42 + (2 * i)] = (MSD_U8)((MSD_U16)(pIpv6->dip[i] >> 8) & (MSD_U16)0xff);
		tcamData->frameOctet[42 + (2 * i) + 1] = (MSD_U8)((pIpv6->dip[i]) & (MSD_U16)0xff);
    }
    /*Data mask for tcamData*/
	data = (MSD_U16)((MSD_U16)pIpv6Mask->version << 12) | (MSD_U16)((MSD_U16)pIpv6Mask->tc << 4) | (MSD_U16)(pIpv6Mask->flowLbl >> 16);
	tcamData->frameOctetMask[18] = (MSD_U8)((MSD_U16)(data >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[19] = (MSD_U8)(data & (MSD_U16)0xff);
	tcamData->frameOctetMask[20] = (MSD_U8)((MSD_U32)(pIpv6Mask->flowLbl >> 8) & (MSD_U32)0xff);
	tcamData->frameOctetMask[21] = (MSD_U8)((pIpv6Mask->flowLbl) & (MSD_U32)0xff);
	tcamData->frameOctetMask[22] = (MSD_U8)((MSD_U16)(pIpv6Mask->payloadLen >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[23] = (MSD_U8)(pIpv6Mask->payloadLen & (MSD_U16)0xff);
    tcamData->frameOctetMask[24] = pIpv6Mask->nextHdr;
    tcamData->frameOctetMask[25] = pIpv6Mask->hopLimit;
    for (i = 0; i < 8; i++)
    {
		tcamData->frameOctetMask[26 + (2 * i)] = (MSD_U8)((MSD_U16)(pIpv6Mask->sip[i] >> 8) & (MSD_U16)0xff);
		tcamData->frameOctetMask[26 + (2 * i) + 1] = (MSD_U8)((pIpv6Mask->sip[i]) & (MSD_U16)0xff);
    }
    for (i = 0; i < 3; i++)
    {
		tcamData->frameOctetMask[42 + (2 * i)] = (MSD_U8)((MSD_U16)(pIpv6Mask->dip[i] >> 8) & (MSD_U16)0xff);
		tcamData->frameOctetMask[42 + (2 * i) + 1] = (MSD_U8)((pIpv6Mask->dip[i]) & (MSD_U16)0xff);
    }

    for (i = 0; i < 5; i++)
    {
		tcamData2->frameOctet[0 + (2 * i)] = (MSD_U8)((MSD_U16)(pIpv6->dip[3 + i] >> 8) & (MSD_U16)0xff);
		tcamData2->frameOctet[0 + (2 * i) + 1] = (MSD_U8)((pIpv6->dip[3 + i]) & (MSD_U16)0xff);
		tcamData2->frameOctetMask[0 + (2 * i)] = (MSD_U8)((MSD_U16)(pIpv6Mask->dip[3 + i] >> 8) & (MSD_U16)0xff);
		tcamData2->frameOctetMask[0 + (2 * i) + 1] = (MSD_U8)((pIpv6Mask->dip[3 + i]) & (MSD_U16)0xff);
    }
}
static void mappingIpv4ToTcam(BONSAI_MSD_TCAM_DATA *tcamData, const MSD_TCAM_ENT_IPV4_UNT *pIpv4Data, const MSD_TCAM_ENT_IPV4_UNT *pIpv4Mask)
{
    MSD_32 i;

	tcamData->frameOctet[18] = (MSD_U8)(pIpv4Data->version << 4) | pIpv4Data->ihl;
    tcamData->frameOctet[19] = pIpv4Data->tos;
	tcamData->frameOctet[20] = (MSD_U8)((MSD_U16)(pIpv4Data->length >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[21] = (MSD_U8)(pIpv4Data->length & (MSD_U16)0xff);
	tcamData->frameOctet[22] = (MSD_U8)((MSD_U16)(pIpv4Data->id >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[23] = (MSD_U8)(pIpv4Data->id & (MSD_U16)0xff);
	tcamData->frameOctet[24] = (MSD_U8)((MSD_U16)(((MSD_U16)((MSD_U16)pIpv4Data->flag << 13) | pIpv4Data->fragOffset) >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[25] = (MSD_U8)(((MSD_U16)((MSD_U16)pIpv4Data->flag << 13) | pIpv4Data->fragOffset) & (MSD_U16)0xff);
    tcamData->frameOctet[26] = pIpv4Data->ttl;
    tcamData->frameOctet[27] = pIpv4Data->protocol;
	tcamData->frameOctet[28] = (MSD_U8)((MSD_U16)(pIpv4Data->hdrChkSum >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[29] = (MSD_U8)((pIpv4Data->hdrChkSum) & (MSD_U16)0xff);
    for (i = 0; i < 4; i++)
    {
        tcamData->frameOctet[30 + i] = pIpv4Data->sip[i];
        tcamData->frameOctet[34 + i] = pIpv4Data->dip[i];
    }

	tcamData->frameOctetMask[18] = (MSD_U8)(pIpv4Mask->version << 4) | pIpv4Mask->ihl;
    tcamData->frameOctetMask[19] = pIpv4Mask->tos;
	tcamData->frameOctetMask[20] = (MSD_U8)((MSD_U16)(pIpv4Mask->length >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[21] = (MSD_U8)(pIpv4Mask->length & (MSD_U16)0xff);
	tcamData->frameOctetMask[22] = (MSD_U8)((MSD_U16)(pIpv4Mask->id >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[23] = (MSD_U8)(pIpv4Mask->id & (MSD_U16)0xff);
	tcamData->frameOctetMask[24] = (MSD_U8)((MSD_U16)(((MSD_U16)((MSD_U16)pIpv4Mask->flag << 13) | pIpv4Mask->fragOffset) >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[25] = (MSD_U8)(((MSD_U16)((MSD_U16)pIpv4Mask->flag << 13) | pIpv4Mask->fragOffset) & (MSD_U16)0xff);
    tcamData->frameOctetMask[26] = pIpv4Mask->ttl;
    tcamData->frameOctetMask[27] = pIpv4Mask->protocol;
	tcamData->frameOctetMask[28] = (MSD_U8)((MSD_U16)(pIpv4Mask->hdrChkSum >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[29] = (MSD_U8)(pIpv4Mask->hdrChkSum & (MSD_U16)0xff);
    for (i = 0; i < 4; i++)
    {
        tcamData->frameOctetMask[30 + i] = pIpv4Mask->sip[i];
        tcamData->frameOctetMask[34 + i] = pIpv4Mask->dip[i];
    }
}
static void mappingUdpToTcam(MSD_16 startIndex, BONSAI_MSD_TCAM_DATA *tcamData, const MSD_TCAM_ENT_UDP_UNT *pUdpData, const MSD_TCAM_ENT_UDP_UNT *pUdpMask)
{
	tcamData->frameOctet[startIndex + 0] = (MSD_U8)((MSD_U16)(pUdpData->srcPort >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[startIndex + 1] = (MSD_U8)(pUdpData->srcPort & (MSD_U16)0xff);
	tcamData->frameOctet[startIndex + 2] = (MSD_U8)((MSD_U16)(pUdpData->destPort >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[startIndex + 3] = (MSD_U8)(pUdpData->destPort & (MSD_U16)0xff);
	tcamData->frameOctet[startIndex + 4] = (MSD_U8)((MSD_U16)(pUdpData->length >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[startIndex + 5] = (MSD_U8)(pUdpData->length & (MSD_U16)0xff);
	tcamData->frameOctet[startIndex + 6] = (MSD_U8)((MSD_U16)(pUdpData->chkSum >> 8) & (MSD_U16)0xff);
	tcamData->frameOctet[startIndex + 7] = (MSD_U8)(pUdpData->chkSum & (MSD_U16)0xff);

	tcamData->frameOctetMask[startIndex + 0] = (MSD_U8)((MSD_U16)(pUdpMask->srcPort >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[startIndex + 1] = (MSD_U8)(pUdpMask->srcPort & (MSD_U16)0xff);
	tcamData->frameOctetMask[startIndex + 2] = (MSD_U8)((MSD_U16)(pUdpMask->destPort >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[startIndex + 3] = (MSD_U8)(pUdpMask->destPort & (MSD_U16)0xff);
	tcamData->frameOctetMask[startIndex + 4] = (MSD_U8)((MSD_U16)(pUdpMask->length >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[startIndex + 5] = (MSD_U8)(pUdpMask->length & (MSD_U16)0xff);
	tcamData->frameOctetMask[startIndex + 6] = (MSD_U8)((MSD_U16)(pUdpMask->chkSum >> 8) & (MSD_U16)0xff);
	tcamData->frameOctetMask[startIndex + 7] = (MSD_U8)(pUdpMask->chkSum & (MSD_U16)0xff);
}
static void mappingActionToTcam(BONSAI_MSD_TCAM_DATA *tcamData, const BONSAI_MSD_TCAM_ENT_ACT *actionPtr)
{
    tcamData->interrupt = actionPtr->interrupt;
    tcamData->IncTcamCtr = actionPtr->IncTcamCtr;
    tcamData->vidOverride = actionPtr->vidOverride;
    tcamData->vidData = actionPtr->vidData;
    tcamData->qpriOverride = actionPtr->qpriOverride;
    tcamData->qpriData = actionPtr->qpriData;
    tcamData->fpriOverride = actionPtr->fpriOverride;
    tcamData->fpriData = actionPtr->fpriData;
    tcamData->dpvSF = actionPtr->dpvSF;
    tcamData->dpvData = actionPtr->dpvData;
    tcamData->dpvMode = actionPtr->dpvMode;
    tcamData->colorMode = actionPtr->colorMode;
    tcamData->vtuPageOverride = actionPtr->vtuPageOverride;
    tcamData->vtuPage = actionPtr->vtuPage;
    tcamData->unKnownFilter = actionPtr->unKnownFilter;
    tcamData->egActPoint = actionPtr->egActPoint;
    tcamData->ldBalanceOverride = actionPtr->ldBalanceOverride;
    tcamData->ldBalanceData = actionPtr->ldBalanceData;
	tcamData->tcamTunnel = 0; /* reserved */
	tcamData->ipMulticast = actionPtr->ipMulticast;
	tcamData->ip2me = actionPtr->ip2me;
	tcamData->routeEntry = actionPtr->routeEntry;
    tcamData->DSCPOverride = actionPtr->DSCPOverride;
    tcamData->DSCP = actionPtr->DSCP;
    tcamData->factionOverride = actionPtr->factionOverride;
    tcamData->factionData = actionPtr->factionData;
	tcamData->tcamCtr = actionPtr->tcamCtr;
	tcamData->flowMeterEn = actionPtr->flowMeterEn;
	tcamData->flowMeterId = actionPtr->flowMeterId;
	tcamData->streamFilterEn = actionPtr->streamFilterEn;
	tcamData->streamFilterId = actionPtr->streamFilterId;
}
static MSD_STATUS Bonsai_setTcamExtensionReg(const MSD_QD_DEV *dev, MSD_U8 block, MSD_U8 port)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16  tmpData = 0;

	tmpData = (MSD_U16)((MSD_U16)(((MSD_U16)block & (MSD_U16)0xF) << 12) | (port & (MSD_U16)0x1F));

	retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)1, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Bonsai_setTcamExtensionReg returned: %s.\n", msdDisplayStatus(retVal)));
	}

	return retVal;
}