/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Topaz__msdPhyCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PhyCtrl functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Topaz__msdPhyCtrl_h
#define __Topaz__msdPhyCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Phy Control Types			 			                        */
/****************************************************************************/

/*
* typedef: enum MSD_PHY_AUTO_MODE
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
    TOPAZ_SPEED_AUTO_DUPLEX_AUTO,
    TOPAZ_SPEED_1000_DUPLEX_AUTO,
    TOPAZ_SPEED_100_DUPLEX_AUTO,
    TOPAZ_SPEED_10_DUPLEX_AUTO,
    TOPAZ_SPEED_AUTO_DUPLEX_FULL,
    TOPAZ_SPEED_AUTO_DUPLEX_HALF,
    TOPAZ_SPEED_1000_DUPLEX_FULL,
    TOPAZ_SPEED_1000_DUPLEX_HALF,
    TOPAZ_SPEED_100_DUPLEX_FULL,
    TOPAZ_SPEED_100_DUPLEX_HALF,
    TOPAZ_SPEED_10_DUPLEX_FULL,
    TOPAZ_SPEED_10_DUPLEX_HALF
}TOPAZ_MSD_PHY_AUTO_MODE;

/*
* typedef: enum MSD_PHY_SPEED
*
* Description: Enumeration of Phy Speed
*
* Enumerations:
*    PHY_SPEED_10_MBPS   - 10Mbps
*    PHY_SPEED_100_MBPS    - 100Mbps
*    PHY_SPEED_1000_MBPS - 1000Mbps
*/
typedef enum
{
    TOPAZ_PHY_SPEED_10_MBPS,
    TOPAZ_PHY_SPEED_100_MBPS,
    TOPAZ_PHY_SPEED_1000_MBPS
} TOPAZ_MSD_PHY_SPEED;

/****************************************************************************/
/* Exported Phy Control Functions		 			                        */
/****************************************************************************/

/*******************************************************************************
* gphyReset
*
* DESCRIPTION:
*       This routine preforms PHY reset.
*       After reset, phy will be in Autonegotiation mode.
*
* INPUTS:
* 		port - The logical port number, unless SERDES device is accessed
*        	   The physical address, if SERDES device is accessed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
* 		data sheet register 0.15 - Reset
* 		data sheet register 0.13 - Speed(LSB)
* 		data sheet register 0.12 - Autonegotiation
* 		data sheet register 0.8  - Duplex Mode
*		data sheet register 0.6  - Speed(MSB)
*
*       If DUT is in power down or loopback mode, port will link down, 
*       in this case, no need to do software reset to restart AN.
*       When port recover from link down, AN will restart automatically.
*
*******************************************************************************/
MSD_STATUS Topaz_gphyReset
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port
);

/*******************************************************************************
* msdSerdesReset
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
*       Topaz:  Port5 (offset 0x15), Device4 Register0x2000
*
*******************************************************************************/
MSD_STATUS Topaz_serdesReset
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port
);

/*******************************************************************************
* gphySetPortLoopback
*
* DESCRIPTION:
*       Enable/Disable Internal Port Loopback. 
*       For 10/100 Fast Ethernet PHY, speed of Loopback is determined as follows:
*         If Auto-Negotiation is enabled, this routine disables Auto-Negotiation and 
*         forces speed to be 10Mbps.
*         If Auto-Negotiation is disabled, the forced speed is used.
*         Disabling Loopback simply clears bit 14 of control register(0.14). Therefore,
*         it is recommended to call gprtSetPortAutoMode for PHY configuration after 
*         Loopback test.
*       For 10/100/1000 Gigagbit Ethernet PHY, speed of Loopback is determined as follows:
*         If Auto-Negotiation is enabled and Link is active, the current speed is used.
*         If Auto-Negotiation is disabled, the forced speed is used.
*         All other cases, default MAC Interface speed is used. Please refer to the data
*         sheet for the information of the default MAC Interface speed.
*   
*
* INPUTS:
*       port - The logical port number, unless SERDES device is accessed
*              The physical address, if SERDES device is accessed
*       enable - If MSD_TRUE, enable loopback mode
*       		   If MSD_FALSE, disable loopback mode
*
* OUTPUTS:
* 		  None.
*
* RETURNS:
* 		MSD_OK - on success
* 		MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
* 		data sheet register 0.14 - Loop_back
*
*******************************************************************************/
MSD_STATUS Topaz_gphySetPortLoopback
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port,
    IN MSD_BOOL   enable
);

