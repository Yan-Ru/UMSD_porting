/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Topaz_msdPIRLIntf.c
*
* DESCRIPTION:
*       API definitions for PIRL Resources
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <topaz/include/api/Topaz_msdPIRL.h>


/*******************************************************************************
* Topaz_gpirlInitialize
*
* DESCRIPTION:
*       This routine initializes PIRL Resources.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gpirlInitializeIntf
(
    IN  MSD_QD_DEV              *dev
)
{
	return Topaz_gpirlInitialize(dev);
}

/*******************************************************************************
* Topaz_gpirlInitResource
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource for a particular port.
*
* INPUTS:
*       irlPort	- target port
*		irlRes - resource unit to be accessed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Topaz_gpirlInitResourceIntf
(
    IN    MSD_QD_DEV            *dev,
	IN    MSD_LPORT             irlPort,
    IN    MSD_U32               irlRes
)
{
	return Topaz_gpirlInitResource(dev, irlPort, irlRes);
}

/*******************************************************************************
* Topaz_gpirlCustomSetup
*
* DESCRIPTION:
*       This function Calculate CBS/EBS/BI/BRF/Constant/Delta according to user 
*       specific target rate, target burst size and countMode.

*
* INPUTS:
*       tgtRate	- target rate(units is fps when countMode = MSD_PIRL_COUNT_FRAME,
*				  otherwise units is kbps)
*       tgtBstSize	- target burst size(units is Byte)
*       customSetup - Constant and countMode in customSetup as input parameter
*
* OUTPUTS:
*       customSetup - customer setup including CBS/EBS/BI/BRF/Constant/Delta/countMode
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		As the input for structure 'MSD_PIRL_CUSTOM_RATE_LIMIT', Constant can be 0 or any  
*		value from {7812500, 125000000, 62500000, 31250000, 15625000, 3906250, 1953750, 976250}.
*		If Constant is 0, this API will automatically calculate the best IRL Constant 
*		which means Delta is small enough or 0. However, since Constant is Global for all
*       Resource of all ports, user can specify fixed Constant to gurantee that the Constant is 
*		consistent across all existing resources.
*
*******************************************************************************/
MSD_STATUS Topaz_gpirlCustomSetupIntf
(
    IN  MSD_U32	   	tgtRate,
    IN  MSD_U32	   	tgtBstSize,
    IN  MSD_PIRL_COUNT_MODE  countMode,
	OUT MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
)
{
	TOPAZ_MSD_PIRL_CUSTOM_RATE_LIMIT setup;
	MSD_STATUS       retVal;

	if (NULL == customSetup)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_CUSTOM_RATE_LIMIT in Topaz_gpirlCustomSetupIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	setup.isValid = customSetup->isValid;
	setup.ebsLimit = customSetup->ebsLimit;
	setup.cbsLimit = customSetup->cbsLimit;
	setup.bktIncrement = customSetup->bktIncrement;
	setup.bktRateFactor = customSetup->bktRateFactorGrn;
	setup.countMode = countMode;
	setup.IRLConstant = customSetup->IRLConstant;

	retVal=Topaz_gpirlCustomSetup(tgtRate, tgtBstSize, &setup);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)customSetup, 0, sizeof(MSD_PIRL_CUSTOM_RATE_LIMIT));
	customSetup->isValid =setup.isValid ;
	customSetup->ebsLimit=setup.ebsLimit  ;
	customSetup->cbsLimit = setup.cbsLimit;
	customSetup->bktIncrement = setup.bktIncrement;
	customSetup->bktRateFactorGrn = setup.bktRateFactor;
	customSetup->countMode = setup.countMode;
	customSetup->IRLConstant = setup.IRLConstant;

	return retVal;
}
/*******************************************************************************
* Topaz_gpirlReadResource
*
* DESCRIPTION:
*       This function read Resource bucket parameter from the given resource of port
*
* INPUTS:
*       irlPort - target logical port
*		irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       pirlData - IRL Resource data
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_gpirlReadResourceIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    OUT MSD_PIRL_DATA    *pirlData
)
{
	TOPAZ_MSD_PIRL_DATA pirl;
	MSD_STATUS       retVal;

	retVal = Topaz_gpirlReadResource(dev, irlPort, irlRes, &pirl);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == pirlData)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_DATA in Topaz_gpirlReadResourceIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)pirlData, 0, sizeof(MSD_PIRL_DATA));

	pirlData->bktTypeMask = pirl.bktTypeMask;
	pirlData->tcamFlows = pirl.tcamFlows;

	/*
	pirlData->priORpt = pirl.priORpt;
	*/
	pirlData->priAndPt = (pirl.priORpt == MSD_TRUE ? MSD_FALSE : MSD_TRUE);

	pirlData->priSelect = pirl.priSelect;

	pirlData->accountQConf = pirl.accountQConf;
	pirlData->accountFiltered = pirl.accountFiltered;
	pirlData->samplingMode = pirl.samplingMode;
	pirlData->actionMode = pirl.actionMode;
	pirlData->fcMode = pirl.fcMode;

	pirlData->customSetup.isValid = pirl.customSetup.isValid;
	pirlData->customSetup.ebsLimit = pirl.customSetup.ebsLimit;
	pirlData->customSetup.cbsLimit = pirl.customSetup.cbsLimit;
	pirlData->customSetup.bktIncrement = pirl.customSetup.bktIncrement;
	/*
	pirlData->customSetup.bktRateFactor = pirl.customSetup.bktRateFactor;
	*/
	pirlData->customSetup.bktRateFactorGrn = pirl.customSetup.bktRateFactor;

	pirlData->customSetup.IRLConstant = pirl.customSetup.IRLConstant;
	pirlData->customSetup.countMode = pirl.customSetup.countMode;

	/*
	pirlData->ebsLimitAction = pirl.ebsLimitAction;
	*/
	pirlData->actionMode = pirl.ebsLimitAction;

	return retVal;

}
/*******************************************************************************
* Topaz_gpirlWriteResource
*
* DESCRIPTION:
*       This function writes Resource bucket parameter to the given resource of port
*
* INPUTS:
*       irlPort	- target port
*		irlRes  - resource unit to be accessed
*       pirlData - IRL Resource data
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
MSD_STATUS Topaz_gpirlWriteResourceIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
	IN  MSD_PIRL_DATA    *pirlData
)
{
	TOPAZ_MSD_PIRL_DATA pirl;

	if (NULL == pirlData)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_DATA in Topaz_gpirlWriteResourceIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	
	if (pirlData->useFPri == MSD_TRUE)
	{
		MSD_DBG_ERROR(("Bad Param(useFPri), Topaz not support Rate limit for Frame Priority. \n"));
		return MSD_BAD_PARAM;
	}

	pirl.bktTypeMask = pirlData->bktTypeMask;
	pirl.tcamFlows = pirlData->tcamFlows;

	/*
	pirl.priORpt = pirlData->priORpt;
	*/
	pirl.priORpt = (pirlData->priAndPt == MSD_TRUE ? MSD_FALSE : MSD_TRUE);

	pirl.priSelect = pirlData->priSelect;

	pirl.accountQConf = pirlData->accountQConf;
	pirl.accountFiltered = pirlData->accountFiltered;
	pirl.samplingMode = pirlData->samplingMode;
	pirl.actionMode = pirlData->actionMode;
	pirl.fcMode = pirlData->fcMode;

	pirl.customSetup.isValid = pirlData->customSetup.isValid;
	pirl.customSetup.ebsLimit = pirlData->customSetup.ebsLimit;
	pirl.customSetup.cbsLimit = pirlData->customSetup.cbsLimit;
	pirl.customSetup.bktIncrement = pirlData->customSetup.bktIncrement;
	/*
	pirl.customSetup.bktRateFactor = pirlData->customSetup.bktRateFactor;
	*/
	pirl.customSetup.bktRateFactor = pirlData->customSetup.bktRateFactorGrn;

	pirl.customSetup.IRLConstant = pirlData->customSetup.IRLConstant;
	pirl.customSetup.countMode = pirlData->customSetup.countMode;
	/*
	 * if actionMode = drop
	 *    topaz_action = Topaz_PIRL_ACTION_USE_LIMIT_ACTION (0)
	 *    topaz_ebsLimitAction = Topaz_EBS_LIMIT_ACTION_DROP (0)
	 * else if actionMode = FC
	 *    topaz_action = Topaz_PIRL_ACTION_USE_LIMIT_ACTION (0)
 	 *    topaz_ebsLimitAction = Topaz_EBS_LIMIT_ACTION_FC(1)
	 *
	 */
	pirl.actionMode = Topaz_PIRL_ACTION_USE_LIMIT_ACTION;
	pirl.ebsLimitAction = pirlData->actionMode;
	return Topaz_gpirlWriteResource(dev, irlPort, irlRes, &pirl);

}

