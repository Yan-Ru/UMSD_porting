/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Spruce_msdPortPrioMap.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Port Priority Mapping Rules functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Spruce_msdPortPrioMap_h
#define Spruce_msdPortPrioMap_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>


#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Types			 			                                   */
/****************************************************************************/

/*
 * Typedef: enum SPRUCE_MSD_INITIAL_PRI
 *
 * Description: Defines Initial Priority
 *
 * Fields:
 *      SPRUCE_MSD_INITIAL_PRI_DEFAULT	- Use Port defauts for Fpri, Qpri&Color.
 *      SPRUCE_MSD_INITIAL_PRI_TAG		- Use Tag Priority.
 *      SPRUCE_MSD_INITIAL_PRI_IP		- Use IP Priority.
 *		SPRUCE_MSD_INITIAL_PRI_TAG_IP	- Use Tag&IP Priority.
 *                 
 */
typedef enum
{
    SPRUCE_MSD_INITIAL_PRI_DEFAULT= 0,
    SPRUCE_MSD_INITIAL_PRI_TAG,
    SPRUCE_MSD_INITIAL_PRI_IP,
    SPRUCE_MSD_INITIAL_PRI_TAG_IP
} SPRUCE_MSD_INITIAL_PRI;

/* Supported Frame Priority Table set For IEEE priority mapping */
typedef enum
{
  SPRUCE_PRI_ING_PCP_2_F_Q_PRI_DEI_0,		/* 0x0 = Ingress PCP to IEEE_FPRI & IEEE_QPRI mapping w/DEI = 0 */
  SPRUCE_PRI_EG_GREEN_FPRI_2_PCP,			/* 0x1 = Egress Green Frame FPRI to PCP mapping (non-AVB) */
  SPRUCE_PRI_EG_YEL_FPRI_2_PCP,			/* 0x2 = Egress Yellow Frame FPRI to PCP mapping (non-AVB) */
  SPRUCE_PRI_EG_AVB_FPRI_2_PCP,			/* 0x3 = Egress AVB Frame FPRI to PCP mapping */
  SPRUCE_PRI_ING_PCP_2_F_Q_PRI_DEI_1,		/* 0x4 = Ingress PCP to IEEE_FPRI & IEEE_QPRI mapping w/DEI = 1 */
  SPRUCE_PRI_EG_GREEN_FPRI_2_DSCP,		    /* 0x5 = Egress Green Frame FPRI to DSCP mapping (non-AVB) */
  SPRUCE_PRI_EG_YEL_FPRI_2_DSCP,			/* 0x6 = Egress Yellow Frame FPRI to DSCP mapping (non-AVB) */
  SPRUCE_PRI_EG_AVB_FPRI_2_DSCP,			/* 0x7 = Egress AVB Frame FPRI to DSCP mapping */
} SPRUCE_MSD_IEEE_PRIORITY_TABLE_SET;

/* Supported the format of each Table set For IEEE priority mapping */
typedef struct
{
	MSD_U8     ieeeYellow;
	MSD_U8     disIeeeQpri;
	MSD_U8     ieeeQpri;
	MSD_U8     disIeeeFpri;
	MSD_U8     ieeeFpri;
} SPRUCE_PRI_PCP_2_FQ_PRI_DEI_0_S;
typedef struct
{
	MSD_U8     disVIDMarking;
	MSD_U8     disPCPMarking;
	MSD_U8     framePCP;
} SPRUCE_PRI_EG_GREEN_FPRI_2_PCP_S;
typedef struct
{
	MSD_U8     disVIDMarking;
	MSD_U8     disPCPMarking;
	MSD_U8     framePCP;
} SPRUCE_PRI_EG_YEL_FPRI_2_PCP_S;
typedef struct
{
	MSD_U8     disVIDMarking;
	MSD_U8     disPCPMarking;
	MSD_U8     framePCP;
} SPRUCE_PRI_EG_AVB_FPRI_2_PCP_S;
typedef struct
{
	MSD_U8     ieeeYellow;
	MSD_U8     disIeeeQpri;
	MSD_U8     ieeeQpri;
	MSD_U8     disIeeeFpri;
	MSD_U8     ieeeFpri;
} SPRUCE_PRI_PCP_2_F_Q_PRI_DEI_1_S;
typedef struct
{
	MSD_U8     enDSCPMarking;
	MSD_U8     frameDSCP;
} SPRUCE_PRI_EG_GREEN_FPRI_2_DSCP_S;
typedef struct
{
	MSD_U8     enDSCPMarking;
	MSD_U8     frameDSCP;
} SPRUCE_PRI_EG_YEL_FPRI_2_DSCP_S;
typedef struct
{
	MSD_U8     enDSCPMarking;
	MSD_U8     frameDSCP;
} SPRUCE_PRI_EG_AVB_FPRI_2_DSCP_S;

