/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Amethyst_msdPIRL.c
*
* DESCRIPTION:
*       API definitions for PIRL Resources
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <amethyst/include/api/Amethyst_msdPIRL.h>
#include <amethyst/include/api/Amethyst_msdApiInternal.h>
#include <amethyst/include/driver/Amethyst_msdHwAccess.h>
#include <amethyst/include/driver/Amethyst_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/*
 * Typedef: enum AMETHYST_MSD_PIRL_OPERATION
 *
 * Description: Defines the PIRL (Port Ingress Rate Limit) Operation type
 *
 * Fields:
 *      Amethyst_PIRL_INIT_ALL_RESOURCE - Initialize all resources to the inital state
 *      Amethyst_PIRL_INIT_RESOURCE     - Initialize selected resources to the inital state
 *      Amethyst_PIRL_WRITE_RESOURCE    - Write to the selected resource/register
 *      Amethyst_PIRL_READ_RESOURCE     - Read from the selected resource/register
 */
typedef enum
{
	Amethyst_PIRL_READ_RESOURCE          = 0x0,
    Amethyst_PIRL_INIT_ALL_RESOURCE      = 0x1,
    Amethyst_PIRL_INIT_RESOURCE          = 0x2,
    Amethyst_PIRL_WRITE_RESOURCE         = 0x3
} AMETHYST_MSD_PIRL_OPERATION;


/*
 *  typedef: struct AMETHYST_MSD_PIRL_OP_DATA
 *
 *  Description: data required by PIRL Operation
 *
 *  Fields:
 *      irlUnit   - Ingress Rate Limit Unit that defines one of IRL resources.
 *      irlReg    - Ingress Rate Limit Register.
 *      irlData   - Ingress Rate Limit Data.
 */
typedef struct
{
    MSD_U32    irlUnit;
    MSD_U32    irlReg;
    MSD_U32    irlData;
} AMETHYST_MSD_PIRL_OP_DATA;

