/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Agate_msdPortCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PortCtrl functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Agate_msdPortCtrl_h
#define __Agate_msdPortCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Types			 			                                   */
/****************************************************************************/

/*  typedef: enum AGATE_MSD_8021Q_MODE */
typedef enum
{
	AGATE_MSD_DISABLE = 0,
	AGATE_MSD_FALLBACK,
	AGATE_MSD_CHECK,
	AGATE_MSD_SECURE
} AGATE_MSD_8021Q_MODE;

/*  typedef: enum AGATE_MSD_EGRESS_MODE */
typedef enum
{
	AGATE_MSD_EGRESS_MODE_UNMODIFIED = 0,
	AGATE_MSD_EGRESS_MODE_UNTAGGED,
	AGATE_MSD_EGRESS_MODE_TAGGED
} AGATE_MSD_EGRESS_MODE;

/*
* Typedef: enum AGATE_MSD_MTU_SIZE
*
* Description: Defines Jumbo Frame Size allowed to be tx and rx
*
* Fields:
*      AGATE_MSD_MTU_SIZE_1522 - Rx and Tx frames with max byte of 1522.
*      AGATE_MSD_MTU_SIZE_2048 - Rx and Tx frames with max byte of 2048.
*      AGATE_MSD_MTU_SIZE_10240 - Rx and Tx frames with max byte of 10240.
*
*/
typedef enum
{
	AGATE_MSD_MTU_SIZE_1522 = 0,
	AGATE_MSD_MTU_SIZE_2048,
	AGATE_MSD_MTU_SIZE_10240
} AGATE_MSD_MTU_SIZE;

/* Definition for the Port Duplex Mode */
typedef enum
{
	AGATE_MSD_DUPLEX_HALF,
	AGATE_MSD_DUPLEX_FULL,
	AGATE_MSD_DUPLEX_AUTO
} AGATE_MSD_PORT_FORCE_DUPLEX_MODE;

/* Definition for full duplex flow control mode */
typedef enum
{
	Agate_PORT_FC_TX_RX_ENABLED,
} AGATE_MSD_PORT_FC_MODE;

/* Definition for the forced Port Speed */
typedef enum
{
	Agate_PORT_FORCE_SPEED_10M = 0x0,
	Agate_PORT_FORCE_SPEED_100M,
	Agate_PORT_FORCE_SPEED_200M,
	Agate_PORT_FORCE_SPEED_1000M,
	Agate_PORT_DO_NOT_FORCE_SPEED = 0x7
} AGATE_MSD_PORT_FORCED_SPEED_MODE;

typedef enum
{
	AGATE_PORT_FORCE_LINK_UP,
	AGATE_PORT_FORCE_LINK_DOWN,
	AGATE_PORT_DO_NOT_FORCE_LINK
} AGATE_MSD_PORT_FORCE_LINK_MODE;

/*
*  typedef: enum AGATE_MSD_ELIMIT_MODE
*
*  Description: Enumeration of the port egress rate limit counting mode.
*
*  Enumerations:
*      AGATE_MSD_ELIMIT_FRAME -
*                Count the number of frames
*      AGATE_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes:
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*      AGATE_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*      AGATE_MSD_ELIMIT_LAYER3 -
*                Count all Layer 3 bytes:
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*/
typedef enum
{
	AGATE_MSD_ELIMIT_FRAME = 0,
	AGATE_MSD_ELIMIT_LAYER1,
	AGATE_MSD_ELIMIT_LAYER2,
	AGATE_MSD_ELIMIT_LAYER3
} AGATE_MSD_ELIMIT_MODE;

/*
* Typedef: union MSD_ERATE_TYPE
*
* Description: Egress Rate
*
* Fields:
*      kbRate      - rate in kbps that should used with the AGATE_MSD_ELIMIT_MODE of
*                                AGATE_MSD_ELIMIT_LAYER1,
*                                AGATE_MSD_ELIMIT_LAYER2, or
*                                AGATE_MSD_ELIMIT_LAYER3 (see Agate_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*      fRate         - frame per second that should used with AGATE_MSD_ELIMIT_MODE of
*                                MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
*/
typedef union
{
	MSD_U32            kbRate;
	MSD_U32            fRate;
} AGATE_MSD_ERATE_TYPE;