/*******************************************************************************
* Topaz_gpirlGetResReg
*
* DESCRIPTION:
*       This routine read general register value from the given resource of the port.
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - target resource 
*       regOffset - register address
*
* OUTPUTS:
*        data	- register value
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
MSD_STATUS Topaz_gpirlGetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT	 irlPort,
IN  MSD_U32		 irlRes,
IN  MSD_U32 		 regOffset,
OUT MSD_U16		 *data
)
{
	return Topaz_gpirlGetResReg(dev, irlPort, irlRes, regOffset, data);
}

/*******************************************************************************
* Topaz_gpirlSetResReg
*
* DESCRIPTION:
*       This routine set general register value to the given resource of the port..
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - target resource 
*       regOffset - register address
*       data	- register value
*
* OUTPUTS:
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
MSD_STATUS Topaz_gpirlSetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT	 irlPort,
IN  MSD_U32		 irlRes,
IN  MSD_U32 		 regOffset,
IN  MSD_U16		 data
)
{
	return Topaz_gpirlSetResReg(dev, irlPort, irlRes, regOffset, data);
}


/*******************************************************************************
* Topaz_gpirlAdvConfigIntf
*
* DESCRIPTION:
*       This routine config ingress rate limit based on packet type or priority
*       the given resource of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarPeridot_get resource
*       regOffset - register address
*
* OUTPUTS:
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
MSD_STATUS Topaz_gpirlAdvConfigIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes,
IN  MSD_U32  tgtRate,
IN  MSD_U32  tgtBstSize,
IN  MSD_PIRL_COUNT_MODE  countMode,
IN  MSD_PIRL_PKTTYPE_OR_PRI  *pktTypeOrPri
)
{
    MSD_STATUS status;
    TOPAZ_MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;
    customSetup.IRLConstant = 0;
    customSetup.countMode = countMode;

	if (pktTypeOrPri->useFPri == MSD_TRUE)
	{
		MSD_DBG_ERROR(("Bad Param(useFPri), Topaz not support Rate limit for Frame Priority. \n"));
		return MSD_BAD_PARAM;
	}

    if ((status = Topaz_gpirlCustomSetup(tgtRate, tgtBstSize, &customSetup)) != MSD_OK)
    {
        return status;
    }

    TOPAZ_MSD_PIRL_DATA pirl;
    msdMemSet(&pirl, 0, sizeof(TOPAZ_MSD_PIRL_DATA));
    pirl.bktTypeMask = pktTypeOrPri->bktTypeMask;
    pirl.tcamFlows = pktTypeOrPri->tcamFlows;
    pirl.priORpt = (pktTypeOrPri->priAndPt == MSD_TRUE ? MSD_FALSE : MSD_TRUE);
    /*
    pirl.useFPri = pktTypeOrPri->useFPri;
    */
    pirl.priSelect = pktTypeOrPri->priSelect;
    pirl.accountQConf = MSD_TRUE;
    pirl.actionMode = Topaz_PIRL_ACTION_USE_LIMIT_ACTION;

    pirl.customSetup.isValid = customSetup.isValid;
    pirl.customSetup.ebsLimit = customSetup.ebsLimit;
    pirl.customSetup.cbsLimit = customSetup.cbsLimit;
    pirl.customSetup.bktIncrement = customSetup.bktIncrement;
    pirl.customSetup.bktRateFactor = customSetup.bktRateFactor;
    pirl.customSetup.IRLConstant = customSetup.IRLConstant;

    pirl.customSetup.countMode = customSetup.countMode;

    return Topaz_gpirlWriteResource(dev, irlPort, irlRes, &pirl);
}

/*******************************************************************************
* Topaz_gpirlAdvConfigDisableIntf
*
* DESCRIPTION:
*       This routine clear config for ingress rate limit to the given resource
*       of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - resource
*       regOffset - register address
*
* OUTPUTS:
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
MSD_STATUS Topaz_gpirlAdvConfigDisableIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes
)
{
    TOPAZ_MSD_PIRL_DATA pirl;
    msdMemSet(&pirl, 0, sizeof(TOPAZ_MSD_PIRL_DATA));

	pirl.customSetup.IRLConstant = 62500000;

    return Topaz_gpirlWriteResource(dev, irlPort, irlRes, &pirl);
}