/****************************************************************************/
/* STATS operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Amethyst_pirlOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    AMETHYST_MSD_PIRL_OPERATION   pirlOp,
    IN    MSD_U32              irlPort,
    IN    MSD_U32              irlRes,
    IN    MSD_U32              irlReg,
    INOUT AMETHYST_MSD_PIRL_OP_DATA     *opData
);

/*******************************************************************************
* Amethyst_gpirlInitialize
*
* DESCRIPTION:
*       This routine initializes all PIRL Resources for all ports.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_gpirlInitialize
(
    IN  MSD_QD_DEV              *dev
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    AMETHYST_MSD_PIRL_OPERATION    op;

    MSD_DBG_INFO(("Amethyst_gpirlInitialize Called.\n"));

    op = Amethyst_PIRL_INIT_ALL_RESOURCE;

    retVal = Amethyst_pirlOperationPerform(dev, op, 0, 0, 0, NULL);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_pirlOperationPerform Amethyst_PIRL_INIT_ALL_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gpirlInitialize Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gpirlInitResource
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource for a particular port.
*
* INPUTS:
*       irlPort	- tarAmethyst_get port
*		irlRes - resource unit to be accessed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_gpirlInitResource
(
    IN    MSD_QD_DEV            *dev,
	IN    MSD_LPORT             irlPort,
    IN    MSD_U32               irlRes
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    AMETHYST_MSD_PIRL_OPERATION    op;
    AMETHYST_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Amethyst_gpirlInitResource Called.\n"));

    op = Amethyst_PIRL_INIT_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
		return MSD_BAD_PARAM;
	}


	if(irlRes > 7)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}
	
    retVal = Amethyst_pirlOperationPerform(dev, op, port, irlRes, 0, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_pirlOperationPerform Amethyst_PIRL_INIT_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gpirlInitResource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gpirlCustomSetup_sr2c
*
* DESCRIPTION:
*       This function Calculate CBS/EBS/BI/BRFGrn/BRFYel/Delta according to user 
*       specific tarAmethyst_get rate, tarAmethyst_get burst size and countMode.
*
* INPUTS:
*       tAmethyst_gtRate	- tarAmethyst_get rate(units is fps when countMode = AMETHYST_MSD_PIRL_COUNT_FRAME,
*				  otherwise units is bps)
*       tAmethyst_gtBstSize	- tarAmethyst_get burst size(units is Byte)
*       customSetup - Constant and countMode in customSetup as input parameter
*
* OUTPUTS:
*       customSetup - customer setup including CBS/EBS/BI/BRFGrn/BRFYel/Delta/countMode
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		We have the following contrain in calculate the final parameters:
*			CBS >= BktIncrement * tAmethyst_gtBstSize
*
*******************************************************************************/
MSD_STATUS Amethyst_gpirlCustomSetup_sr2c
(
    IN  MSD_U32	   	tAmethyst_gtRate,
    IN  MSD_U32	   	tAmethyst_gtBstSize,
	INOUT AMETHYST_MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
)
{
#if defined(__KERNEL__) && defined(LINUX)
    return MSD_NOT_SUPPORTED;
#else
    MSD_U32 IRL_constant = 500000000;
    MSD_U16 bktIncrement = 0x1FFF;
    MSD_U16 bktRateFactorGrn = 0;
    MSD_U32 cbsLimit = 0xFFFFFF;

    AMETHYST_MSD_PIRL_CUSTOM_RATE_LIMIT Setup;
    MSD_DOUBLE	   	Rate = 0.0;
    MSD_DOUBLE delta = 1.0, deltaMin = 1.0, mode = 1.0;
    MSD_32 j;
    MSD_DOUBLE tgRate = (MSD_DOUBLE)(tAmethyst_gtRate * 1000.0);

    MSD_DBG_INFO(("Amethyst_gpirlCustomSetup Called.\n"));

    if (tAmethyst_gtBstSize < 1600 || tAmethyst_gtRate == 0)
    {
        MSD_DBG_ERROR(("Bad tgtBstSize Or tgtRate, tgtBstSize: %u, tgtRate: %u.\n", tAmethyst_gtBstSize, tAmethyst_gtRate));
        return MSD_BAD_PARAM;
    }

    msdMemSet(&Setup, 0, sizeof(AMETHYST_MSD_PIRL_CUSTOM_RATE_LIMIT));

    if (customSetup->countMode == AMETHYST_MSD_PIRL_COUNT_FRAME)
        IRL_constant /= 8;

    for (j = 1; j<bktIncrement; j++)
    {
        if (((MSD_U32)((MSD_DOUBLE)cbsLimit / j + 0.5))<tAmethyst_gtBstSize + 1)
            continue;
        if ((MSD_U32)(tgRate / IRL_constant*j*mode + 0.5)<0xFFFF)
            bktRateFactorGrn = (MSD_U16)(tgRate / IRL_constant*j*mode + 0.5);
        else
            continue;

        Rate = (MSD_DOUBLE)(IRL_constant / mode*((MSD_DOUBLE)bktRateFactorGrn / j));
        delta = (MSD_DOUBLE)(tgRate - Rate) / tgRate;
        if (delta < 0)
            delta = 0 - delta;

        if (deltaMin > delta)
        {
            deltaMin = delta;
            Setup.bktIncrement = j;
            Setup.bktRateFactorGrn = bktRateFactorGrn;
            Setup.cbsLimit = j * tAmethyst_gtBstSize;

            if (delta == 0)
            {
                break;
            }
        }
    }

    if (deltaMin == 1.0)
    {
        MSD_DBG_ERROR(("Failed (Amethyst_gpirlCustomSetup tune failed).\n"));
        return MSD_FAIL;
    }

    MSD_DBG_INFO(("delta((tgRate - rate)/tgRate) = %f", deltaMin));

    customSetup->isValid = MSD_TRUE;
    customSetup->cbsLimit = Setup.cbsLimit;
    customSetup->bktIncrement = Setup.bktIncrement;
    customSetup->bktRateFactorGrn = Setup.bktRateFactorGrn;

    customSetup->bktRateFactorYel = 0;
    customSetup->ebsLimit = 0;

    customSetup->Delta = deltaMin;

    MSD_DBG_INFO(("Amethyst_gpirlCustomSetup Exit.\n"));
    return MSD_OK;
#endif
}

