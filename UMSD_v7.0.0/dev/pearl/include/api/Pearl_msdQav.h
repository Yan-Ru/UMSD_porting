/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/******************************************************************************
* Pearl_msdQav.h
*
* DESCRIPTION:
*       API definitions for Qav feature
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/

#ifndef __Pearl_msdQav_h
#define __Pearl_msdQav_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PTP_CONSECUTIVE_READ    4
/******************************************************************************
* gqavSetPortQpriXRate
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
    IN  MSD_LPORT      port,
    IN  MSD_U8         queue,
    IN  MSD_U32        rate
);

/******************************************************************************
* gqavGetPortQpriXRate
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
    IN  MSD_LPORT      port,
    IN  MSD_U8         queue,
    OUT MSD_U32        *rate
);

/******************************************************************************
* gqavSetPortQpriXHiLimit
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
    IN  MSD_LPORT      port,
    IN  MSD_U8         queue,
    IN  MSD_U16        hiLimit
);

/******************************************************************************
* gqavSetPortQpriXHiLimit
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
MSD_STATUS Pearl_gqavGetPortQpriXHiLimit
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT      port,
    IN  MSD_U8         queue,
    OUT MSD_U16        *hiLimit
);

#ifdef __cplusplus
}
#endif

#endif /* __Pearl_msdQav_h */
