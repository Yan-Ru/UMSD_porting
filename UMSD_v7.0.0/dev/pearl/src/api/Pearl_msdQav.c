/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Pearl_msdQav.c
*
* DESCRIPTION:
*       API definitions for Qav
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <pearl/include/api/Pearl_msdQav.h>
#include <pearl/include/api/Pearl_msdApiInternal.h>
#include <pearl/include/driver/Pearl_msdHwAccess.h>
#include <pearl/include/driver/Pearl_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

typedef enum
{
	MSD_AVB_BLOCK_PTP = 0x0,
	MSD_AVB_BLOCK_AVB_POLICY = 0x1,
	MSD_AVB_BLOCK_QAV = 0x2,
	MSD_AVB_BLOCK_QBV = 0x3
} MSD_AVB_BLOCK;

/*
* Typedef: enum MSD_PTP_OPERATION
*
* Description: Defines the PTP (Precise Time Protocol) Operation type
*
* Fields:
*      PTP_WRITE_DATA             - Write data to the PTP register
*      PTP_READ_DATA            - Read data from PTP register
*      PTP_READ_MULTIPLE_DATA    - Read multiple data from PTP register
*      PTP_READ_TIMESTAMP_DATA    - Read timestamp data from PTP register
*                    valid bit will be reset after read
*/
typedef enum
{
	PTP_WRITE_DATA = 0x3,
	PTP_READ_DATA = 0x4,
	PTP_READ_MULTIPLE_DATA = 0x6,
	PTP_READ_TIMESTAMP_DATA = 0x8,
} MSD_PTP_OPERATION;

/*
*  typedef: struct MSD_PTP_OP_DATA
*
*  Description: data required by PTP Operation
*
*  Fields:
*      ptpPort        - physical port of the device
*      ptpBlock    - AVB Block
*      ptpAddr     - register address
*      ptpData     - data for ptp register.
*      ptpMultiData- used for multiple read operation.
*      nData         - number of data to be read on multiple read operation.
*/
typedef struct
{
	MSD_U32    ptpPort;
	MSD_U32    ptpBlock;
	MSD_U32    ptpAddr;
	MSD_U32    ptpData;
	MSD_U32    ptpMultiData[MAX_PTP_CONSECUTIVE_READ];
	MSD_U32    nData;
} MSD_PTP_OP_DATA;