typedef union
{
  SPRUCE_PRI_PCP_2_FQ_PRI_DEI_0_S         ingTblDei0;
  SPRUCE_PRI_EG_GREEN_FPRI_2_PCP_S        egGreeTblPcp;
  SPRUCE_PRI_EG_YEL_FPRI_2_PCP_S          egYelTblPcp;
  SPRUCE_PRI_EG_AVB_FPRI_2_PCP_S          egAvbTblPcp;
  SPRUCE_PRI_PCP_2_F_Q_PRI_DEI_1_S        ingTblDei1;
  SPRUCE_PRI_EG_GREEN_FPRI_2_DSCP_S       egGreeTblDscp;
  SPRUCE_PRI_EG_YEL_FPRI_2_DSCP_S         egYelTblDscp;
  SPRUCE_PRI_EG_AVB_FPRI_2_DSCP_S         egAvbTbldscp;
} SPRUCE_PRI_TABLE_STRUCT;

/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* Spruce_gqosSetPrioMapRule
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
MSD_STATUS Spruce_gqosSetPrioMapRule
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   port,
    IN  MSD_BOOL    mode
);
MSD_STATUS Spruce_gqosSetPrioMapRuleIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_BOOL    mode
);
/*******************************************************************************
* Spruce_gqosGetPrioMapRule
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
MSD_STATUS Spruce_gqosGetPrioMapRule
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_BOOL   *mode
);
MSD_STATUS Spruce_gqosGetPrioMapRuleIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL   *mode
);
/*******************************************************************************
* Spruce_gprtSetInitialPri
*
* DESCRIPTION:
*       This routine set initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            SPRUCE_MSD_INITIAL_PRI_DEFAULT
*               
*            SPRUCE_MSD_INITIAL_PRI_TAG
*                
*            SPRUCE_MSD_INITIAL_PRI_IP
*                
*            SPRUCE_MSD_INITIAL_PRI_TAG_IP
*                
*
* INPUTS:
*       port - the logical port number.
*       mode - SPRUCE_MSD_INITIAL_PRI type
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
MSD_STATUS Spruce_gprtSetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    IN  SPRUCE_MSD_INITIAL_PRI  mode
);
MSD_STATUS Spruce_gprtSetInitialPriIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    IN  MSD_INITIAL_PRI  mode
);
/*******************************************************************************
* Spruce_gprtGetInitialPri
*
* DESCRIPTION:
*       This routine gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            SPRUCE_MSD_INITIAL_PRI_DEFAULT
*               
*            SPRUCE_MSD_INITIAL_PRI_TAG
*                
*            SPRUCE_MSD_INITIAL_PRI_IP
*                
*            SPRUCE_MSD_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - SPRUCE_MSD_INITIAL_PRI type
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
MSD_STATUS Spruce_gprtGetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT SPRUCE_MSD_INITIAL_PRI  *mode
);
MSD_STATUS Spruce_gprtGetInitialPriIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT MSD_INITIAL_PRI  *mode
);
/*******************************************************************************
* Spruce_gprtSetDefQpri
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
MSD_STATUS Spruce_gprtSetDefQpri
(
  IN  MSD_QD_DEV    *dev,
  IN  MSD_LPORT    port,
  IN  MSD_U8  	  defque
);
MSD_STATUS Spruce_gprtSetDefQpriIntf
(
  IN  MSD_QD_DEV    *dev,
  IN  MSD_LPORT    port,
  IN  MSD_U8  	  defque
);
/*******************************************************************************
* Spruce_gprtGetDefQpri
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
MSD_STATUS Spruce_gprtGetDefQpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_U8       *defque
);
MSD_STATUS Spruce_gprtGetDefQpriIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_U8       *defque
);
/*******************************************************************************
* Spruce_gprtSetIEEEPrioMap
*
* DESCRIPTION:
*       This API set port IEEE Priority Mapping.
*
* INPUTS:
*       port		- the logical port number
*       table		- Frame Priority Table set
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr		- Pointer to the desired entry of the IEEE Priority  
*						  Mapping table selected above
*       entryStruct	- Mapping data structure for the above selected entry.
*							SPRUCE_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*							SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*							SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*							SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*							SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*							SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*							SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
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
MSD_STATUS Spruce_gprtSetIEEEPrioMap
(
    IN  MSD_QD_DEV	*dev,
    IN  MSD_LPORT	port,
    IN  SPRUCE_MSD_IEEE_PRIORITY_TABLE_SET		table,
    IN  MSD_U8							entryPtr,
	IN  SPRUCE_PRI_TABLE_STRUCT   *entryStruct
);
MSD_STATUS Spruce_gprtSetIEEEPrioMapIntf
(
    IN  MSD_QD_DEV	*dev,
    IN  MSD_LPORT	port,
    IN  MSD_IEEE_PRIORITY_TABLE_SET		table,
    IN  MSD_U8							entryPtr,
    IN  MSD_IEEE_PRIORITY_TABLE_STRUCT   *entryStruct
);
/*******************************************************************************
* Spruce_gprtGetIEEEPrioMap
*
* DESCRIPTION:
*       This API ge tport IEEE Priority Mapping.
*
* INPUTS:
*       port		- the logical port number
*       table		- Frame Priority Table set
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr	- Pointer to the desired entry of the IEEE Priority  
*					  Mapping table selected above
*
* OUTPUTS:
*       entryStruct	- Mapping data structure for the above selected entry.
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*						SPRUCE_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
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
MSD_STATUS Spruce_gprtGetIEEEPrioMap
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_LPORT    port,
    IN  SPRUCE_MSD_IEEE_PRIORITY_TABLE_SET      table,
    IN  MSD_U8							entryPtr,
	OUT SPRUCE_PRI_TABLE_STRUCT    *entryStruct
);
MSD_STATUS Spruce_gprtGetIEEEPrioMapIntf
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_LPORT    port,
    IN  MSD_IEEE_PRIORITY_TABLE_SET      table,
    IN  MSD_U8							entryPtr,
    OUT MSD_IEEE_PRIORITY_TABLE_STRUCT    *entryStruct
);

/*******************************************************************************
* Spruce_gprtRewriteDscp
*
* DESCRIPTION:
*       Re-write ip dscp at egress based on the classification/trust settings in the switch. When this is
*       disabled, the packet is given qos treatment within the switch but the DSCP is not modified at the
*       egress. The L2 cos value gets modified as per the classification/trust settings in the switch
*       irrespective of this setting.
*
* INPUTS:
*       portNum - the logical port number
*       en      �C enable/disable dscp re-write at egress on the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Spruce_gprtRewriteDscp
(
    IN  MSD_QD_DEV *dev,
    IN MSD_LPORT   portNum,
    IN MSD_BOOL    en
);
/*******************************************************************************
* Spruce_gprtRewriteDscp
*
* DESCRIPTION:
*       Re-write ip dscp at egress based on the classification/trust settings in the switch. When this is
*       disabled, the packet is given qos treatment within the switch but the DSCP is not modified at the
*       egress. The L2 cos value gets modified as per the classification/trust settings in the switch
*       irrespective of this setting.
*
* INPUTS:
*       portNum - the logical port number
*       en      �C enable/disable dscp re-write at egress on the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Spruce_gprtRewriteDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_BOOL    en
);
/*******************************************************************************
* Spruce_gprtSetCosToQueue
*
* DESCRIPTION:
*       Update the COS to egress queue mapping.
*
* INPUTS:
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos for the packet
*       queue   - Physical queue to be mapped for trusted or assigned cos
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
*		None.
*
*******************************************************************************/
MSD_STATUS Spruce_gprtSetCosToQueue
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cosVal,
    IN  MSD_U8     queue
);