/*******************************************************************************
* Amethyst_gpirlReadResource
*
* DESCRIPTION:
*       This function read Resource bucket parameter from the given resource of port
*
* INPUTS:
*       irlPort - tarAmethyst_get logical port
*		irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       pirlData - IRL Resource data
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Amethyst_gpirlReadResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    OUT AMETHYST_MSD_PIRL_DATA    *pirlData
)
{
    MSD_STATUS       retVal;         /* Functions return value */
    MSD_U16          data[9];		/* temporary Data storage */
    AMETHYST_MSD_PIRL_OPERATION    op;
    AMETHYST_MSD_PIRL_OP_DATA      opData;
	MSD_U8			port;
    MSD_32               i;

    MSD_DBG_INFO(("Amethyst_gpirlReadResource Called.\n"));

    op = Amethyst_PIRL_READ_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
        return MSD_BAD_PARAM;
	}

	if(irlRes > 7)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}

	msdMemSet((void*)(pirlData), 0, sizeof(AMETHYST_MSD_PIRL_DATA));

    for(i=0; i<9; i++)
    {
        opData.irlData = 0;

        retVal = Amethyst_pirlOperationPerform(dev, op, port, irlRes, i, &opData);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Amethyst_pirlOperationPerform Amethyst_PIRL_READ_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }

        data[i] = (MSD_U16)opData.irlData;
    }
	
    pirlData->bktTypeMask = (data[0]) & 0xFFFF;
	pirlData->tcamFlows = ((data[2]>>13) & 0x1)?MSD_TRUE:MSD_FALSE;
	pirlData->priAndPt = ((data[1]>>9) & 0x1)?MSD_TRUE:MSD_FALSE;
	pirlData->useFPri = ((data[1]>>8) & 0x1)?MSD_TRUE:MSD_FALSE;
	pirlData->priSelect = (MSD_U8)(data[1] & 0xff);

	pirlData->colorAware = ((data[1] >> 13) & 0x1)?MSD_TRUE:MSD_FALSE;

	pirlData->accountGrnOverflow = ((data[1] >> 12) & 0x1)?MSD_TRUE:MSD_FALSE;
    pirlData->accountQConf = ((data[1] >> 11) & 0x1)?MSD_TRUE:MSD_FALSE;
    pirlData->accountFiltered = ((data[1] >> 10) & 0x1)?MSD_TRUE:MSD_FALSE;

    pirlData->samplingMode = ((data[1] >> 14) & 0x1)?MSD_TRUE:MSD_FALSE;
	
    pirlData->actionMode = ((data[8] >> 11) & 0x1)?Amethyst_PIRL_ACTION_FC:Amethyst_PIRL_ACTION_DROP;
    pirlData->fcMode = ((data[8] >> 12) & 0x1)?AMETHYST_MSD_PIRL_FC_DEASSERT_EBS_LIMIT:AMETHYST_MSD_PIRL_FC_DEASSERT_EMPTY;
	pirlData->fcPriority = (MSD_U8)((data[8] >> 13) & 0x7);

	pirlData->customSetup.ebsLimit = ((data[8] & 0xFF) << 16) | (data[7] & 0xFFFF);
    pirlData->customSetup.cbsLimit = ((data[5] & 0xFF) << 16) | (data[4] & 0xFFFF);
    pirlData->customSetup.bktIncrement = data[2] & 0x1FFF;
    pirlData->customSetup.bktRateFactorGrn = data[3] & 0xFFFF;
	pirlData->customSetup.bktRateFactorYel = data[6] & 0xFFFF;
    pirlData->customSetup.countMode = (data[2]>>14) & 3;
	pirlData->customSetup.isValid = MSD_TRUE;

    MSD_DBG_INFO(("Amethyst_gpirlReadResource Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gpirlWriteResource
*
* DESCRIPTION:
*       This function writes Resource bucket parameter to the given resource of port
*
* INPUTS:
*       irlPort	- tarAmethyst_get port
*		irlRes  - resource unit to be accessed
*       pirlData - IRL Resource data
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
MSD_STATUS Amethyst_gpirlWriteResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    IN  AMETHYST_MSD_PIRL_DATA    *pirlData
)
{
    MSD_STATUS       retVal;            /* Functions return value */
    MSD_U16          data[9];     /* temporary Data storage */
    AMETHYST_MSD_PIRL_OPERATION    op;
    AMETHYST_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;
    int                i;

    MSD_DBG_INFO(("Amethyst_gpirlWriteResource Called.\n"));

    op = Amethyst_PIRL_WRITE_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
        return MSD_BAD_PARAM;
	}

	if(irlRes > 7)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}

    /* reg0 data */
    data[0] = (MSD_U16)(pirlData->bktTypeMask & 0xFFFF);        /* Bit[15:0] : Traffic Type   */

    /* reg1 data */
	data[1] = (MSD_U16)((((MSD_U16)pirlData->samplingMode?1:0) << 14) |	/* Bit[14] : sampling Mode */
				(((MSD_U16)pirlData->colorAware?1:0) << 13) |			/* Bit[13] : Color Aware */
				(((MSD_U16)pirlData->accountGrnOverflow?1:0) << 12) |    /* Bit[12] : AcctForGrnOvflow */
				(((MSD_U16)pirlData->accountQConf?1:0) << 11) |		/* Bit[11] : AcctForQConf */
				(((MSD_U16)pirlData->accountFiltered?1:0) << 10) |   /* Bit[10] : AcctForFiltered */
				(((MSD_U16)pirlData->priAndPt?1:0) << 9) |			/* Bit[9] : Priority And Packet Type */
				(((MSD_U16)pirlData->useFPri?1:0) << 8) |			/* Bit[8] : Frame Priority vs Queue Priority */
				((MSD_U16)(pirlData->priSelect&0xFF))); 					/* Bit[7:0] : Priority Select */

    /* reg2 data */
	data[2] = (MSD_U16)((((MSD_U16)pirlData->customSetup.countMode)<<14) |
				(((MSD_U16)pirlData->tcamFlows?1:0)<<13) |
				((MSD_U16)((pirlData->customSetup.bktIncrement)&0x1FFF)));

    /* reg3 data */
	data[3] = (MSD_U16)(pirlData->customSetup.bktRateFactorGrn & 0xFFFF);    /* Bit[15:0] : Bucket Rate Factor Green */

    /* reg4 data */
    data[4] = (MSD_U16)(pirlData->customSetup.cbsLimit & 0xFFFF);   /* Bit[15:0] : CBS Limit[15:0] */

    /* reg5 data */
    data[5] = (MSD_U16)((pirlData->customSetup.cbsLimit >> 16) & 0xFF);   /* Bit[7:0] : CBS Limit[23:16] */
	
    /* reg6 data */
	data[6] = (MSD_U16)(pirlData->customSetup.bktRateFactorYel & 0xFFFF);		/* Bit[15:0] : Bucket Rate Factor Yellow */

	/* reg7 data */
	data[7] = (MSD_U16)(pirlData->customSetup.ebsLimit & 0xFFFF);	/* Bit[15:0] : EBS Limit[15:0] */

	/* reg8 data */
	data[8] = (MSD_U16)(((pirlData->fcPriority&0x7) << 13) |
				((pirlData->fcMode?1:0) << 12) |
				((pirlData->actionMode?1:0) << 11) |
				((pirlData->customSetup.ebsLimit >> 16) & 0xFF));	

    for(i=0; i<9; i++)
    {
        opData.irlData = data[i];

        retVal = Amethyst_pirlOperationPerform(dev, op, port, irlRes, i, &opData);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Amethyst_pirlOperationPerform Amethyst_PIRL_WRITE_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
    }

    MSD_DBG_INFO(("Amethyst_gpirlWriteResource Exit.\n"));
    return MSD_OK;  
}

