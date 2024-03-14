/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdPIRL.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PIRL functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdPIRL_h
#define msdPIRL_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdIRLReset
*
* DESCRIPTION:
*       This routine initializes all PIRL Resources for all ports.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None
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
MSD_STATUS msdIRLReset
(
    IN  MSD_U8  devNum
);

/*******************************************************************************
* msdIRLInitResourceCounter
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource counters for a particular port.
*
* INPUTS:
*       devNum  - physical device number
*       irlPort - target logical port
*       irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       None
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
MSD_STATUS msdIRLInitResourceCounter
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  irlPort,
    IN  MSD_U32  irlRes
);

/*******************************************************************************
* msdIRLCustomSetup
*
* DESCRIPTION:
*       This function calculates CBS/EBS/BI/BRFGrn/BRFYel/Delta according to user
*       specific target rate, target burst size and countMode.
*
* INPUTS:
*       devNum  - physical device number
*       tgtRate	- target rate(units is fps when countMode = MSD_PIRL_COUNT_FRAME,
*				  otherwise units is kbps)
*       tgtBstSize	- target burst size(units is Byte)
*       countMode - MSD_PIRL_COUNT_MODE
*       customSetup - countMode in customSetup as input parameter
*
* OUTPUTS:
*       customSetup - MSD_PIRL_CUSTOM_RATE_LIMIT
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		We have the following contrain in calculate the final parameters:
*			CBS >= BktIncrement * tgtBstSize
*       for Topaz:
*		    As the input for structure 'MSD_PIRL_CUSTOM_RATE_LIMIT', Constant can be
*           0 or any value from {7812500, 125000000, 62500000, 31250000, 15625000, 
*           3906250, 1953750, 976250}. If Constant is 0, this API will automatically
*           calculate the best IRL Constant which means Delta between target Rate
*           and real Rate is small enough or 0. However, since Constant is Global
*           for all Resource of all ports, user can specify fixed Constant to 
*           gurantee that the Constant is consistent across all existing resources.
*
*******************************************************************************/
MSD_STATUS msdIRLCustomSetup
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  tgtRate,
    IN  MSD_U32  tgtBstSize,
    IN  MSD_PIRL_COUNT_MODE  countMode,
    OUT MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
);

/*******************************************************************************
* msdIRLResourceRead
*
* DESCRIPTION:
*       This function reads Resource bucket parameter from the given resource of port
*
* INPUTS:
*       devNum  - physical device number
*       irlPort - target logical port
*		irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       pirlData - IRL Resource data
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
MSD_STATUS msdIRLResourceRead
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  irlPort,
    IN  MSD_U32  irlRes,
    OUT MSD_PIRL_DATA  *pirlData
);

/*******************************************************************************
* msdIRLResourceWrite
*
* DESCRIPTION:
*       This function writes resource bucket parameter to the given resource of port
*
* INPUTS:
*       devNum  - physical device number
*       irlPort	- target port
*       irlRes  - resource unit to be accessed
*       pirlData - IRL Resource data
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
MSD_STATUS msdIRLResourceWrite
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  irlPort,
    IN  MSD_U32  irlRes,
    IN  MSD_PIRL_DATA  *pirlData
);

/*******************************************************************************
* msdIRLResRegGet
*
* DESCRIPTION:
*       This routine reads general register value from the given resource of the port.
*
* INPUTS:
*       devNum  - physical device number
*       irlPort   - logical port number
*       irlRes - target resource
*       regOffset - register address
*
* OUTPUTS:
*       data	- register value
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
MSD_STATUS msdIRLResRegGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  irlPort,
    IN  MSD_U32  irlRes,
    IN  MSD_U32  regOffset,
    OUT MSD_U16  *data
);

/*******************************************************************************
* msdIRLResRegSet
*
* DESCRIPTION:
*       This routine sets general register value to the given resource of the port..
*
* INPUTS:
*       devNum  - physical device number
*       irlPort   - logical port number
*       irlRes - target resource
*       regOffset - register address
*       data	- register value
*
* OUTPUTS:
*       None
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
MSD_STATUS msdIRLResRegSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  irlPort,
    IN  MSD_U32  irlRes,
    IN  MSD_U32  regOffset,
    IN  MSD_U16  data
);

/*******************************************************************************
* msdIRLAdvConfig
*
* DESCRIPTION:
*       This routine configures ingress rate limit based on packet type or priority
*       the given resource of the port
*
* INPUTS:
*       devNum  - physical device number
*       irlPort   - logical port number
*       irlRes - target resource
*       tgtRate	- target rate(units is fps when countMode = PERIDOT_MSD_PIRL_COUNT_FRAME,
*				  otherwise units is kbps)
*       tgtBstSize	- target burst size(units is Byte)
*       countMode - MSD_PIRL_COUNT_MODE
*       pktTypeOrPri - MSD_PIRL_PKTTYPE_OR_PRI
*
* OUTPUTS:
*       None
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
MSD_STATUS msdIRLAdvConfig
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  irlPort,
    IN  MSD_U32  irlRes,
    IN  MSD_U32  tgtRate,
    IN  MSD_U32  tgtBstSize,
    IN  MSD_PIRL_COUNT_MODE  countMode,
    IN  MSD_PIRL_PKTTYPE_OR_PRI  *pktTypeOrPri
);

/*******************************************************************************
* msdIRLAdvConfigDisable
*
* DESCRIPTION:
*       This routine clears configuration for ingress rate limit to the given resource
*       of the port
*
* INPUTS:
*       devNum  - physical device number
*       irlPort   - logical port number
*       irlRes - target resource
*
* OUTPUTS:
*       None
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
MSD_STATUS msdIRLAdvConfigDisable
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  irlPort,
    IN  MSD_U32  irlRes
);
#ifdef __cplusplus
}
#endif

#endif /* __msdBrgFdb_h */
