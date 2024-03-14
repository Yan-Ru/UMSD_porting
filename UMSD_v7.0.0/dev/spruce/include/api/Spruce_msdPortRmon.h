/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Spruce_msdPortRmon.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell MIBS functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Spruce_msdPortRmon_h
#define Spruce_msdPortRmon_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported MIBS Types			 			                                   */
/****************************************************************************/
/*
 *  typedef: struct MSD_STATS_COUNTERS
 *
 *  Description: MIB counter
 *     
 */
#define SPRUCE_MSD_TYPE_BANK  0x400
#define SPRUCE_MSD_TYPE_BANK2 0x800

typedef enum
{
	/* Bank 0 */
	Spruce_STATS_InGoodOctetsLo = 0,
	Spruce_STATS_InGoodOctetsHi = 1,
	Spruce_STATS_InBadOctets = 2,

	Spruce_STATS_OutFCSErr = 3,
	Spruce_STATS_InUnicasts = 4,
	Spruce_STATS_Deferred = 5,             /* offset 5 */
	Spruce_STATS_InBroadcasts = 6,
	Spruce_STATS_InMulticasts = 7,
	Spruce_STATS_64Octets = 8,
	Spruce_STATS_127Octets = 9,
	Spruce_STATS_255Octets = 0xa,            /* offset 10 */
	Spruce_STATS_511Octets = 0xb,
	Spruce_STATS_1023Octets = 0xc,
	Spruce_STATS_MaxOctets = 0xd,
	Spruce_STATS_OutOctetsLo = 0xe,
	Spruce_STATS_OutOctetsHi = 0xf,
	Spruce_STATS_OutUnicasts = 0x10,          /* offset 16 */
	Spruce_STATS_Excessive = 0x11,
	Spruce_STATS_OutMulticasts = 0x12,
	Spruce_STATS_OutBroadcasts = 0x13,
	Spruce_STATS_Single = 0x14,
	Spruce_STATS_OutPause = 0x15,
	Spruce_STATS_InPause = 0x16,
	Spruce_STATS_Multiple = 0x17,
	Spruce_STATS_InUndersize = 0x18,          /* offset 24 */
	Spruce_STATS_InFragments = 0x19,
	Spruce_STATS_InOversize = 0x1a,
	Spruce_STATS_InJabber = 0x1b,
	Spruce_STATS_InRxErr = 0x1c,
	Spruce_STATS_InFCSErr = 0x1d,
	Spruce_STATS_Collisions = 0x1e,
	Spruce_STATS_Late = 0x1f,                 /* offset 31 */
	/* Bank 1 */
    Spruce_STATS_InDiscards      = SPRUCE_MSD_TYPE_BANK+0x00,
    Spruce_STATS_InFiltered      = SPRUCE_MSD_TYPE_BANK+0x01,
    Spruce_STATS_InAccepted      = SPRUCE_MSD_TYPE_BANK+0x02,
    Spruce_STATS_InBadAccepted   = SPRUCE_MSD_TYPE_BANK+0x03,
    Spruce_STATS_InGoodAvbClassA = SPRUCE_MSD_TYPE_BANK+0x04,
    Spruce_STATS_InGoodAvbClassB = SPRUCE_MSD_TYPE_BANK+0x05,
    Spruce_STATS_InBadAvbClassA  = SPRUCE_MSD_TYPE_BANK+0x06,
    Spruce_STATS_InBadAvbClassB  = SPRUCE_MSD_TYPE_BANK+0x07,
    Spruce_STATS_TCAMCounter0    = SPRUCE_MSD_TYPE_BANK+0x08,
    Spruce_STATS_TCAMCounter1    = SPRUCE_MSD_TYPE_BANK+0x09,
    Spruce_STATS_TCAMCounter2    = SPRUCE_MSD_TYPE_BANK+0x0a,
    Spruce_STATS_TCAMCounter3    = SPRUCE_MSD_TYPE_BANK+0x0b,
    Spruce_STATS_InDroppedAvbA   = SPRUCE_MSD_TYPE_BANK+0x0c,
    Spruce_STATS_InDroppedAvbB   = SPRUCE_MSD_TYPE_BANK+0x0d, 
    Spruce_STATS_InDaUnknown     = SPRUCE_MSD_TYPE_BANK+0x0e,
    Spruce_STATS_InMGMT          = SPRUCE_MSD_TYPE_BANK+0x0f,
    Spruce_STATS_OutQueue0       = SPRUCE_MSD_TYPE_BANK+0x10,
    Spruce_STATS_OutQueue1       = SPRUCE_MSD_TYPE_BANK+0x11,
    Spruce_STATS_OutQueue2       = SPRUCE_MSD_TYPE_BANK+0x12,
    Spruce_STATS_OutQueue3       = SPRUCE_MSD_TYPE_BANK+0x13,
    Spruce_STATS_OutQueue4       = SPRUCE_MSD_TYPE_BANK+0x14,
    Spruce_STATS_OutQueue5       = SPRUCE_MSD_TYPE_BANK+0x15,
    Spruce_STATS_OutQueue6       = SPRUCE_MSD_TYPE_BANK+0x16,
    Spruce_STATS_OutQueue7       = SPRUCE_MSD_TYPE_BANK+0x17,
    Spruce_STATS_OutCutThrough   = SPRUCE_MSD_TYPE_BANK+0x18,
	Spruce_STATS_InBadQbv		  = SPRUCE_MSD_TYPE_BANK+0x19,
    Spruce_STATS_OutOctetsA      = SPRUCE_MSD_TYPE_BANK+0x1a,
    Spruce_STATS_OutOctetsB      = SPRUCE_MSD_TYPE_BANK+0x1b,
    Spruce_STATS_OutYel          = SPRUCE_MSD_TYPE_BANK+0x1c,
    Spruce_STATS_OutDroppedYel   = SPRUCE_MSD_TYPE_BANK+0x1d, 
    Spruce_STATS_OutDiscards     = SPRUCE_MSD_TYPE_BANK+0x1e, 
    Spruce_STATS_OutMGMT         = SPRUCE_MSD_TYPE_BANK+0x1f,

	/* Bank 2 */
	Spruce_STATS_DropEvents      = SPRUCE_MSD_TYPE_BANK2 + 0x00

} SPRUCE_MSD_STATS_COUNTERS;

