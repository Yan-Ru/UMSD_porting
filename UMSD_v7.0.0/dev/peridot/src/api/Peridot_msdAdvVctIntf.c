/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Peridot_msdAdvVctIntf.c
*
* DESCRIPTION:
*       API for Marvell Virtual Cable Tester.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <peridot/include/api/Peridot_msdVct.h>

/*******************************************************************************
* gvctGetAdvCableDiag
*
* DESCRIPTION:
*       This routine perform the advanced virtual cable test for the requested
*       port and returns the the status per MDI pair.
*
* INPUTS:
*       port - logical port number.
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gvctGetAdvCableDiagIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT        port,
    IN  MSD_ADV_VCT_MODE mode,
    OUT MSD_ADV_CABLE_STATUS *cableStatus
)
{
	PERIDOT_MSD_ADV_VCT_MODE vctmode;
	PERIDOT_MSD_ADV_CABLE_STATUS cable;
	MSD_STATUS  retVal;
	MSD_U8 i = 0, j = 0;

	vctmode.mode = mode.mode;
	vctmode.peakDetHyst = mode.peakDetHyst;
	vctmode.sampleAvg = mode.sampleAvg;
	vctmode.samplePointDist = mode.samplePointDist;
	vctmode.transChanSel = mode.transChanSel;
	
	retVal=Peridot_gvctGetAdvCableDiag(dev, port, vctmode, &cable);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == cableStatus)
	{
		MSD_DBG_ERROR(("Input param MSD_ADV_CABLE_STATUS in Peridot_gvctGetAdvCableDiagIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)cableStatus, 0, sizeof(MSD_ADV_CABLE_STATUS));

	for (i = 0; i < MSD_MDI_PAIR_NUM; i++)
	{
		cableStatus->cableStatus[i] = cable.cableStatus[i];
		for (j = 0; j < MSD_MDI_PAIR_NUM; j++)
		{
			cableStatus->u[i].crossShort.channel[j] = cable.u[i].crossShort.channel[j];
			cableStatus->u[i].crossShort.dist2fault[j] = cable.u[i].crossShort.dist2fault[j];
		}
	}
	return retVal;
}

/*******************************************************************************
* gvctGetAdvExtendedStatus
*
* DESCRIPTION:
*        This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*        DSP based cable length is also provided.
*
* INPUTS:
*       dev  - pointer to MSD driver structure returned from msdLoadDriver
*       port - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Peridot_gvctGetAdvExtendedStatusIntf
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT   port,
    OUT MSD_ADV_EXTENDED_STATUS *extendedStatus
)
{
	PERIDOT_MSD_ADV_EXTENDED_STATUS extend;
	MSD_STATUS retVal;
	MSD_U8 i = 0;

	retVal = Peridot_gvctGetAdvExtendedStatus(dev, port, &extend);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == extendedStatus)
	{
		MSD_DBG_ERROR(("Input param MSD_ADV_EXTENDED_STATUS in Peridot_gvctGetAdvExtendedStatusIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)extendedStatus, 0, sizeof(MSD_ADV_EXTENDED_STATUS));
	extendedStatus->isValid = extend.isValid;
	for (i = 0; i < MSD_MDI_PAIR_NUM; i++)
	{
		extendedStatus->pairSwap[i] = extend.pairSwap[i];
		extendedStatus->pairPolarity[i] = extend.pairPolarity[i];
		extendedStatus->pairSkew[i] = extend.pairSkew[i];
		extendedStatus->cableLen[i] = extend.cableLen[i];
	}

	return retVal;
}

