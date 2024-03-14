/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Oak_msdEEPROMIntf.c
*
* DESCRIPTION:
*       API definitions for Marvell EEPROM related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <oak/include/api/Oak_msdEEPROM.h>

/*******************************************************************************
* Oak_EEPROMWrite
*
* DESCRIPTION:
*       This routine is used to write one byte EEPROM data
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
MSD_STATUS Oak_EEPROMWriteIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
IN  MSD_U16  data
)
{
	return Oak_EEPROMWrite(dev, addr, data);
}
/*******************************************************************************
* Oak_EEPROMRead
*
* DESCRIPTION:
*       This routine is used to read one byte EEPROM data
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
MSD_STATUS Oak_EEPROMReadIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
OUT MSD_U16  *data
)
{
	return Oak_EEPROMRead(dev, addr, data);
}
/*******************************************************************************
* Oak_EEPROMChipSelSet
*
* DESCRIPTION:
*       This routine is used to select one external EERPOM chip to operation
*
*
* INPUTS:
*      chipSelect - selected EEPROM chip number
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
MSD_STATUS Oak_EEPROMChipSelSetIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  chipSelect
)
{
	return Oak_EEPROMChipSelSet(dev, chipSelect);
}
/*******************************************************************************
* Oak_EEPROMChipSelGet
*
* DESCRIPTION:
*       This routine is used to get which external EERPOM chip is actively been operated on
*
*
* INPUTS:
*      None
*
* OUTPUTS:
*      chipSelect - selected EEPROM chip number
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
MSD_STATUS Oak_EEPROMChipSelGetIntf
(
IN  MSD_QD_DEV *dev,
OUT  MSD_U8  *chipSelect
)
{
	return Oak_EEPROMChipSelGet(dev, chipSelect);
}