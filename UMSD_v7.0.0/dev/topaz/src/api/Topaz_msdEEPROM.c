/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Topaz_msdEEPROM.c
*
* DESCRIPTION:
*       API definitions for Marvell EEPROM related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <topaz/include/api/Topaz_msdEEPROM.h>
#include <topaz/include/api/Topaz_msdApiInternal.h>
#include <topaz/include/driver/Topaz_msdHwAccess.h>
#include <topaz/include/driver/Topaz_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* internal EEPROM related function declaration.                            */
/****************************************************************************/
static MSD_STATUS Topaz_waitBusyBitSelfClear
(
IN	MSD_QD_DEV	*dev
);
static MSD_STATUS Topaz_EEPROM_Write
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U16	data
);
static MSD_STATUS Topaz_EEPROM_Read
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
OUT	MSD_U16	*data
);

/*******************************************************************************
* Topaz_EEPROMWrite
*
* DESCRIPTION:
*       This routine is used to write one byte to EEPROM
*
*
* INPUTS:
*      addr - EEPROM address
*      data - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_EEPROMWrite
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
IN  MSD_U16  data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Topaz_EEPROMWrite Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Topaz_EEPROM_Write(dev, addr, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_EEPROMWrite returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Topaz_EEPROMWrite Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Topaz_EEPROMRead
*
* DESCRIPTION:
*       This routine is used to read one byte from EEPROM
*
*
* INPUTS:
*      addr - EEPROM address
*
* OUTPUTS:
*      data - EEPROM Data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_EEPROMRead
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
OUT MSD_U16  *data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Topaz_EEPROMRead Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Topaz_EEPROM_Read(dev, addr, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Topaz_EEPROMRead returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Topaz_EEPROMRead Exit.\n"));
	return retVal;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Topaz_waitBusyBitSelfClear
(
IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 data = 1;

	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, 20, 15, 1, &data);
		if (retVal != MSD_OK)
			return MSD_FAIL;
	}

	return MSD_OK;
}
static MSD_STATUS Topaz_EEPROM_Write
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U16	data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15;
	
	retVal = Topaz_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg15 = addr;
	retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, 0x15, reg15);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	retVal = Topaz_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg14 = 0x8000 | (3 << 12) | (1 << 10) | (data & 0xff);

	retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, 0x14, reg14);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	retVal = Topaz_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	return retVal;
}
static MSD_STATUS Topaz_EEPROM_Read
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
OUT MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15, tmpData;
	
	reg15 = addr;
	retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, 0x15, reg15);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	retVal = Topaz_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg14 = 0x8000 | (4 << 12);

	retVal = msdSetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, 0x14, reg14);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}
	retVal = Topaz_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	retVal = msdGetAnyReg(dev->devNum, TOPAZ_GLOBAL2_DEV_ADDR, 0x14, &tmpData);
	*data = tmpData & 0xFF;
	
	return retVal;

}