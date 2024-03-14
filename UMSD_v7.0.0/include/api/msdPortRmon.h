/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdPortRmon.h
*
* DESCRIPTION:
*       API definitions for MIBs.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdPortRmon_h
#define msdPortRmon_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdStatsFlushAll
*
* DESCRIPTION:
*       Flushs all RMON counters for all ports.
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
MSD_STATUS msdStatsFlushAll
(
    IN  MSD_U8  devNum
);

/*******************************************************************************
* msdStatsFlushPort
*
* DESCRIPTION:
*       Flush all RMON counters for a given port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
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
MSD_STATUS msdStatsFlushPort
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum
);

/*******************************************************************************
* msdStatsPortCounterGet
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*       counter - the counter which will be read
*
* OUTPUTS:
*       statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdStatsPortCounterGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_STATS_COUNTERS  counter,
    OUT MSD_U32  *statsData
);

/*******************************************************************************
* msdStatsPortAllCountersGet
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS msdStatsPortAllCountersGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_STATS_COUNTER_SET  *statsCounterSet
);

/*******************************************************************************
* msdStatsHistogramModeGet
*
* DESCRIPTION:
*       This routine gets the Histogram Counters Mode.
*
* INPUTS:
*       devNum  - physical device number
*       None.
*
* OUTPUTS:
*       mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY,
*                    and MSD_COUNT_RX_TX)
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
MSD_STATUS msdStatsHistogramModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_HISTOGRAM_MODE  *mode
);

/*******************************************************************************
* msdStatsHistogramModeSet
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*       devNum  - physical device number
*       mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY,
*                    and MSD_COUNT_RX_TX)
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
*        None.
*
*******************************************************************************/
MSD_STATUS msdStatsHistogramModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_HISTOGRAM_MODE  mode
);

/*******************************************************************************
* msdPortMibDump
*
* DESCRIPTION:
*       Dumps MIB from the specified port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
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
MSD_STATUS msdPortMibDump
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum
);
#ifdef __cplusplus
}
#endif

#endif /* __msdPortRmon_h */