/*******************************************************************************
* Amethyst_gpirlGetResReg
*
* DESCRIPTION:
*       This routine read general register value from the given resource of the port.
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarAmethyst_get resource 
*       regOffset - register address
*
* OUTPUTS:
*        data	- register value
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
MSD_STATUS Amethyst_gpirlGetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT	 irlPort,
	IN  MSD_U32		 irlRes,
    IN  MSD_U32 	regOffset,
    OUT MSD_U16		 *data
)
{
	MSD_STATUS       retVal;            /* Functions return value */
    AMETHYST_MSD_PIRL_OPERATION    op;
    AMETHYST_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Amethyst_gpirlGetResReg Called.\n"));

	if (NULL == data)
	{
		MSD_DBG_ERROR(("Input param MSD_U16 data is NULL. \n"));
		return MSD_BAD_PARAM;
	}

    op = Amethyst_PIRL_READ_RESOURCE;

	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
		return MSD_BAD_PARAM;
	}

	if(irlRes > 7)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}
	opData.irlData = 0;

	retVal = Amethyst_pirlOperationPerform(dev, op, port, irlRes, regOffset, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_pirlOperationPerform Amethyst_PIRL_READ_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	*data = (MSD_U16)opData.irlData;

    MSD_DBG_INFO(("Amethyst_gpirlGetResReg Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gpirlSetResReg
*
* DESCRIPTION:
*       This routine set general register value to the given resource of the port..
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarAmethyst_get resource 
*       regOffset - register address
*       data	- register value
*
* OUTPUTS:
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
MSD_STATUS Amethyst_gpirlSetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT	 irlPort,
	IN  MSD_U32		 irlRes,
    IN  MSD_U32 		 regOffset,
    IN  MSD_U16		 data
)
{
	MSD_STATUS       retVal;            /* Functions return value */
    AMETHYST_MSD_PIRL_OPERATION    op;
    AMETHYST_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Amethyst_gpirlSetResReg Called.\n"));

    op = Amethyst_PIRL_WRITE_RESOURCE;

	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)irlPort));
	    return MSD_BAD_PARAM;
	}

	if(irlRes > 7)
	{
		MSD_DBG_ERROR(("Bad irlRes: %u.\n", (unsigned int)irlRes));
        return MSD_BAD_PARAM;
	}
	opData.irlData = data;

	retVal = Amethyst_pirlOperationPerform(dev, op, port, irlRes, regOffset, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_pirlOperationPerform Amethyst_PIRL_WRITE_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gpirlSetResReg Exit.\n"));
    return MSD_OK;  
}