/* Definition for the Port Speed */
typedef enum
{
	Agate_PORT_SPEED_10_MBPS = 0,
	Agate_PORT_SPEED_100_MBPS = 1,
	Agate_PORT_SPEED_200_MBPS = 2,
	Agate_PORT_SPEED_1000_MBPS = 3,
	Agate_PORT_SPEED_UNKNOWN = 7
} AGATE_MSD_PORT_SPEED;

typedef enum
{
	AGATE_PRI_OVERRIDE_NONE = 0,
	AGATE_PRI_OVERRIDE_FRAME,
	AGATE_PRI_OVERRIDE_QUEUE,
	AGATE_PRI_OVERRIDE_FRAME_QUEUE,
} AGATE_MSD_PRI_OVERRIDE;

typedef enum
{
	AGATE_FRAME_MODE_NORMAL,
	AGATE_FRAME_MODE_DSA,
	AGATE_FRAME_MODE_PROVIDER,
	AGATE_FRAME_MODE_ETHER_TYPE_DSA
}  AGATE_MSD_FRAME_MODE;


#ifndef AGATE_STP_STATE
typedef enum
{
	AGATE_MSD_PORT_DISABLE = 0,
	AGATE_MSD_PORT_BLOCKING,
	AGATE_MSD_PORT_LEARNING,
	AGATE_MSD_PORT_FORWARDING
} AGATE_MSD_PORT_STP_STATE;
#define AGATE_STP_STATE
#endif

typedef struct
{
	MSD_BOOL enable;
	MSD_U8 cutThruQueue;
} AGATE_CUT_THROUGH;
/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* Agate_gvlnSetPortVid
*
* DESCRIPTION:
*       This routine Set the port default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*       vid  - the port vlan id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_gvlnSetPortVid
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_U16       vid
);