/*******************************************************************************
* SPRUCE_gprtGetCosToQueue
*
* DESCRIPTION:
*       Get the physical queue to be mapped for trusted or assigned cos
* INPUTS:
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos for the packet
*
* OUTPUTS:
*       queue   - Physical queue to be mapped for trusted or assigned cos
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Spruce_gprtGetCosToQueue
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cosVal,
    OUT MSD_U8     *queue
);

/*******************************************************************************
* Spruce_gprtSetCosToDscp
*
* DESCRIPTION:
*       Update the COS to DSCP mapping. This will be used for rewriting DSCP for
*       IPv4/IPv6 packets based on the assigned cos value.
*
* INPUTS:
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos for the packet
*       dscp    - frame differentiated services code point
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
*		None.
*
*******************************************************************************/
MSD_STATUS Spruce_gprtSetCosToDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      cosVal,
    IN  MSD_U8      dscp
);
/*******************************************************************************
* Spruce_gprtGetCosToDscp
*
* DESCRIPTION:
*       Update the COS to DSCP mapping. This will be used for rewriting DSCP for
*       IPv4/IPv6 packets based on the assigned cos value.
*
* INPUTS:
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos for the packet
*
* OUTPUTS:
*       dscp    - frame differentiated services code point
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Spruce_gprtGetCosToDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      cosVal,
    OUT MSD_U8      *dscp
);

/******************************************************************************
* Spruce_gprtSetDefFpri
*
* Set port default priority.
*
******************************************************************************/
MSD_STATUS Spruce_gprtSetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8  	  defque
);
/******************************************************************************
* Spruce_gprtGetDefFpri
*
* Get port default priority.
*
******************************************************************************/
MSD_STATUS Spruce_gprtGetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_U8  	   *defque
);

#ifdef __cplusplus
}
#endif

#endif /* __Spruce_msdPortPrioMap_h */
