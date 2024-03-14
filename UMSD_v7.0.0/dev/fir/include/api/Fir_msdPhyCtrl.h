/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Fir_msdPhyCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PhyCtrl functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Fir_msdPhyCtrl_h
#define Fir_msdPhyCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Phy Control Types			 			                        */
/****************************************************************************/

/*
* typedef: enum FIR_MSD_PHY_AUTO_MODE
*
* Description: Enumeration of Autonegotiation mode.
*    Auto for both speed and duplex.
*    Auto for speed only and Full duplex.
*    Auto for speed only and Half duplex. (1000Mbps is not supported)
*    Auto for duplex only and speed 1000Mbps.
*    Auto for duplex only and speed 100Mbps.
*    Auto for duplex only and speed 10Mbps.
*    1000Mbps Full duplex.
*    100Mbps Full duplex.
*    100Mbps Half duplex.
*    10Mbps Full duplex.
*    10Mbps Half duplex.
*/

typedef enum
{
    FIR_SPEED_AUTO_DUPLEX_AUTO,
    FIR_SPEED_1000_DUPLEX_AUTO,
    FIR_SPEED_100_DUPLEX_AUTO,
    FIR_SPEED_10_DUPLEX_AUTO,
    FIR_SPEED_AUTO_DUPLEX_FULL,
    FIR_SPEED_AUTO_DUPLEX_HALF,
    FIR_SPEED_1000_DUPLEX_FULL,
    FIR_SPEED_1000_DUPLEX_HALF,
    FIR_SPEED_100_DUPLEX_FULL,
    FIR_SPEED_100_DUPLEX_HALF,
    FIR_SPEED_10_DUPLEX_FULL,
    FIR_SPEED_10_DUPLEX_HALF
}FIR_MSD_PHY_AUTO_MODE;

/*
* typedef: enum FIR_MSD_PHY_SPEED
*
* Description: Enumeration of Phy Speed
*
* Enumerations:
*    FIR_PHY_SPEED_10_MBPS   - 10Mbps
*    FIR_PHY_SPEED_100_MBPS    - 100Mbps
*    FIR_PHY_SPEED_1000_MBPS - 1000Mbps
*/
typedef enum
{
    FIR_PHY_SPEED_10_MBPS,
    FIR_PHY_SPEED_100_MBPS,
    FIR_PHY_SPEED_1000_MBPS
} FIR_MSD_PHY_SPEED;

/****************************************************************************/
/* Exported Phy Control Functions		 			                        */
/****************************************************************************/


/*******************************************************************************
* Fir_serdesReset
*
* DESCRIPTION:
*       This routine preforms internal serdes reset.
*       different devcie have different register location for the reset bit
*
* INPUTS:
*       devNum  - physical device number
* 		portNum - The logical PHY port number
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
*       Fir: Port0, Port9 and Port10
*                 Port operation register, Device4 Register0xF002
*                 10G-BaseR PCS Control, Devcie4Register0x2000
*                 1000BaseX Control register, Devcie4Register0x1000
*
*
*******************************************************************************/
MSD_STATUS Fir_serdesReset
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port
);


#ifdef __cplusplus
}
#endif

#endif /* Fir_msdPhyCtrl_h */