/*******************************************************************************
* Agate_gvlnGetPortVid
*
* DESCRIPTION:
*       This routine Get the port default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*
* OUTPUTS:
*       vid  - the port vlan id.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Agate_gvlnGetPortVid
(
IN MSD_QD_DEV *dev,
IN  MSD_LPORT port,
OUT MSD_U16   *vid
);

/*******************************************************************************
* Agate_gprtSetForceDefaultVid
*
* DESCRIPTION:
*       This routine Set the port force default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*       en   - enable force port default vlan id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetForceDefaultVid
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      en
);

/*******************************************************************************
* Agate_gprtGetForceDefaultVid
*
* DESCRIPTION:
*       This routine Get the port force default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*
* OUTPUTS:
*       en  - enable port force default vlan id.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetForceDefaultVid
(
IN MSD_QD_DEV	*dev,
IN  MSD_LPORT	port,
OUT MSD_BOOL	*en
);

/********************************************************************
* Agate_gvlnSetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine sets the IEEE 802.1q mode for this port
*
* INPUTS:
*       port    - logical port number to set.
*       mode     - 802.1q mode for this port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gvlnSetPortVlanDot1qMode
(
IN MSD_QD_DEV        *dev,
IN MSD_LPORT     port,
IN AGATE_MSD_8021Q_MODE    mode
);
MSD_STATUS Agate_gvlnSetPortVlanDot1qModeIntf
(
IN MSD_QD_DEV        *dev,
IN MSD_LPORT     port,
IN MSD_8021Q_MODE    mode
);
/*******************************************************************************
* Agate_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to get.
*
* OUTPUTS:
*       mode     - 802.1q mode for this port
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gvlnGetPortVlanDot1qMode
(
IN MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
OUT AGATE_MSD_8021Q_MODE   *mode
);
MSD_STATUS Agate_gvlnGetPortVlanDot1qModeIntf
(
IN MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
OUT MSD_8021Q_MODE   *mode
);
/*******************************************************************************
* Agate_gprtSetDiscardTagged
*
* DESCRIPTION:
*       When this bit is set to a one, all non-MGMT frames that are processed as
*       Tagged will be discarded as they enter this switch port. Priority only
*       tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to discard tagged frame, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetDiscardTagged
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
MSD_STATUS Agate_gprtSetDiscardTaggedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
/*******************************************************************************
* Agate_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine gets DiscardTagged bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if DiscardTagged bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetDiscardTagged
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
MSD_STATUS Agate_gprtGetDiscardTaggedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
/*******************************************************************************
* Agate_gprtSetDiscardUntagged
*
* DESCRIPTION:
*       When this bit is set to a one, all non-MGMT frames that are processed as
*       Untagged will be discarded as they enter this switch port. Priority only
*       tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to discard untagged frame, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetDiscardUntagged
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
MSD_STATUS Agate_gprtSetDiscardUntaggedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);

/*******************************************************************************
* Agate_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine gets DiscardUntagged bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if DiscardUntagged bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetDiscardUntagged
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
MSD_STATUS Agate_gprtGetDiscardUntaggedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
/*******************************************************************************
* Agate_gprtSetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine set Forward Unknown mode of a switch port.
*       When this mode is set to MSD_TRUE, normal switch operation occurs.
*       When this mode is set to MSD_FALSE, unicast frame with unknown DA addresses
*       will not egress out this port.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA unicast frames
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetUnicastFloodBlock
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT    port,
IN MSD_BOOL        mode
);
MSD_STATUS Agate_gprtSetUnicastFloodBlockIntf
(
IN  MSD_QD_DEV   *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      mode
);
/*******************************************************************************
* Agate_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine gets Forward Unknown mode of a switch port.
*       When this mode is set to MSD_TRUE, normal switch operation occurs.
*       When this mode is set to MSD_FALSE, unicast frame with unknown DA addresses
*       will not egress out this port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA unicast frames
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetUnicastFloodBlock
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
MSD_STATUS Agate_gprtGetUnicastFloodBlockIntf
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
/*******************************************************************************
* Agate_gprtSetMulticastFloodBlock
*
* DESCRIPTION:
*       When this bit is set to a one, normal switch operation will occurs and
*       multicast frames with unknown DA addresses are allowed to egress out this
*       port (assuming the VLAN settings allow the frame to egress this port too).
*       When this bit is cleared to a zero, multicast frames with unknown DA
*       addresses will not egress out this port.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA multicast frames
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetMulticastFloodBlock
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
MSD_STATUS Agate_gprtSetMulticastFloodBlockIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
/*******************************************************************************
* Agate_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine gets DefaultForward bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA multicast frames
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetMulticastFloodBlock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
MSD_STATUS Agate_gprtGetMulticastFloodBlockIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
/*******************************************************************************
* Agate_gprtSetDiscardBCastMode
*
* DESCRIPTION:
*       This routine sets the Discard Broadcast mode.
*       If the mode is enabled, all the broadcast frames to the given port will
*       be discarded.
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetDiscardBCastMode
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);
MSD_STATUS Agate_gprtSetDiscardBCastModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);
/*******************************************************************************
* Agate_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine gets the Discard Broadcast Mode. If the mode is enabled,
*       all the broadcast frames to the given port will be discarded.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetDiscardBCastMode
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);
MSD_STATUS Agate_gprtGetDiscardBCastModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

/*******************************************************************************
* Agate_gprtSetIGMPSnoopEnable
*
* DESCRIPTION:
*       This routine sets the Enable/disable trapping all IPV4 IGMP packets to
*       the CPU,based on their ingress port.
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetIGMPSnoopEnable
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);

/*******************************************************************************
* Agate_gprtGetIGMPSnoopEnable
*
* DESCRIPTION:
*       This routine gets the Enable/disable trapping all IPV4 IGMP packets to
*       the CPU,based on their ingress port.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetIGMPSnoopEnable
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

/*******************************************************************************
* Agate_gprtGetHeaderMode
*
* DESCRIPTION:
*       This routine gets the ingress and egress header mode of a switch
*       port.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetHeaderMode
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

/*******************************************************************************
* Agate_gprtSetHeaderMode
*
* DESCRIPTION:
*       This routine sets the ingress and egress header mode of a switch
*       port. To be used only for port connected to cpu mainly used for layer3
*       32bit alignment.
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetHeaderMode
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);

/*******************************************************************************
* Agate_gprtGetDropOnLock
*
* DESCRIPTION:
*       This routine gets the DropOnLock mode of specified port on specified device
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetDropOnLock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

/*******************************************************************************
* Agate_gprtSetDropOnLock
*
* DESCRIPTION:
*       This routine sets the Drop on Lock. When set to one, Ingress frames
*       will be discarded if their SA field is not in the ATU's address database
*       of specified port on specified device
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetDropOnLock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);

/*******************************************************************************
* Agate_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - AGATE_MSD_MTU_SIZE (1522, 2048, or 10240)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetJumboMode
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  AGATE_MSD_MTU_SIZE   mode
);
MSD_STATUS Agate_gprtSetJumboModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_MTU_SIZE   mode
);
/*******************************************************************************
* Agate_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - AGATE_MSD_MTU_SIZE (1522, 2048, or 10240)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetJumboMode
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT AGATE_MSD_MTU_SIZE   *mode
);
MSD_STATUS Agate_gprtGetJumboModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_MTU_SIZE   *mode
);
/*******************************************************************************
* Agate_gprtSetLearnEnable
*
* DESCRIPTION:
*       This routine enables/disables automatic learning of new source MAC
*       addresses on the given port ingress
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for disable or MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetLearnEnable
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      mode
);
MSD_STATUS Agate_gprtSetLearnEnableIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      mode
);
/*******************************************************************************
* Agate_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine gets LearnDisable setup
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE: Learning disabled on the given port ingress frames,
*                 MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetLearnEnable
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *mode
);
MSD_STATUS Agate_gprtGetLearnEnableIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *mode
);

MSD_STATUS Agate_gprtGetVTUPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Agate_gprtGetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Agate_gprtSetVTUPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS Agate_gprtSetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  mode
);

MSD_STATUS Agate_gprtGetSAPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Agate_gprtGetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Agate_gprtSetSAPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS Agate_gprtSetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  mode
);

MSD_STATUS Agate_gprtGetDAPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Agate_gprtGetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Agate_gprtSetDAPriorityOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS Agate_gprtSetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PRI_OVERRIDE  mode
);


/*******************************************************************************
* Agate_gprtGetMessagePort
*
* DESCRIPTION:
*       This routine gets message port mode for specific port on a specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE: enable Message Port, MSD_FALSE: otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetMessagePort
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL  *en
);

/*******************************************************************************
* Agate_gprtSetMessagePort
*
* DESCRIPTION:
*       This routine Set message port for specific port on a specific device.
*       Learning and learn refresh message frames will be sent out to this port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE: enable Message Port, MSD_FALSE: otherwise
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
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetMessagePort
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL  en
);

/*******************************************************************************
* Agate_gprtGetEtherType
*
* DESCRIPTION:
*       This routine gets the port's special Ether Type. This Ether Type is used
*       for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode
*       API) of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       eType - Ethernet type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16  *eType
);

/*******************************************************************************
* Agate_gprtSetEtherType
*
* DESCRIPTION:
*       This routine sets the port's special Ether Type. This Ether Type is used
*       for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode API)
*       of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       eType - Ethernet type
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
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_U16  eType
);

/*******************************************************************************
* Agate_gprtSetTrunkPort
*
* DESCRIPTION:
*        This function enables/disables and sets the trunk ID.
*
* INPUTS:
*        port - the logical port number.
*        en - MSD_TRUE to make the port be a member of a trunk with the given trunkId.
*             MSD_FALSE, otherwise.
*        trunkId - valid ID is 0 ~ 31
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_BAD_PARAM - if port is invalid nor INVALID_TRUNK_ID
*
* COMMENTS:
*		 None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetTrunkPort
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      en,
IN MSD_U32       trunkId
);

/*******************************************************************************
* Agate_gprtGetTrunkPort
*
* DESCRIPTION:
*       This function returns trunk state of the port.
*       When trunk is disabled, trunkId field won't have valid value.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       en - MSD_TRUE, if the port is a member of a trunk,
*             MSD_FALSE, otherwise.
*       trunkId - 0 ~ 31, valid only if en is MSD_TRUE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetTrunkPort
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
OUT MSD_BOOL     *en,
OUT MSD_U32      *trunkId
);

/*******************************************************************************
* Agate_gprtSetFlowCtrl
*
* DESCRIPTION:
*       This routine enable/disable port flow control and set flow control mode
*          mode - PORT_FC_TX_RX_ENABLED,
*                 PORT_RX_ONLY,
*                 PORT_TX_ONLY,
*                 PORT_PFC_ENABLED
*
* INPUTS:
*       port - the logical port number.
*		en - enable/disable the flow control
*       mode - flow control mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Set ForcedFC=1, FCValue = enable/disable, FCMode
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetFlowCtrl
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_BOOL     en,
IN  AGATE_MSD_PORT_FC_MODE      mode
);
MSD_STATUS Agate_gprtSetFlowCtrlIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_BOOL     en,
IN  MSD_PORT_FC_MODE      mode
);
/*******************************************************************************
* Agate_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine get switch port flow control enable/disable status and return
*       flow control mode
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       en - enable/disable the flow control
*       mode - flow control mode
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetFlowCtrl
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *en,
OUT AGATE_MSD_PORT_FC_MODE      *mode
);
MSD_STATUS Agate_gprtGetFlowCtrlIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *en,
OUT MSD_PORT_FC_MODE      *mode
);

/*******************************************************************************
* Agate_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed.
*
* INPUTS:
*       port - the logical port number.
*       mode - AGATE_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000 or No Speed Force)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetForceSpeed
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN AGATE_MSD_PORT_FORCED_SPEED_MODE  mode
);
MSD_STATUS Agate_gprtSetForceSpeedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_PORT_FORCED_SPEED_MODE  mode
);
/*******************************************************************************
* Agate_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - AGATE_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000 or No Speed Force)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetForceSpeed
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PORT_FORCED_SPEED_MODE   *mode
);
MSD_STATUS Agate_gprtGetForceSpeedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PORT_FORCED_SPEED_MODE   *mode
);
/*******************************************************************************
* Agate_gprtSetEgressMonitorSource
*
* DESCRIPTION:
*       When this bit is cleared to a zero, normal network switching occurs.
*       When this bit is set to a one, any frame that egresses out this port will
*       also be sent to the EgressMonitorDest Port
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to set EgressMonitorSource, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetEgressMonitorSource
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);

/*******************************************************************************
* Agate_gprtGetEgressMonitorSource
*
* DESCRIPTION:
*       This routine gets EgressMonitorSource bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if EgressMonitorSource bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetEgressMonitorSource
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);

/*******************************************************************************
* Agate_gprtSetIngressMonitorSource
*
* DESCRIPTION:
*       When this be is cleared to a zero, normal network switching occurs.
*       When this bit is set to a one, any frame that ingresses in this port will
*       also be sent to the IngressMonitorDest Port
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to set IngressMonitorSource, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetIngressMonitorSource
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);

/*******************************************************************************
* Agate_gprtGetIngressMonitorSource
*
* DESCRIPTION:
*       This routine gets IngressMonitorSource bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if IngressMonitorSource bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetIngressMonitorSource
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);

/*******************************************************************************
* Agate_grcSetEgressRate
*
* DESCRIPTION:
*       This routine sets the port's egress data limit.
*
*
* INPUTS:
*       port      - logical port number.
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the AGATE_MSD_ELIMIT_MODE of
*                                AGATE_MSD_ELIMIT_LAYER1,
*                                AGATE_MSD_ELIMIT_LAYER2, or
*                                AGATE_MSD_ELIMIT_LAYER3 (see Agate_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with AGATE_MSD_ELIMIT_MODE of
*                                MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_grcSetEgressRate
(
IN MSD_QD_DEV       *dev,
IN MSD_LPORT        port,
IN AGATE_MSD_ELIMIT_MODE   mode,
IN MSD_U32   rate
);
MSD_STATUS Agate_grcSetEgressRateIntf
(
IN MSD_QD_DEV       *dev,
IN MSD_LPORT        port,
IN MSD_ELIMIT_MODE   mode,
IN MSD_U32   rate
);
/*******************************************************************************
* Agate_grcGetEgressRate
*
* DESCRIPTION:
*       This routine gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the AGATE_MSD_ELIMIT_MODE of
*                                AGATE_MSD_ELIMIT_LAYER1,
*                                AGATE_MSD_ELIMIT_LAYER2, or
*                                AGATE_MSD_ELIMIT_LAYER3 (see Agate_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with AGATE_MSD_ELIMIT_MODE of
*                                MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_grcGetEgressRate
(
IN MSD_QD_DEV		*dev,
IN  MSD_LPORT		port,
OUT AGATE_MSD_ELIMIT_MODE   *mode,
OUT MSD_U32   *rate
);
MSD_STATUS Agate_grcGetEgressRateIntf
(
IN MSD_QD_DEV		*dev,
IN MSD_LPORT		port,
OUT MSD_ELIMIT_MODE   *mode,
OUT MSD_U32   *rate
);

/*******************************************************************************
* Agate_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            AGATE_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            AGATE_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes:
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            AGATE_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            AGATE_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes:
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - AGATE_MSD_ELIMIT_MODE enum type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_grcSetELimitMode
(
IN  MSD_QD_DEV			*dev,
IN  MSD_LPORT			port,
IN  AGATE_MSD_ELIMIT_MODE      mode
);
MSD_STATUS Agate_grcSetELimitModeIntf
(
IN  MSD_QD_DEV			*dev,
IN  MSD_LPORT			port,
IN  MSD_ELIMIT_MODE      mode
);
/*******************************************************************************
* Agate_grcGetELimitMode
*
* DESCRIPTION:
*       This routine gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            AGATE_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            AGATE_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes:
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            AGATE_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            AGATE_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes:
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - AGATE_MSD_ELIMIT_MODE enum type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_grcGetELimitMode
(
IN  MSD_QD_DEV		*dev,
IN  MSD_LPORT		port,
OUT AGATE_MSD_ELIMIT_MODE  *mode
);
MSD_STATUS Agate_grcGetELimitModeIntf
(
IN  MSD_QD_DEV		*dev,
IN  MSD_LPORT		port,
OUT MSD_ELIMIT_MODE  *mode
);
/*******************************************************************************
* Agate_gprtGetDuplexStatus
*
* DESCRIPTION:
*       This routine retrives the port duplex mode.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for Full-duplex  or MSD_FALSE for Half-duplex
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetDuplex
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
IN AGATE_MSD_PORT_FORCE_DUPLEX_MODE   mode
);
MSD_STATUS Agate_gprtSetDuplexIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
IN MSD_PORT_FORCE_DUPLEX_MODE   mode
);
MSD_STATUS Agate_gprtGetDuplex
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_PORT_FORCE_DUPLEX_MODE   *mode
);
MSD_STATUS Agate_gprtGetDuplexIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_DUPLEX_MODE   *mode
);
MSD_STATUS Agate_gprtGetDuplexStatus
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL   *mode
);
MSD_STATUS Agate_gprtGetDuplexStatusIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL   *mode
);
/*******************************************************************************
* Agate_gprtGetLinkState
*
* DESCRIPTION:
*       This routine retrives the link state.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       state - MSD_TRUE for Up  or MSD_FALSE for Down
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetForceLink
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_PORT_FORCE_LINK_MODE   *mode
);
MSD_STATUS Agate_gprtGetForceLinkIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_LINK_MODE   *mode
);
MSD_STATUS Agate_gprtSetForceLink
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_PORT_FORCE_LINK_MODE  mode
);
MSD_STATUS Agate_gprtSetForceLinkIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_LINK_MODE  mode
);
MSD_STATUS Agate_gprtGetLinkState
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL   *state
);

/*******************************************************************************
* Agate_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port MAC speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       speed - AGATE_MSD_PORT_SPEED type.
*                (Agate_PORT_SPEED_10_MBPS,Agate_PORT_SPEED_100_MBPS, Agate_PORT_SPEED_1000_MBPS,
*                etc.)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetSpeed
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_PORT_SPEED   *speed
);
MSD_STATUS Agate_gprtGetSpeedIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_SPEED   *speed
);

/*******************************************************************************
* Agate_gprtGetVlanPorts
*
* DESCRIPTION:
*       this routine Get port based vlan table of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       memPorts - vlan ports
*       memPortsLen - number of vlan ports
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_LPORT  *memPorts,
OUT MSD_U8  *memPortsLen
);

/*******************************************************************************
* Agate_gprtSetVlanPorts
*
* DESCRIPTION:
*       this routine Set port based vlan table of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       memPorts - vlan ports to set
*       memPortsLen - number of vlan ports
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
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_LPORT  *memPorts,
IN  MSD_U8  memPortsLen
);

/*******************************************************************************
* Agate_gprtGetFrameMode
*
* DESCRIPTION:
*       this routine Get frame mode of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - MSD_FRAME_MODE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_FRAME_MODE  *mode
);
MSD_STATUS Agate_gprtGetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_FRAME_MODE  *mode
);

/*******************************************************************************
* Agate_gprtSetFrameMode
*
* DESCRIPTION:
*       this routine Set frame mode of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - MSD_FRAME_MODE
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
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  AGATE_MSD_FRAME_MODE  mode
);

MSD_STATUS Agate_gprtSetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_FRAME_MODE  mode
);

/*******************************************************************************
* Agate_gprtSetEgressMode
*
* DESCRIPTION:
*       this routine Set egress mode of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - MSD_EGRESS_MODE
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
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetEgressMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  AGATE_MSD_EGRESS_MODE  mode
);

MSD_STATUS Agate_gprtSetEgressModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_EGRESS_MODE  mode
);

/*******************************************************************************
* Agate_gprtGetEgressMode
*
* DESCRIPTION:
*       this routine Get egress mode of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - MSD_EGRESS_MODE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetEgressMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_EGRESS_MODE  *mode
);

MSD_STATUS Agate_gprtGetEgressModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_EGRESS_MODE  *mode
);

/*******************************************************************************
* Agate_gprtGetPortState
*
* DESCRIPTION:
*       this routine Get port state of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - AGATE_PORT_STP_STATE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetPortState
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT AGATE_MSD_PORT_STP_STATE  *mode
);
MSD_STATUS Agate_gprtGetPortStateIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_STP_STATE  *mode
);

/*******************************************************************************
* Agate_gprtSetPortState
*
* DESCRIPTION:
*       this routine Set port state of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - AGATE_PORT_STP_STATE
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
*		None.
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetPortState
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  AGATE_MSD_PORT_STP_STATE  mode
);
MSD_STATUS Agate_gprtSetPortStateIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_PORT_STP_STATE  mode
);

MSD_STATUS Agate_gprtGetCutThrouthEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  AGATE_CUT_THROUGH  *cutThru
);
MSD_STATUS Agate_gprtGetCutThrouthEnableIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_CUT_THROUGH  *cutThru
);

MSD_STATUS Agate_gprtSetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtSetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtSetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
);

MSD_STATUS Agate_gprtGetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
);

MSD_STATUS Agate_gprtSetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
);

MSD_STATUS Agate_gprtGetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
);

MSD_STATUS Agate_gprtGetOutQueueSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8 queue,
OUT MSD_U16  *count
);

MSD_STATUS Agate_gprtSetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtSetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtSetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtSetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtSetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtSetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtSetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Agate_gprtGetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Agate_gprtRegDump
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  port
);
#ifdef __cplusplus
}
#endif

#endif /* __Agate_msdPortCtrl_h */