/*******************************************************************************
* statsOperationPerform
*
* DESCRIPTION:
*       This function accesses Ingress Rate Command Register and Data Register.
*
* INPUTS:
*       pirlOp       - The stats operation bits to be written into the stats
*                     operation register.
*       irlPort      - port number
*       irlRes       - resource unit to be accessed
*       irlReg       - IRL register address to be accessed
*       counter      - counter to be read if it's read operation
*
* OUTPUTS:
*       pirlData   - points to the data storage where the MIB counter will be saved.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*		None.
*
*******************************************************************************/
static MSD_STATUS Amethyst_pirlOperationPerform_MultiChip
(
    IN    MSD_QD_DEV           *dev,
    IN    AMETHYST_MSD_PIRL_OPERATION   pirlOp,
    IN    MSD_U32              irlPort,
    IN    MSD_U32              irlRes,
    IN    MSD_U32              irlReg,
    INOUT AMETHYST_MSD_PIRL_OP_DATA     *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U16          data;     /* temporary Data storage */

    msdSemTake(dev->devNum, dev->pirlRegsSem, OS_WAIT_FOREVER);

    /* Wait until the pirl in ready. */
    data = 1;
    while (data == 1)
    {
        retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_IGR_RATE_COMMAND_MULTICHIP, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    /* Set the PIRL Operation register */
    switch (pirlOp)
    {
    case Amethyst_PIRL_INIT_ALL_RESOURCE:
        data = (1 << 15) | (Amethyst_PIRL_INIT_ALL_RESOURCE << 13);
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_IGR_RATE_COMMAND_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
        break;
    case Amethyst_PIRL_INIT_RESOURCE:
        data = (MSD_U16)((1 << 15) | (Amethyst_PIRL_INIT_RESOURCE << 13) | (irlPort & 0x1f) << 8 |
            ((irlRes & 0x7) << 5));
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_IGR_RATE_COMMAND_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
        break;

    case Amethyst_PIRL_WRITE_RESOURCE:
        data = (MSD_U16)(opData->irlData);
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_IGR_RATE_DATA_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }

        data = (MSD_U16)((1 << 15) | (Amethyst_PIRL_WRITE_RESOURCE << 13) | (irlPort & 0x1f) << 8 |
            ((irlRes & 0x7) << 5) | (irlReg & 0xF));
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_IGR_RATE_COMMAND_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
        break;

    case Amethyst_PIRL_READ_RESOURCE:
        data = (MSD_U16)((1 << 15) | (Amethyst_PIRL_READ_RESOURCE << 13) | (irlPort & 0x1f) << 8 |
            ((irlRes & 0x7) << 5) | (irlReg & 0xF));
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_IGR_RATE_COMMAND_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }

        data = 1;
        while (data == 1)
        {
            retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_IGR_RATE_COMMAND_MULTICHIP, 15, 1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
        }

        retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_IGR_RATE_DATA_MULTICHIP, &data);
        opData->irlData = (MSD_U32)data;
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
        msdSemGive(dev->devNum, dev->pirlRegsSem);
        return retVal;

    default:

        msdSemGive(dev->devNum, dev->pirlRegsSem);
        return MSD_FAIL;
    }

    /* Wait until the pirl in ready. */

    data = 1;
    while (data == 1)
    {
        retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_IGR_RATE_COMMAND_MULTICHIP, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    msdSemGive(dev->devNum, dev->pirlRegsSem);
    return retVal;
}