/*
 *  typedef: struct SPRUCE_MSD_STATS_COUNTER_SET
 *
 *  Description: MIB Counter Set
 *     
 */
typedef struct SPRUCE_MSD_STATS_COUNTER_SET_
{
	/* Bank 0 */
    MSD_U32    InGoodOctetsLo;     /* offset 0 */
    MSD_U32    InGoodOctetsHi;     /* offset 1 */
    MSD_U32    InBadOctets;        /* offset 2 */
    MSD_U32    OutFCSErr;          /* offset 3 */
    MSD_U32    InUnicasts;         /* offset 4 */
    MSD_U32    Deferred;           /* offset 5 */
    MSD_U32    InBroadcasts;       /* offset 6 */
    MSD_U32    InMulticasts;       /* offset 7 */
    /* 
        Histogram Counters : Rx Only, Tx Only, or both Rx and Tx 
        (refer to Histogram Mode) 
    */
    MSD_U32    Octets64;         /* 64 Octets, offset 8 */
    MSD_U32    Octets127;        /* 65 to 127 Octets, offset 9 */
    MSD_U32    Octets255;        /* 128 to 255 Octets, offset 10 */
    MSD_U32    Octets511;        /* 256 to 511 Octets, offset 11 */
    MSD_U32    Octets1023;       /* 512 to 1023 Octets, offset 12 */
    MSD_U32    OctetsMax;        /* 1024 to Max Octets, offset 13 */
    MSD_U32    OutOctetsLo;      /* offset 14 */
    MSD_U32    OutOctetsHi;      /* offset 15 */
    MSD_U32    OutUnicasts;      /* offset 16 */
    MSD_U32    Excessive;        /* offset 17 */
    MSD_U32    OutMulticasts;    /* offset 18 */
    MSD_U32    OutBroadcasts;    /* offset 19 */
    MSD_U32    Single;           /* offset 20 */

    MSD_U32    OutPause;         /* offset 21 */
    MSD_U32    InPause;          /* offset 22 */
    MSD_U32    Multiple;         /* offset 23 */
    MSD_U32    InUndersize;        /* offset 24 */
    MSD_U32    InFragments;        /* offset 25 */
    MSD_U32    InOversize;         /* offset 26 */
    MSD_U32    InJabber;           /* offset 27 */
    MSD_U32    InRxErr;          /* offset 28 */
    MSD_U32    InFCSErr;         /* offset 29 */
    MSD_U32    Collisions;       /* offset 30 */
    MSD_U32    Late;             /* offset 31 */
	/* Bank 1 */
    MSD_U32    InDiscards;       /* offset 0x00 */
    MSD_U32    InFiltered;       /* offset 0x01 */
    MSD_U32    InAccepted;       /* offset 0x02 */
    MSD_U32    InBadAccepted;    /* offset 0x03 */
    MSD_U32    InGoodAvbClassA;  /* offset 0x04 */
    MSD_U32    InGoodAvbClassB;  /* offset 0x05 */
    MSD_U32    InBadAvbClassA ;  /* offset 0x06 */
    MSD_U32    InBadAvbClassB ;  /* offset 0x07 */
    MSD_U32    TCAMCounter0;     /* offset 0x08 */
    MSD_U32    TCAMCounter1;     /* offset 0x09 */
    MSD_U32    TCAMCounter2;     /* offset 0x0a */
    MSD_U32    TCAMCounter3;     /* offset 0x0b */
    MSD_U32    InDroppedAvbA;    /* offset 0x0c */
    MSD_U32    InDroppedAvbB;    /* offset 0x0d */
    MSD_U32    InDaUnknown ;     /* offset 0x0e */
    MSD_U32    InMGMT;           /* offset 0x0f */
    MSD_U32    OutQueue0;        /* offset 0x10 */
    MSD_U32    OutQueue1;        /* offset 0x11 */
    MSD_U32    OutQueue2;        /* offset 0x12 */
    MSD_U32    OutQueue3;        /* offset 0x13 */
    MSD_U32    OutQueue4;        /* offset 0x14 */
    MSD_U32    OutQueue5;        /* offset 0x15 */
    MSD_U32    OutQueue6;        /* offset 0x16 */
    MSD_U32    OutQueue7;        /* offset 0x17 */
    MSD_U32    OutCutThrough;    /* offset 0x18 */
	MSD_U32    InBadQbv;		 /* offset 0x19 */
    MSD_U32    OutOctetsA;       /* offset 0x1a */
    MSD_U32    OutOctetsB;       /* offset 0x1b */
    MSD_U32    OutYel;           /* offset 0x1c */
    MSD_U32    OutDroppedYel;    /* offset 0x1d */
    MSD_U32    OutDiscards;      /* offset 0x1e */
    MSD_U32    OutMGMT;          /* offset 0x1f */

	/* Bank 2 */
	MSD_U32    DropEvents;       /* offset 0x00 */

}SPRUCE_MSD_STATS_COUNTER_SET;

