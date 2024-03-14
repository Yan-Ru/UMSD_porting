/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdTCAMRCComp.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell TCAM range check comparator functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdTCAMRCComp_h
#define msdTCAMRCComp_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdTcamRCCompAdd
*
* DESCRIPTION:
*       This routine configures a Range Check Compatator for the specified port.
*
* INPUTS:
*       devNum			- physical device number
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
MSD_STATUS msdTcamRCCompAdd
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum,
IN  MSD_TCAM_RC_COMP_DATA *tcamRCCompData
);
/*******************************************************************************
* msdTcamRCCompAllDelete
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
MSD_STATUS msdTcamRCCompAllDelete
(
IN  MSD_U8  devNum
);
/*******************************************************************************
* msdTcamRCCompFlushPort
*
* DESCRIPTION:
*       This routine is to flush a single Range Checker entry.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum			- Logical port number to set
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
MSD_STATUS msdTcamRCCompFlushPort
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum
);
/*******************************************************************************
* msdTcamRCCompRead
*
* DESCRIPTION:
*       This routine reads a Range Check Compatator for the specified port.
*
* INPUTS:
*       devNum			- physical device number
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
MSD_STATUS msdTcamRCCompRead
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum,
OUT MSD_TCAM_RC_COMP_DATA *tcamRCCompData
);

#ifdef __cplusplus
}
#endif

#endif /* __msdTCAMRCComp_h */
