/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* Oak_msdHwAccess.h
*
* DESCRIPTION:
*       Functions declarations for HW accessing PHY, Serdes and Switch registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Oak_msdHwAccess_H
#define Oak_msdHwAccess_H

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* SMI PHY Registers indirect R/W functions.                                         */
/****************************************************************************/

/*****************************************************************************
* msdSetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly write internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
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
MSD_STATUS Oak_msdSetSMIPhyReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    regAddr,
    IN MSD_U16   data
);

/*****************************************************************************
* msdGetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly read internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register data to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_msdGetSMIPhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    OUT MSD_U16  *data
);

/*****************************************************************************
* msdSetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly write internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
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
MSD_STATUS Oak_msdSetSMIC45PhyReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    phyAddr,
    IN MSD_U16   regAddr,
    IN MSD_U16   data
);

/*****************************************************************************
* msdGetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly read internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address to read.
*
* OUTPUTS:
*       data    - The storage where register data to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_msdGetSMIC45PhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8     devAddr,
    IN  MSD_U8     phyAddr,
    IN  MSD_U16    regAddr,
    OUT MSD_U16    *data
);

/*****************************************************************************
* msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly write internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*		data		- register date to be written.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

MSD_STATUS Oak_msdSetSMIPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN MSD_U16   data
);

/*****************************************************************************
* msdGetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly read internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data       - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_msdGetSMIPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    OUT  MSD_U16   *data
);

MSD_STATUS Oak_msdWritePagedPhyReg
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U16   data
);

MSD_STATUS Oak_msdReadPagedPhyReg
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    OUT MSD_U16   *data
);

MSD_STATUS Oak_msdSetPagedPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   data
);

MSD_STATUS Oak_msdGetPagedPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   *data
);

MSD_STATUS Oak_msdSetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U16  regAddr,
    IN MSD_U16 data
);

MSD_STATUS Oak_msdGetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U16  regAddr,
    OUT MSD_U16 *data
);

MSD_STATUS Oak_msdPhyReset
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U8        portNum,
	IN  MSD_U8		 pageNum,
    IN  MSD_U16       u16Data
);

/****************************************************************************/
/* Directly register access via Mulit Chip mode                             */
/****************************************************************************/
MSD_STATUS Oak_msdDirectSetMultiChipReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8  regAddr,
    IN  MSD_U16 data
);
MSD_STATUS Oak_msdDirectGetMultiChipReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8  regAddr,
    OUT MSD_U16 *data
);
MSD_STATUS Oak_msdDirectSetMultiChipRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   data
);
MSD_STATUS Oak_msdDirectGetMultiChipRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    OUT MSD_U16   *data
);

MSD_STATUS Oak_msdDirectSetMultiChipExtendedReg
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  regAddr,
IN  MSD_U32 data
);
MSD_STATUS Oak_msdDirectGetMultiChipExtendedReg
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  regAddr,
OUT MSD_U32 *data
);
MSD_STATUS Oak_msdDirectSetMultiChipExtendedRegField
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8    regAddr,
IN  MSD_U8    fieldOffset,
IN  MSD_U8    fieldLength,
IN  MSD_U32   data
);
MSD_STATUS Oak_msdDirectGetMultiChipExtendedRegField
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8    regAddr,
IN  MSD_U8    fieldOffset,
IN  MSD_U8    fieldLength,
OUT MSD_U32   *data
);

/********************Below is external PHY register access*******************/

/*****************************************************************************
* msdSetSMIExtPhyReg
*
* DESCRIPTION:
*       This function indirectly write external PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
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
MSD_STATUS Oak_msdSetSMIExtPhyReg
(
IN MSD_QD_DEV *dev,
IN MSD_U8    devAddr,
IN MSD_U8    regAddr,
IN MSD_U16   data
);

/*****************************************************************************
* msdGetSMIExtPhyReg
*
* DESCRIPTION:
*       This function indirectly read external PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register data to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_msdGetSMIExtPhyReg
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8    devAddr,
IN  MSD_U8    regAddr,
OUT MSD_U16  *data
);

MSD_STATUS Oak_msdWritePagedExtPhyReg
(
IN MSD_QD_DEV *dev,
IN  MSD_U8    portNum,
IN  MSD_U8    pageNum,
IN  MSD_U8    regAddr,
IN  MSD_U16   data
);

MSD_STATUS Oak_msdReadPagedExtPhyReg
(
IN MSD_QD_DEV *dev,
IN  MSD_U8    portNum,
IN  MSD_U8    pageNum,
IN  MSD_U8    regAddr,
OUT MSD_U16   *data
);

MSD_STATUS Oak_msdSetSMIExtPhyXMDIOReg
(
IN MSD_QD_DEV *dev,
IN MSD_U8  portNum,
IN MSD_U8  devAddr,
IN MSD_U16  regAddr,
IN MSD_U16 data
);

MSD_STATUS Oak_msdGetSMIExtPhyXMDIOReg
(
IN MSD_QD_DEV *dev,
IN MSD_U8  portNum,
IN MSD_U8  devAddr,
IN MSD_U16  regAddr,
OUT MSD_U16 *data
);

#ifdef __cplusplus
}
#endif
#endif /* __Oak_msdHwAccess_h */