/******************************************************************************
* ptpOperationPerform
*
* DESCRIPTION:
*       This function accesses PTP Command Register and Data Register.
*
* INPUTS:
*       ptpOp      - The stats operation bits to be written into the stats
*                    operation register.
*
* OUTPUTS:
*       ptpData    - points to the data storage that the operation requires.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
******************************************************************************/
static MSD_STATUS ptpOperationPerform
(
IN    MSD_QD_DEV             *dev,
IN    MSD_PTP_OPERATION      ptpOp,
INOUT MSD_PTP_OP_DATA        *opData
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U32             i;
	MSD_U16          ptpOperation;
	MSD_U16 data;

	msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);

	/* Wait until the ptp in ready. */
	data = 1;
	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
			PEARL_QD_REG_AVB_COMMAND, 15, 1, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}
	}
	/* Set the PTP Operation register */
	switch (ptpOp)
	{
	case PTP_WRITE_DATA:
	{
						   data = (MSD_U16)opData->ptpData;
						   retVal = msdSetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR, PEARL_QD_REG_AVB_DATA, data);
						   if (retVal != MSD_OK)
						   {
							   msdSemGive(dev->devNum, dev->ptpRegsSem);
							   return retVal;
						   }

						   ptpOperation = PTP_WRITE_DATA;
						   ptpOperation &= 7;
						   ptpOperation <<= 12;
						   data = (MSD_U16)((1 << 15) | (ptpOperation) |
							   (opData->ptpPort << 8) |
							   (opData->ptpBlock << 5) |
							   (opData->ptpAddr & 0x1F));
						   retVal = msdSetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
							   PEARL_QD_REG_AVB_COMMAND, data);
						   if (retVal != MSD_OK)
						   {
							   msdSemGive(dev->devNum, dev->ptpRegsSem);
							   return retVal;
						   }
	}
		break;

	case PTP_READ_DATA:
	{
						  ptpOperation = PTP_READ_DATA;
						  ptpOperation &= 7;
						  ptpOperation <<= 12;
						  data = (MSD_U16)((1 << 15) | (ptpOperation) |
							  (opData->ptpPort << 8) |
							  (opData->ptpBlock << 5) |
							  (opData->ptpAddr & 0x1F));
						  retVal = msdSetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
							  PEARL_QD_REG_AVB_COMMAND, data);
						  if (retVal != MSD_OK)
						  {
							  msdSemGive(dev->devNum, dev->ptpRegsSem);
							  return retVal;
						  }

						  data = 1;
						  while (data == 1)
						  {
							  retVal = msdGetAnyRegField(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
								  PEARL_QD_REG_AVB_COMMAND, 15, 1, &data);
							  if (retVal != MSD_OK)
							  {
								  msdSemGive(dev->devNum, dev->ptpRegsSem);
								  return retVal;
							  }
						  }

						  retVal = msdGetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR, PEARL_QD_REG_AVB_DATA, &data);
						  if (retVal != MSD_OK)
						  {
							  msdSemGive(dev->devNum, dev->ptpRegsSem);
							  return retVal;
						  }
						  opData->ptpData = (MSD_U32)data;
	}
		break;

	case PTP_READ_MULTIPLE_DATA:
	{
								   ptpOperation = PTP_READ_MULTIPLE_DATA;
								   ptpOperation &= 7;
								   ptpOperation <<= 12;
								   data = (MSD_U16)((1 << 15) | (ptpOperation) |
									   (opData->ptpPort << 8) |
									   (opData->ptpBlock << 5) |
									   (opData->ptpAddr & 0x1F));
								   retVal = msdSetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR, PEARL_QD_REG_AVB_COMMAND, data);
								   if (retVal != MSD_OK)
								   {
									   msdSemGive(dev->devNum, dev->ptpRegsSem);
									   return retVal;
								   }

								   data = 1;
								   while (data == 1)
								   {
									   retVal = msdGetAnyRegField(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
										   PEARL_QD_REG_AVB_COMMAND, 15, 1, &data);
									   if (retVal != MSD_OK)
									   {
										   msdSemGive(dev->devNum, dev->ptpRegsSem);
										   return retVal;
									   }
								   }

								   for (i = 0; i<opData->nData; i++)
								   {
									   retVal = msdGetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
										   PEARL_QD_REG_AVB_DATA, &data);
									   if (retVal != MSD_OK)
									   {
										   msdSemGive(dev->devNum, dev->ptpRegsSem);
										   return retVal;
									   }
									   opData->ptpMultiData[i] = (MSD_U32)data;
								   }
	}
		break;

	case PTP_READ_TIMESTAMP_DATA:
	{
									ptpOperation = PTP_READ_MULTIPLE_DATA;
									ptpOperation &= 7;
									ptpOperation <<= 12;
									data = (MSD_U16)((1 << 15) | (ptpOperation) |
										(opData->ptpPort << 8) |
										(opData->ptpBlock << 5) |
										(opData->ptpAddr & 0x1F));
									retVal = msdSetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR, PEARL_QD_REG_AVB_COMMAND, data);
									if (retVal != MSD_OK)
									{
										msdSemGive(dev->devNum, dev->ptpRegsSem);
										return retVal;
									}

									data = 1;
									while (data == 1)
									{
										retVal = msdGetAnyRegField(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
											PEARL_QD_REG_AVB_COMMAND, 15, 1, &data);
										if (retVal != MSD_OK)
										{
											msdSemGive(dev->devNum, dev->ptpRegsSem);
											return retVal;
										}
									}

									retVal = msdGetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR, PEARL_QD_REG_AVB_DATA, &data);
									if (retVal != MSD_OK)
									{
										msdSemGive(dev->devNum, dev->ptpRegsSem);
										return retVal;
									}
									opData->ptpMultiData[0] = (MSD_U32)data;

									if (!(data & 0x1))
									{
										/* valid bit is not set */
										msdSemGive(dev->devNum, dev->ptpRegsSem);
										return retVal;
									}

									for (i = 1; i<opData->nData; i++)
									{
										retVal = msdGetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
											PEARL_QD_REG_AVB_DATA, &data);
										opData->ptpMultiData[i] = (MSD_U32)data;
										if (retVal != MSD_OK)
										{
											msdSemGive(dev->devNum, dev->ptpRegsSem);
											return retVal;
										}
									}

									retVal = msdSetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR, PEARL_QD_REG_AVB_DATA, 0);
									if (retVal != MSD_OK)
									{
										msdSemGive(dev->devNum, dev->ptpRegsSem);
										return retVal;
									}

									ptpOperation = PTP_WRITE_DATA;
									ptpOperation &= 7;
									ptpOperation <<= 12;
									data = (MSD_U16)((1 << 15) | (ptpOperation) |
										(opData->ptpPort << 8) |
										(opData->ptpBlock << 5) |
										(opData->ptpAddr & 0x1F));
									retVal = msdSetAnyReg(dev->devNum, PEARL_GLOBAL2_DEV_ADDR, PEARL_QD_REG_AVB_COMMAND, data);
									if (retVal != MSD_OK)
									{
										msdSemGive(dev->devNum, dev->ptpRegsSem);
										return retVal;
									}
	}
		break;

	default:
	{
			   msdSemGive(dev->devNum, dev->ptpRegsSem);
			   return MSD_FAIL;
	}
	}

	data = 1;
	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, PEARL_GLOBAL2_DEV_ADDR,
			PEARL_QD_REG_AVB_COMMAND, 15, 1, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}
	}
	msdSemGive(dev->devNum, dev->ptpRegsSem);
	return retVal;
}

