/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Topaz_msdPTPIntf.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <topaz/include/api/Topaz_msdPTP.h>

MSD_STATUS Topaz_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    TOPAZ_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_INTERRUPT_EN in Topaz_gptpSetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempIntEn.ptpArrIntEn = interruptEn->ptpArrIntEn;
    tempIntEn.ptpDepIntEn = interruptEn->ptpDepIntEn;

    return Topaz_gptpSetIntEnable(dev, port, &tempIntEn);
}

MSD_STATUS Topaz_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS retVal;
    TOPAZ_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_INTERRUPT_EN in Topaz_gptpGetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Topaz_gptpGetIntEnable(dev, port, &tempIntEn);
    if (MSD_OK != retVal)
        return retVal;

    interruptEn->ptpArrIntEn = tempIntEn.ptpArrIntEn;
    interruptEn->ptpDepIntEn = tempIntEn.ptpDepIntEn;

    return retVal;
}

MSD_STATUS Topaz_gptpSetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_PTP_PORT_CONFIG *ptpData
)
{
    TOPAZ_PTP_PORT_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_CONFIG in Topaz_gptpSetPortConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempData.arrLedCtrl = ptpData->arrLedCtrl;
    tempData.arrTSMode = ptpData->arrTSMode;
    tempData.depLedCtrl = ptpData->depLedCtrl;
    tempData.disTSOverwrite = ptpData->disTSOverwrite;
    tempData.disTSpec = ptpData->disTSpec;
    tempData.etJump = ptpData->etJump;
    tempData.filterAct = ptpData->filterAct;
    tempData.ipJump = ptpData->ipJump;
    tempData.ptpArrIntEn = ptpData->ptpArrIntEn;
    tempData.ptpDepIntEn = ptpData->ptpDepIntEn;
    tempData.transSpec = ptpData->transSpec;

    return Topaz_gptpSetPortConfig(dev, port, &tempData);
}

MSD_STATUS Topaz_gptpGetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_CONFIG *ptpData
)
{
    MSD_STATUS retVal;
    TOPAZ_PTP_PORT_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_CONFIG in Topaz_gptpGetPortConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Topaz_gptpGetPortConfig(dev, port, &tempData);
    if (retVal != MSD_OK)
        return retVal;

    ptpData->arrLedCtrl = tempData.arrLedCtrl;
    ptpData->arrTSMode = tempData.arrTSMode;
    ptpData->depLedCtrl = tempData.depLedCtrl;
    ptpData->disTSOverwrite = tempData.disTSOverwrite;
    ptpData->disTSpec = tempData.disTSpec;
    ptpData->etJump = tempData.etJump;
    ptpData->filterAct = tempData.filterAct;
    ptpData->ipJump = tempData.ipJump;
    ptpData->ptpArrIntEn = tempData.ptpArrIntEn;
    ptpData->ptpDepIntEn = tempData.ptpDepIntEn;
    ptpData->transSpec = tempData.transSpec;

    return retVal;
}

MSD_STATUS Topaz_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    TOPAZ_PTP_TS_STATUS tempStatus;

    if (ptpStatus == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TS_STATUS in Topaz_gptpGetTimeStampIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Topaz_gptpGetTimeStamp(dev, port, (TOPAZ_PTP_TIME)timeToRead, &tempStatus);
    if (retVal != MSD_OK)
        return retVal;

    ptpStatus->isValid = tempStatus.isValid;
    ptpStatus->ptpSeqId = tempStatus.ptpSeqId;
    ptpStatus->status = tempStatus.status;
    ptpStatus->timeStamped = tempStatus.timeStamped;

    return retVal;
}

MSD_STATUS Topaz_gptpSetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    TOPAZ_PTP_GLOBAL_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_GLOBAL_CONFIG in Topaz_gptpSetGlobalConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempData.msgIdTSEn = ptpData->msgIdTSEn;
    tempData.ptpEType = ptpData->ptpEType;
    tempData.tsArrPtr = ptpData->tsArrPtr;

    return Topaz_gptpSetGlobalConfig(dev, &tempData);
}

MSD_STATUS Topaz_gptpGetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MSD_STATUS retVal;
    TOPAZ_PTP_GLOBAL_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_GLOBAL_CONFIG in Topaz_gptpGetGlobalConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Topaz_gptpGetGlobalConfig(dev, &tempData);
    if (retVal != MSD_OK)
        return retVal;

    ptpData->msgIdTSEn = tempData.msgIdTSEn;
    ptpData->ptpEType = tempData.ptpEType;
    ptpData->tsArrPtr = tempData.tsArrPtr;

    return retVal;
}

MSD_STATUS Topaz_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    TOPAZ_PTP_INTERRUPT_STATUS tempIntSt;

    if (ptpIntSt == NULL)
    {
        MSD_DBG_ERROR(("param PTP_INTERRUPT_STATUS in Topaz_gptpGetIntStatusIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Topaz_gptpGetIntStatus(dev, &tempIntSt);
    if (retVal != MSD_OK)
        return retVal;

    ptpIntSt->ptpEventInt = tempIntSt.ptpEventInt;
    ptpIntSt->ptpIntStPortVect = tempIntSt.ptpIntStPortVect;
    ptpIntSt->ptpTrigGenInt = tempIntSt.ptpTrigGenInt;

    return retVal;
}