static MSD_STATUS Amethyst_pirlOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    AMETHYST_MSD_PIRL_OPERATION   pirlOp,
    IN    MSD_U32              irlPort,
    IN    MSD_U32              irlRes,
    IN    MSD_U32              irlReg,
    INOUT AMETHYST_MSD_PIRL_OP_DATA     *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U16          data;     /* temporary Data storage */

    if (IS_SMI_MULTICHIP_SUPPORTED(dev))
    {
        return Amethyst_pirlOperationPerform_MultiChip(dev, pirlOp, irlPort, irlRes, irlReg, opData);
    }

    msdSemTake(dev->devNum, dev->pirlRegsSem,OS_WAIT_FOREVER);

    /* Wait until the pirl in ready. */
    data = 1;
    while(data == 1)
    {
		retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_COMMAND, 15, 1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    /* Set the PIRL Operation register */
    switch (pirlOp)
    {
        case Amethyst_PIRL_INIT_ALL_RESOURCE:
            data = (1 << 15) | (Amethyst_PIRL_INIT_ALL_RESOURCE << 13);
			retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;
        case Amethyst_PIRL_INIT_RESOURCE:
            data = (MSD_U16)((1 << 15) | (Amethyst_PIRL_INIT_RESOURCE << 13) | (irlPort&0x1f)<<8 | 
                    ((irlRes&0x7)<< 5));
			retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;

        case Amethyst_PIRL_WRITE_RESOURCE:
            data = (MSD_U16)(opData->irlData);
			retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_DATA, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }

            data = (MSD_U16)((1 << 15) | (Amethyst_PIRL_WRITE_RESOURCE << 13) | (irlPort&0x1f)<<8 | 
                    ((irlRes&0x7) << 5) | (irlReg & 0xF));
			retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;

        case Amethyst_PIRL_READ_RESOURCE:
            data = (MSD_U16)((1 << 15) | (Amethyst_PIRL_READ_RESOURCE << 13) | (irlPort&0x1f)<<8 | 
                    ((irlRes&0x7) << 5) | (irlReg & 0xF));
			retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }

            data = 1;
            while(data == 1)
            {
				retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_COMMAND, 15, 1, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->pirlRegsSem);
                    return retVal;
                }
            }

			retVal = msdGetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_DATA, &data);
            opData->irlData = (MSD_U32)data;
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;

        default:
            
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return MSD_FAIL;
    }

    /* Wait until the pirl in ready. */

    data = 1;
    while(data == 1)
    {
		retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_IGR_RATE_COMMAND, 15, 1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    msdSemGive(dev->devNum, dev->pirlRegsSem);
    return retVal;
}