/*******************************************************************************
* gphySetPortSpeed
*
* DESCRIPTION:
*       Sets speed for a specific logical port. This function will keep the duplex 
*       mode and loopback mode to the previous value, but disable others, such as 
*       Autonegotiation.
*
* INPUTS:
*       port -  The logical port number, unless SERDES device is accessed
*               The physical address, if SERDES device is accessed
*       speed - port speed.
*               PHY_SPEED_10_MBPS for 10Mbps
*               PHY_SPEED_100_MBPS for 100Mbps
*               PHY_SPEED_1000_MBPS for 1000Mbps
*		
* OUTPUTS:
* 		None.
*
* RETURNS:
* 		MSD_OK - on success
* 		MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
* 		data sheet register 0.13 - Speed Selection (LSB)
* 		data sheet register 0.6  - Speed Selection (MSB)
*
*       If DUT is in power down or loopback mode, port will link down, 
*       in this case, no need to do software reset to force take effect .
*       When port recover from link down, configure will take effect automatically.
*
*******************************************************************************/
MSD_STATUS Topaz_gphySetPortSpeed
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN TOPAZ_MSD_PHY_SPEED speed
);
MSD_STATUS Topaz_gphySetPortSpeedIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN MSD_PHY_SPEED speed
);

/*******************************************************************************
* gprtPortPowerDown
*
* DESCRIPTION:
*        Enable/disable (power down) on specific logical port.
*        Phy configuration remains unchanged after Power down.
*
* INPUTS:
*        port -  The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*        state - MSD_TRUE: power down
*                 MSD_FALSE: normal operation
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*         data sheet register 0.11 - Power Down
*
*******************************************************************************/
MSD_STATUS Topaz_gphyPortPowerDown
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN MSD_BOOL   state
);

/*******************************************************************************
* gphySetPortDuplexMode
*
* DESCRIPTION:
*		Sets duplex mode for a specific logical port. This function will keep 
*		the speed and loopback mode to the previous value, but disable others,
*		such as Autonegotiation.
*
* INPUTS:
*		port -  The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*		dMode    - dulpex mode
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		MSD_OK  - on success
*		MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
* 		data sheet register 0.8 - Duplex Mode
*
*       If DUT is in power down or loopback mode, port will link down, 
*       in this case, no need to do software reset to force take effect .
*       When port recover from link down, configure will take effect automatically.
*
*******************************************************************************/
MSD_STATUS Topaz_gphySetPortDuplexMode
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN MSD_BOOL   dMode
);

/*******************************************************************************
* gphySetPortAutoMode
*
* DESCRIPTION:
*		This routine sets up the port with given Auto Mode.
*		Supported mode is as follows:
*        - Auto for both speed and duplex.
*        - Auto for speed only and Full duplex.
*        - Auto for speed only and Half duplex.
*        - Auto for duplex only and speed 1000Mbps.
*        - Auto for duplex only and speed 100Mbps.
*        - Auto for duplex only and speed 10Mbps.
*        - Speed 1000Mbps and Full duplex.
*        - Speed 1000Mbps and Half duplex.
*        - Speed 100Mbps and Full duplex.
*        - Speed 100Mbps and Half duplex.
*        - Speed 10Mbps and Full duplex.
*        - Speed 10Mbps and Half duplex.
*        
*
* INPUTS:
*		port -	The logical port number, unless SERDES device is accessed
*               The physical address, if SERDES device is accessed
*		mode - 	Auto Mode to be written
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		MSD_OK   - on success
*		MSD_FAIL - on error
*
* COMMENTS:
*		data sheet register 4.6, 4.5 Autonegotiation Advertisement for 10BT
*		data sheet register 4.8, 4.7 Autonegotiation Advertisement for 100BT
*		data sheet register 9.9, 9.8 Autonegotiation Advertisement for 1000BT
*
*       If DUT is in power down or loopback mode, port will link down, 
*       in this case, no need to do software reset to restart AN.
*       When port recover from link down, AN will restart automatically.
*
*******************************************************************************/
MSD_STATUS Topaz_gphySetPortAutoMode
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port,
    IN TOPAZ_MSD_PHY_AUTO_MODE mode
);
MSD_STATUS Topaz_gphySetPortAutoModeIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN MSD_PHY_AUTO_MODE mode
);