/*
 * typedef: enum MSD_HISTOGRAM_MODE
 *
 * Description: Enumeration of the histogram counters mode.
 *
 * Enumerations:
 *   MSD_COUNT_RX_ONLY - In this mode, Rx Histogram Counters are counted.
 *   MSD_COUNT_TX_ONLY - In this mode, Tx Histogram Counters are counted.
 *   MSD_COUNT_RX_TX   - In this mode, Rx and Tx Histogram Counters are counted.
 */
typedef enum
{
	SPRUCE_MSD_COUNT_RX_ONLY = 1,
	SPRUCE_MSD_COUNT_TX_ONLY,
	SPRUCE_MSD_COUNT_RX_TX
} SPRUCE_MSD_HISTOGRAM_MODE;

/****************************************************************************/
/* Exported MIBS Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Spruce_gstatsFlushAll
*
* DESCRIPTION:
*       Flush All RMON counters for all ports.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gstatsFlushAll
(
    IN MSD_QD_DEV* dev
);

/*******************************************************************************
* Spruce_gstatsFlushPort
*
* DESCRIPTION:
*       Flush All RMON counters for a given port.
*
* INPUTS:
*       port - the logical port number.
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
MSD_STATUS Spruce_gstatsFlushPort
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT    port
);

/*******************************************************************************
* Spruce_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Spruce_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT         port,
	IN  SPRUCE_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
);
MSD_STATUS Spruce_gstatsGetPortCounterIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
IN  MSD_STATS_COUNTERS    counter,
OUT MSD_U32            *statsData
);
/*******************************************************************************
* Spruce_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to Spruce_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS Spruce_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT        port,
	OUT SPRUCE_MSD_STATS_COUNTER_SET    *statsCounterSet
);
MSD_STATUS Spruce_gstatsGetPortAllCntersIntf
(
IN  MSD_QD_DEV               *dev,
IN  MSD_LPORT        port,
OUT MSD_STATS_COUNTER_SET    *statsCounterSet
);
/*******************************************************************************
* Spruce_gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY, 
*                    and MSD_COUNT_RX_TX)
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
MSD_STATUS Spruce_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
	OUT SPRUCE_MSD_HISTOGRAM_MODE    *mode
);
MSD_STATUS Spruce_gstatsGetHistogramModeIntf
(
IN  MSD_QD_DEV   *dev,
OUT MSD_HISTOGRAM_MODE    *mode
);

/*******************************************************************************
* Spruce_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY, 
*                    and MSD_COUNT_RX_TX)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Spruce_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
	IN SPRUCE_MSD_HISTOGRAM_MODE        mode
);
MSD_STATUS Spruce_gstatsSetHistogramModeIntf
(
IN MSD_QD_DEV   *dev,
IN MSD_HISTOGRAM_MODE        mode
);

/*******************************************************************************
* Spruce_gmibDump
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
MSD_STATUS Spruce_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
);

#ifdef __cplusplus
}
#endif

#endif /* __Spruce_msdPortRmon_h */
