/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Agate_msdPortPrioMap.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Port Priority Mapping Rules functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Agate_msdPortPrioMap_h
#define __Agate_msdPortPrioMap_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>


#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Types			 			                                   */
/****************************************************************************/

/*
 * Typedef: enum AGATE_MSD_INITIAL_PRI
 *
 * Description: Defines Initial Priority
 *
 * Fields:
 *      AGATE_MSD_INITIAL_PRI_DEFAULT	- Use Port defauts for Fpri, Qpri&Color.
 *      AGATE_MSD_INITIAL_PRI_TAG		- Use Tag Priority.
 *      AGATE_MSD_INITIAL_PRI_IP		- Use IP Priority.
 *		AGATE_MSD_INITIAL_PRI_TAG_IP	- Use Tag&IP Priority.
 *                 
 */
typedef enum
{
    AGATE_MSD_INITIAL_PRI_DEFAULT= 0,
    AGATE_MSD_INITIAL_PRI_TAG,
    AGATE_MSD_INITIAL_PRI_IP,
    AGATE_MSD_INITIAL_PRI_TAG_IP
} AGATE_MSD_INITIAL_PRI;


/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* Agate_gqosSetPrioMapRule
*
* DESCRIPTION:
*       This routine sets priority mapping rule(TagIfBoth bit).
*       If the current frame is both IEEE 802.3ac tagged and an IPv4 or IPv6, 
*       and UserPrioMap (for IEEE 802.3ac) and IPPrioMap (for IP frame) are 
*       enabled, then priority selection is made based on this setup.
*       If PrioMapRule is set to MSD_TRUE, UserPrioMap is used.
*       If PrioMapRule is reset to MSD_FALSE, IPPrioMap is used.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for user prio rule, MSD_FALSE for otherwise.
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
MSD_STATUS Agate_gqosSetPrioMapRule
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   port,
    IN  MSD_BOOL    mode
);
MSD_STATUS Agate_gqosSetPrioMapRuleIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_BOOL    mode
);
/*******************************************************************************
* Agate_gqosGetPrioMapRule
*
* DESCRIPTION:
*       This routine gets priority mapping rule (TagIfBoth bit).
*        If the current frame is both IEEE 802.3ac tagged and an IPv4 or IPv6, 
*        and UserPrioMap (for IEEE 802.3ac) and IPPrioMap (for IP frame) are 
*        enabled, then priority selection is made based on this setup.
*        If PrioMapRule is set to MSD_TRUE, UserPrioMap is used.
*        If PrioMapRule is reset to MSD_FALSE, IPPrioMap is used.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for user prio rule, MSD_FALSE for otherwise.
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
MSD_STATUS Agate_gqosGetPrioMapRule
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_BOOL   *mode
);
MSD_STATUS Agate_gqosGetPrioMapRuleIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL   *mode
);
/*******************************************************************************
* Agate_gprtSetInitialPri
*
* DESCRIPTION:
*       This routine set initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            AGATE_MSD_INITIAL_PRI_DEFAULT
*               
*            AGATE_MSD_INITIAL_PRI_TAG
*                
*            AGATE_MSD_INITIAL_PRI_IP
*                
*            AGATE_MSD_INITIAL_PRI_TAG_IP
*                
*
* INPUTS:
*       port - the logical port number.
*       mode - AGATE_MSD_INITIAL_PRI type
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
MSD_STATUS Agate_gprtSetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    IN  AGATE_MSD_INITIAL_PRI  mode
);
MSD_STATUS Agate_gprtSetInitialPriIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    IN  MSD_INITIAL_PRI  mode
);
/*******************************************************************************
* Agate_gprtGetInitialPri
*
* DESCRIPTION:
*       This routine gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            AGATE_MSD_INITIAL_PRI_DEFAULT
*               
*            AGATE_MSD_INITIAL_PRI_TAG
*                
*            AGATE_MSD_INITIAL_PRI_IP
*                
*            AGATE_MSD_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - AGATE_MSD_INITIAL_PRI type
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
MSD_STATUS Agate_gprtGetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT AGATE_MSD_INITIAL_PRI  *mode
);
MSD_STATUS Agate_gprtGetInitialPriIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT MSD_INITIAL_PRI  *mode
);
/*******************************************************************************
* Agate_gprtSetDefQpri
*
* DESCRIPTION:
*       This routine Set Default queue priority.
*
* INPUTS:
*       port - the logical port number
*       defque - default queue priority (0~7)
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
MSD_STATUS Agate_gprtSetDefQpri
(
  IN  MSD_QD_DEV    *dev,
  IN  MSD_LPORT    port,
  IN  MSD_U8  	  defque
);
MSD_STATUS Agate_gprtSetDefQpriIntf
(
  IN  MSD_QD_DEV    *dev,
  IN  MSD_LPORT    port,
  IN  MSD_U8  	  defque
);
/*******************************************************************************
* Agate_gprtGetDefQpri
*
* DESCRIPTION:
*       This routine Get Default queue priority.
*
* INPUTS:
*       port - the logical port number
*
* OUTPUTS:
*       defque - default queue priority structure
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
MSD_STATUS Agate_gprtGetDefQpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_U8       *defque
);
MSD_STATUS Agate_gprtGetDefQpriIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_U8       *defque
);
/******************************************************************************
* Agate_gprtSetDefFpri
*
* Set port default priority.
*
******************************************************************************/
MSD_STATUS Agate_gprtSetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8  	  defque
);
/******************************************************************************
* Agate_gprtGetDefFpri
*
* Get port default priority.
*
******************************************************************************/
MSD_STATUS Agate_gprtGetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_U8  	   *defque
);
#ifdef __cplusplus
}
#endif

#endif /* __Agate_msdPortPrioMap_h */
