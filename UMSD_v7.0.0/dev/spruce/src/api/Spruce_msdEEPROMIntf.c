/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Spruce_msdEEPROMIntf.c
*
* DESCRIPTION:
*       API definitions for Marvell EEPROM related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <spruce/include/api/Spruce_msdEEPROM.h>

/*******************************************************************************
* Spruce_EEPROMWrite
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
MSD_STATUS Spruce_EEPROMWriteIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
IN  MSD_U16  data
)
{
	return Spruce_EEPROMWrite(dev, addr, data);
}
/*******************************************************************************
* Spruce_EEPROMRead
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
MSD_STATUS Spruce_EEPROMReadIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
OUT MSD_U16  *data
)
{
	return Spruce_EEPROMRead(dev, addr, data);
}
/*******************************************************************************
* Spruce_EEPROMChipSelSet
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
MSD_STATUS Spruce_EEPROMChipSelSetIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  chipSelect
)
{
	return Spruce_EEPROMChipSelSet(dev, chipSelect);
}
/*******************************************************************************
* Spruce_EEPROMChipSelGet
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
MSD_STATUS Spruce_EEPROMChipSelGetIntf
(
IN  MSD_QD_DEV *dev,
OUT  MSD_U8  *chipSelect
)
{
	return Spruce_EEPROMChipSelGet(dev, chipSelect);
}