/******************************************************************************
* Pearl_gqavSetPortQpriXRate
*
* DESCRIPTION:
*        This routine set Priority Queue 2-3 rate on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue - 2 - 3
*        rate - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Pearl_gqavSetPortQpriXRate
(
IN  MSD_QD_DEV     *dev,
IN    MSD_LPORT    port,
IN  MSD_U8        queue,
IN  MSD_U32        rate
)
{
	MSD_STATUS           retVal;
	MSD_PTP_OPERATION    op;
	MSD_PTP_OP_DATA        opData;
	MSD_U32            hwPort;

	MSD_DBG_INFO(("Pearl_gqavSetPortQpriXRate Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
	hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	if (queue >= PEARL_MAX_NUM_OF_QUEUES)
	{
		MSD_DBG_ERROR(("Failed (Bad queue).\n"));
		return MSD_BAD_PARAM;
	}
	if (rate % 32 != 0)
	{
		MSD_DBG_ERROR(("Failed (Bad rate).\n"));
		return MSD_BAD_PARAM;
	}

	opData.ptpBlock = MSD_AVB_BLOCK_QAV;    /* QAV register space */
	opData.ptpAddr = queue * 2;

	opData.ptpPort = hwPort;

	op = PTP_WRITE_DATA;
	opData.ptpData = (rate / 32);

	if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
	{
		MSD_DBG_ERROR(("OperationPerform Pearl_gqavSetPortQpriXRate returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Pearl_gqavSetPortQpriXRate Exit.\n"));
	return MSD_OK;
}
/******************************************************************************
* Pearl_gqavGetPortQpriXRate
*
* DESCRIPTION:
*        This routine get Priority Queue 2-3 rate Bytes on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 2 - 3
*
* OUTPUTS:
*        rate - number of credits in bytes .
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Pearl_gqavGetPortQpriXRate
(
IN  MSD_QD_DEV     *dev,
IN    MSD_LPORT    port,
IN  MSD_U8        queue,
OUT MSD_U32        *rate
)
{
	MSD_STATUS           retVal;
	MSD_PTP_OPERATION    op;
	MSD_PTP_OP_DATA        opData;
	MSD_U32            hwPort;

	MSD_DBG_INFO(("Pearl_gqavGetPortQpriXRate Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
	hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	if (queue >= PEARL_MAX_NUM_OF_QUEUES)
	{
		MSD_DBG_ERROR(("Failed (Bad queue).\n"));
		return MSD_BAD_PARAM;
	}

	opData.ptpBlock = MSD_AVB_BLOCK_QAV;    /* QAV register space */

	opData.ptpAddr = queue * 2;
	opData.ptpPort = hwPort;

	op = PTP_READ_DATA;

	if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
	{
		MSD_DBG_ERROR(("OperationPerform Pearl_gqavGetPortQpriXRate returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	*rate = opData.ptpData * 32;

	MSD_DBG_INFO(("Pearl_gqavGetPortQpriXRate Exit.\n"));
	return MSD_OK;
}
/******************************************************************************
* Pearl_gqavSetPortQpriXHiLimit
*
* DESCRIPTION:
*        This routine set Priority Queue 2-3 HiLimit on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue - 2 - 3
*        hiLimit - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Pearl_gqavSetPortQpriXHiLimit
(
IN  MSD_QD_DEV     *dev,
IN    MSD_LPORT    port,
IN  MSD_U8        queue,
IN  MSD_U16        hiLimit
)
{
	MSD_STATUS           retVal;
	MSD_PTP_OPERATION    op;
	MSD_PTP_OP_DATA        opData;
	MSD_U32            hwPort;

	MSD_DBG_INFO(("Pearl_gqavSetPortQpriXHiLimit Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	if (queue >= PEARL_MAX_NUM_OF_QUEUES)
	{
		MSD_DBG_ERROR(("Failed (Bad queue).\n"));
		return MSD_BAD_PARAM;
	}
	if (hiLimit > 0x7fff)
	{
		MSD_DBG_ERROR(("Failed (Bad hiLimit).\n"));
		return MSD_BAD_PARAM;
	}

	opData.ptpBlock = MSD_AVB_BLOCK_QAV;    /* QAV register space */
	opData.ptpAddr = queue * 2 + 1;

	opData.ptpPort = hwPort;

	op = PTP_WRITE_DATA;
	opData.ptpData = hiLimit & 0x7fff;

	if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
	{
		MSD_DBG_ERROR(("OperationPerform Pearl_gqavSetPortQpriXHiLimit returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Pearl_gqavSetPortQpriXHiLimit Exit.\n"));
	return MSD_OK;
}
/******************************************************************************
* Pearl_gqavGetPortQpriXHiLimit
*
* DESCRIPTION:
*        This routine get Priority Queue 2-3 HiLimit Bytes on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 2 - 3
*
* OUTPUTS:
*        hiLimit - number of credits in bytes .
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Pearl_gqavGetPortQpriXHiLimit
(
IN  MSD_QD_DEV     *dev,
IN    MSD_LPORT    port,
IN  MSD_U8        queue,
OUT MSD_U16        *hiLimit
)
{
	MSD_STATUS           retVal;
	MSD_PTP_OPERATION    op;
	MSD_PTP_OP_DATA        opData;
	MSD_U32            hwPort;

	MSD_DBG_INFO(("Pearl_gqavGetPortQpriXHiLimit Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
	hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	if (queue >= PEARL_MAX_NUM_OF_QUEUES)
	{
		MSD_DBG_ERROR(("Failed (Bad queue).\n"));
		return MSD_BAD_PARAM;
	}

	opData.ptpBlock = MSD_AVB_BLOCK_QAV;    /* QAV register space */

	opData.ptpAddr = queue * 2 + 1;
	opData.ptpPort = hwPort;

	op = PTP_READ_DATA;

	if ((retVal = ptpOperationPerform(dev, op, &opData)) != MSD_OK)
	{
		MSD_DBG_ERROR(("OperationPerform Pearl_gqavGetPortQpriXHiLimit returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	*hiLimit = (MSD_U16)opData.ptpData & 0x7fff;

	MSD_DBG_INFO(("Pearl_gqavGetPortQpriXHiLimit Exit.\n"));
	return MSD_OK;
}