MSD_STATUS Topaz_gphySetEEE
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT portNum,
    IN MSD_BOOL en
);

MSD_STATUS Topaz_gphySetFlowControlEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
IN MSD_BOOL en
);

/*******************************************************************************
* Topaz_gphySetIntEnable
*
* DESCRIPTION:
*       This routine enables or disables one PHY Interrupt, When an interrupt occurs,
*       the corresponding bit is set and remains set until register 19 is read. When
*       interrupt enable bits are not set in register 18, interrupt status bits in
*       register 19 are still set when the corresponding interrupt events occur.
*       If a certain interrupt event is not enabled for the INTn pin, it will still
*       be indicated by the corresponding Interrupt status bits if the interrupt event occurs.
*       The unselected events will not cause the INTn pin to be activated.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*       en      - the data of interrupt to enable/disable. any combination of below
*                 MSD_PHY_AUTO_NEG_ERROR
*                 MSD_PHY_SPEED_CHANGED
*                 MSD_PHY_DUPLEX_CHANGED
*                 MSD_PHY_PAGE_RECEIVED
*                 MSD_PHY_AUTO_NEG_COMPLETED
*                 MSD_PHY_LINK_STATUS_CHANGED
*                 MSD_PHY_SYMBOL_ERROR
*                 MSD_PHY_FALSE_CARRIER
*                 MSD_PHY_MDI_CROSSOVER_CHANGED
*                 MSD_PHY_DOWNSHIFT
*                 MSD_PHY_ENERGY_DETECT
*                 MSD_PHY_FLP_EXCHANGE_COMPLETE
*                 MSD_PHY_DTE_POWER_DETECTION
*                 MSD_PHY_POLARITY_CHANGED
*                 MSD_PHY_JABBER
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
*
*******************************************************************************/
MSD_STATUS Topaz_gphySetIntEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
IN MSD_U16 en
);

/*******************************************************************************
* Topaz_gphyGetIntEnable
*
* DESCRIPTION:
*       This routine gets one PHY Interrupt enable/disable, When an interrupt occurs,
*       the corresponding bit is set and remains set until register 19 is read. When
*       interrupt enable bits are not set in register 18, interrupt status bits in
*       register 19 are still set when the corresponding interrupt events occur.
*       If a certain interrupt event is not enabled for the INTn pin, it will still
*       be indicated by the corresponding Interrupt status bits if the interrupt event occurs.
*       The unselected events will not cause the INTn pin to be activated.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*
* OUTPUTS:
*       en      - the data of interrupt to enable/disable.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_gphyGetIntEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
OUT MSD_U16 *en
);

/*******************************************************************************
* Topaz_gphyGetIntStatus
*
* DESCRIPTION:
*       This routine gets one PHY Interrupt status, When an interrupt occurs,
*       the corresponding bit is set and remains set until register 19 is read. When
*       interrupt enable bits are not set in register 18, interrupt status bits in
*       register 19 are still set when the corresponding interrupt events occur.
*       If a certain interrupt event is not enabled for the INTn pin, it will still
*       be indicated by the corresponding Interrupt status bits if the interrupt event occurs.
*       The unselected events will not cause the INTn pin to be activated.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*
* OUTPUTS:
*       en      - the data of interrupt to enable/disable.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_gphyGetIntStatus
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
OUT MSD_U16 *stat
);


#ifdef __cplusplus
}
#endif

#endif /* __msdPhyCtrl_h */
