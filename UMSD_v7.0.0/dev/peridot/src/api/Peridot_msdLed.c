/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Peridot_msdLed.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell LED functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <peridot/include/api/Peridot_msdLed.h>
#include <peridot/include/api/Peridot_msdApiInternal.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#include <peridot/include/driver/Peridot_msdDrvSwRegs.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
#include <utils/msdUtils.h>


/****************************************************************************/
/* internal LED related function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Peridot_LedCtrl_Write
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT  portNum,
IN	MSD_U8	pointer,
IN	MSD_U16	data
);
static MSD_STATUS Peridot_LedCtrl_Read
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT  portNum,
IN	MSD_U8	pointer,
OUT	MSD_U16	*data
);

/*******************************************************************************
* Peridot_gledForceOn
*
* DESCRIPTION:
*       This routine is to Force Port LED On
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
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
MSD_STATUS Peridot_gledForceOn
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_STATUS retVal;
    MSD_U8 pointer;
    MSD_U8 foff;
    MSD_U8 flen;
    MSD_U16 val;
    MSD_U16 tmp;

    MSD_DBG_INFO(("Peridot_gledForceOn Called.\n"));

    if ((portNum > 10) || (0 == portNum))
    {
        MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
        return MSD_BAD_PARAM;
    }

    switch (ledNum)
    {
        case 0:
            pointer = 0;
            foff = 0;
            flen = 4;
            val = 0xf;
            break;
        case 1:
            pointer = 0;
            foff = 4;
            flen = 4;
            val = 0xf;
            break;
        default:
            MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", (unsigned int)ledNum));
            return MSD_BAD_PARAM;    
    }

    retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &tmp);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_BF_SET(tmp, val, foff, flen);

    retVal = Peridot_LedCtrl_Write(dev, portNum, pointer, tmp);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gledForceOn Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gledForceOff
*
* DESCRIPTION:
*       This routine is to Force Port LED Off
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
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
MSD_STATUS Peridot_gledForceOff
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_STATUS retVal;
    MSD_U8 pointer;
    MSD_U8 foff;
    MSD_U8 flen;
    MSD_U16 val;
    MSD_U16 tmp;

    MSD_DBG_INFO(("Peridot_gledForceOff Called.\n"));

    if ((portNum > 10) || (0 == portNum))
    {
        MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
        return MSD_BAD_PARAM;
    }

    switch (ledNum)
    {
        case 0:
            pointer = 0;
            foff = 0;
            flen = 4;
            val = 0xe;
            break;
        case 1:
            pointer = 0;
            foff = 4;
            flen = 4;
            val = 0xe;
            break;
        default:
            MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", (unsigned int)ledNum));
            return MSD_BAD_PARAM;
    }

    retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &tmp);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_BF_SET(tmp, val, foff, flen);

    retVal = Peridot_LedCtrl_Write(dev, portNum, pointer, tmp);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gledForceOff Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Peridot_gledLinkAct
*
* DESCRIPTION:
*       This routine is to set Port LED mode to link/act. That is:
*           off - no link, on - link, blink = activity.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
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
MSD_STATUS Peridot_gledLinkAct
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_STATUS retVal;
    MSD_U8 pointer;
    MSD_U8 foff;
    MSD_U8 flen;
    MSD_U16 val;
    MSD_U16 tmp;

    MSD_DBG_INFO(("Peridot_gledLinkAct Called.\n"));

    if ((portNum > 10) || (0 == portNum))
    {
        MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
        return MSD_BAD_PARAM;
    }

    switch (ledNum)
    {
        case 0:
            pointer = 0;
            foff = 0;
            flen = 4;
            if ((9 == portNum) || (10 == portNum))
            {
                val = 0x1;
            }
            else
            {
                val = 0x3;
            }
            break;
        case 1:
            pointer = 0;
            foff = 4;
            flen = 4;
            if ((9 == portNum) || (10 == portNum))
            {
                val = 0x7;
            }
            else
            {
                val = 0x5;
            }

            break;
        default:
            MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", (unsigned int)ledNum));
            return MSD_BAD_PARAM;
    }

    retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &tmp);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_BF_SET(tmp, val, foff, flen);

    retVal = Peridot_LedCtrl_Write(dev, portNum, pointer, tmp);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Peridot_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Peridot_gledLinkAct Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
*   msdLedModeSet
*
* DESCRIPTION:
*       This routine is to set led mode
*		For Peridot :
*		Port 1 - 8 :
*			0000 :  Link/Act/Speed by Blink Rate (off = no link, on = link, blink = activity, blink speed = link speed)
*			0001 :  100/Gig Link/Act (off = no link, on = 100 or Gig link, blink = activity)
*			0010 :  Gig Link/Act (off = no link, on = Gig link, blink = activity)
*			0010 :  Link/Act (off = no link, on = link, blink = activity)
*			0100 :  Special LED 3
*			0101 :  Special LED 4
*			0110 :  Duplex/Collision (off = half duplex, on = full duplex, blink = col)
*			0111 :  10/Gig Link/Act (off = no link, on = 10 or Gig link, blink = activity)
*			1000 :  Link (off = no link, on = link)
*			1001 :  10 Link (off = no link, on = 10 link)
*			1010 :  10 Link/Act (off = no link, on = 10 link, blink = activity)
*			1011 :  100/Gig Link (off = no link, on = 100 or Gig link)
*			1100 :  PTP Act (blink on = PTP activity)
*			1101 :  Force Blink
*			1110 :  Force Off
*			1111 :  Force On
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
*       mode  - LED mode
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
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledModeSet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum,
IN  MSD_U8    mode
)
{
	MSD_STATUS retVal;
	MSD_U8 pointer;
	MSD_U8 foff;
	MSD_U8 flen;
	MSD_U16 val;
	MSD_U16 tmp;

	MSD_DBG_INFO(("Peridot_gledModeSet Called.\n"));

	if (portNum > 10 || portNum == 0)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	if (mode > 0xf)
	{
		MSD_DBG_ERROR(("Bad Mode: %u. It should be within [0,15]\n", (unsigned int)mode));
		return MSD_BAD_PARAM;
	}
	switch (ledNum)
	{
	case 0:
		pointer = 0;
		foff = 0;
		flen = 4;
		val = mode;
		break;
	case 1:
		pointer = 0;
		foff = 4;
		flen = 4;
		val = mode;
		break;
	default:
		MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", (unsigned int)ledNum));
		return MSD_BAD_PARAM;
	}

	retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_BF_SET(tmp, val, foff, flen);

	retVal = Peridot_LedCtrl_Write(dev, portNum, pointer, tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Peridot_gledModeSet Exit.\n"));
	return MSD_OK;
}
/*******************************************************************************
*   msdLedModeGet
*
* DESCRIPTION:
*       This routine is to get led mode
*		For Peridot :
*		LED mode (Port 1 - 8) :
*			0000 :  Link/Act/Speed by Blink Rate (off = no link, on = link, blink = activity, blink speed = link speed)
*			0001 :  100/Gig Link/Act (off = no link, on = 100 or Gig link, blink = activity)
*			0010 :  Gig Link/Act (off = no link, on = Gig link, blink = activity)
*			0010 :  Link/Act (off = no link, on = link, blink = activity)
*			0100 :  Special LED 3
*			0101 :  Special LED 4
*			0110 :  Duplex/Collision (off = half duplex, on = full duplex, blink = col)
*			0111 :  10/Gig Link/Act (off = no link, on = 10 or Gig link, blink = activity)
*			1000 :  Link (off = no link, on = link)
*			1001 :  10 Link (off = no link, on = 10 link)
*			1010 :  10 Link/Act (off = no link, on = 10 link, blink = activity)
*			1011 :  100/Gig Link (off = no link, on = 100 or Gig link)
*			1100 :  PTP Act (blink on = PTP activity)
*			1101 :  Force Blink
*			1110 :  Force Off
*			1111 :  Force On
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
*
* OUTPUTS:
*       mode - Led mode.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledModeGet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum,
OUT  MSD_U8    *mode
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	
	MSD_DBG_INFO(("Peridot_gledModeGet Called.\n"));

	if (portNum > 10 || portNum == 0)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	
	retVal = Peridot_LedCtrl_Read(dev, portNum, 0, &tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	switch (ledNum)
	{
	case 0:
		*mode = tmp & 0x0f;
		break;
	case 1:
		*mode = (tmp & 0xf0) >> 4;
		break;
	default:
		MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", (unsigned int)ledNum));
		return MSD_BAD_PARAM;
	}
	MSD_DBG_INFO(("Peridot_gledModeGet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
*   msdLedBlinkRateSet
*
* DESCRIPTION:
*       This routine is to set led blink rate
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       rate  - LED blink rate
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
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledBlinkRateSet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  PERIDOT_MSD_LED_BLINK_RATE    rate
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	MSD_DBG_INFO(("Peridot_gledBlinkRateSet Called.\n"));

	if (portNum > 10 || portNum == 0)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	if (rate > PERIDOT_MSD_LED_BLINK_RATE_672_MS)
	{
		MSD_DBG_ERROR(("Bad Rate: %u.\n", (unsigned int)rate));
		return MSD_BAD_PARAM;
	}
	retVal = Peridot_LedCtrl_Read(dev, portNum, 6, &tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_BF_SET(tmp, (MSD_U16)rate, 0, 3);

	retVal = Peridot_LedCtrl_Write(dev, portNum, 6, tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Peridot_gledBlinkRateSet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
*   msdLedBlinkRateGet
*
* DESCRIPTION:
*       This routine is to get led blink rate
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       rate  - LED blink rate
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledBlinkRateGet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
OUT  PERIDOT_MSD_LED_BLINK_RATE    *rate
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	MSD_U16 val;
	MSD_DBG_INFO(("Peridot_gledBlinkRateGet Called.\n"));

	if (portNum > 10 || portNum == 0)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	retVal = Peridot_LedCtrl_Read(dev, portNum, 6, &tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	val = tmp & 0x7;
	switch (val)
	{
	case 0: *rate = PERIDOT_MSD_LED_BLINK_RATE_21_MS;
		break;
	case 1: *rate = PERIDOT_MSD_LED_BLINK_RATE_42_MS;
		break;
	case 2: *rate = PERIDOT_MSD_LED_BLINK_RATE_84_MS;
		break;
	case 3: *rate = PERIDOT_MSD_LED_BLINK_RATE_168_MS;
		break;
	case 4: *rate = PERIDOT_MSD_LED_BLINK_RATE_336_MS;
		break;
	case 5: *rate = PERIDOT_MSD_LED_BLINK_RATE_672_MS;
		break;
	default:
		MSD_DBG_ERROR(("Peridot_gledBlinkRateGet read error"));
		return MSD_FAIL;
	}

	MSD_DBG_INFO(("Peridot_gledBlinkRateGet Exit.\n"));
	return MSD_OK;
}


/*******************************************************************************
*   Peridot_gledPulseStretchSet
*
* DESCRIPTION:
*       This routine is to set led stretch duration
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       duration  - LED stretch duration
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
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledPulseStretchSet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  PERIDOT_MSD_LED_STRETCH_DURATION    duration
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	MSD_DBG_INFO(("Peridot_gledPulseStretchSet Called.\n"));

	if (portNum > 10 || portNum == 0)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	if (duration > PERIDOT_MSD_LED_STRETCH_DURATION_168_MS)
	{
		MSD_DBG_ERROR(("Bad duration: %u.\n", (unsigned int)duration));
		return MSD_BAD_PARAM;
	}
	retVal = Peridot_LedCtrl_Read(dev, portNum, 6, &tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_BF_SET(tmp, (MSD_U16)duration, 4, 3);

	retVal = Peridot_LedCtrl_Write(dev, portNum, 6, tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Peridot_gledPulseStretchSet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
*   Peridot_gledPulseStretchGet
*
* DESCRIPTION:
*       This routine is to get led stretch duration
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       duration  - LED stretch duration
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledPulseStretchGet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
OUT  PERIDOT_MSD_LED_STRETCH_DURATION    *duration
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	MSD_U16 val;
	MSD_DBG_INFO(("Peridot_gledPulseStretchGet Called.\n"));

	if (portNum > 10 || portNum == 0)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	retVal = Peridot_LedCtrl_Read(dev, portNum, 6, &tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	val = (tmp & 0x70) >> 4;
	switch (val)
	{
	case 0: *duration = PERIDOT_MSD_LED_STRETCH_DURATION_NO;
		break;
	case 1: *duration = PERIDOT_MSD_LED_STRETCH_DURATION_21_MS;
		break;
	case 2: *duration = PERIDOT_MSD_LED_STRETCH_DURATION_42_MS;
		break;
	case 3: *duration = PERIDOT_MSD_LED_STRETCH_DURATION_84_MS;
		break;
	case 4: *duration = PERIDOT_MSD_LED_STRETCH_DURATION_168_MS;
		break;

	default:
		MSD_DBG_ERROR(("Peridot_gledPulseStretchGet read error"));
		return MSD_FAIL;
	}

	MSD_DBG_INFO(("Peridot_gledPulseStretchGet Exit.\n"));
	return MSD_OK;
}


/*******************************************************************************
*   msdLedSpecialModeSet
*
* DESCRIPTION:
*       This routine is to set Special led portvect
*
*
* INPUTS:
*       devNum  - physical device number
*       mode  - Special led number
*		portVect - Special led bit mask
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
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledSpecialModeSet
(
IN  MSD_QD_DEV   *dev,
IN  PERIDOT_MSD_LED_SPECIAL_MODE	mode,
IN  MSD_U16  portVect
)
{
	MSD_STATUS retVal;
	MSD_U8 pointer;
	MSD_U8 foff;
	MSD_U8 flen;
	MSD_U16 tmp;

	MSD_DBG_INFO(("Peridot_gledSpecialModeSet Called.\n"));
	if (mode > PERIDOT_MSD_LED_SPECIAL_MODE_PTP)
	{
		MSD_DBG_INFO(("bad mode: only support LAN , WAN , PTP."));
		return MSD_BAD_PARAM;
	}
	else
	{
		pointer = (MSD_U8)mode+1;
		foff = 0;
		flen = 11;
	}
	if (portVect>0x7ff)
	{
		MSD_DBG_INFO(("bad portVect: only support [0,0x7ff]."));
		return MSD_BAD_PARAM;
	}
	retVal = Peridot_LedCtrl_Read(dev, 0,pointer, &tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_BF_SET(tmp, portVect, foff, flen);

	retVal = Peridot_LedCtrl_Write(dev, 0, pointer, tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Peridot_gledSpecialModeSet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
*   msdLedSpecialModeGet
*
* DESCRIPTION:
*       This routine is to get Special led portvect
*
*
* INPUTS:
*       devNum  - physical device number
*       mode  - Special led number
*
* OUTPUTS:
*		portVect - Special led bit mask
*
* RETURNS:

*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledSpecialModeGet
(
IN  MSD_QD_DEV   *dev,
IN  PERIDOT_MSD_LED_SPECIAL_MODE	mode,
OUT  MSD_U16	*portVect
)
{
	MSD_STATUS retVal;
	MSD_U8 pointer;
	MSD_U16 tmp;

	MSD_DBG_INFO(("Peridot_gledSpecialModeGet Called.\n"));
	if (mode > PERIDOT_MSD_LED_SPECIAL_MODE_PTP)
	{
		MSD_DBG_INFO(("bad mode: only support LAN , WAN , PTP."));
		return MSD_BAD_PARAM;
	}
	else
	{
		pointer = (MSD_U8)mode + 1;
	}
	retVal = Peridot_LedCtrl_Read(dev, 0, pointer, &tmp);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	*portVect = tmp & 0x7ff;

	MSD_DBG_INFO(("Peridot_gledSpecialModeGet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
*   msdLedCtrlSet
*
* DESCRIPTION:
*       This routine is to set led control register
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pointer - LED register pointer
*       data  - LED register data
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
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledCtrlSet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  MSD_U8 	pointer,
IN  MSD_U16	data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Peridot_gledCtrlSet Called.\n"));

	if (portNum > 10)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	if (pointer > 7)
	{
		MSD_DBG_ERROR(("Bad Pointer: %u. It should be within [0,7]\n", (unsigned int)pointer));
		return MSD_BAD_PARAM;
	}
	if (data > 0x7ff)
	{
		MSD_DBG_ERROR(("Bad Data: %u. It should be less than 11 bits\n", (unsigned int)data));
		return MSD_BAD_PARAM;
	}

	retVal = Peridot_LedCtrl_Write(dev, portNum, pointer, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Peridot_gledCtrlSet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
*   msdLedCtrlGet
*
* DESCRIPTION:
*       This routine is to get led control register
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pointer - LED register pointer
*
* OUTPUTS:
*       data  - LED register data
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Peridot:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Peridot_gledCtrlGet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  MSD_U8 	pointer,
OUT MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Peridot_gledCtrlSet Called.\n"));

	if (portNum > 10)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,10]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	if (pointer>7)
	{
		MSD_DBG_ERROR(("Bad Pointer: %u. It should be within [0,7]\n", (unsigned int)pointer));
		return MSD_BAD_PARAM;
	}
		
	retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	*data = *data & 0x7ff;
	MSD_DBG_INFO(("Peridot_gledCtrlSet Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Peridot_msdLedDump
*
* DESCRIPTION:
*       This routine is to get all led infomation
*
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS Peridot_gledDump
(
IN  MSD_QD_DEV   *dev
)
{
	MSD_STATUS retVal;
	MSD_U8 portNum = 0, pointer = 0;
	MSD_U16 data0, data6;
	MSD_DBG_INFO(("Peridot_gledDump Called.\n"));

	/* Port0 global control*/
	MSG(("\n--------- Port 0 Global Control ----------\n"));
    
	pointer = 0;
	retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("DirectLEDS      0x%-7x\n", (data0 & 0x10) >> 4));
	MSG(("SkipColumns     0x%-7x\n", (data0 & 0xF)));

	pointer = 1;
	retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("LAN Link/Act    0x%-7x\n", data0));

	pointer = 2;
	retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("WAN Link/Act    0x%-7x\n", data0));

	pointer = 3;
	retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("CPU Link/Act    0x%-7x\n", data0));

	pointer = 4;
	retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("PTP Activity    0x%-7x\n", data0));

	MSG(("------------------------------------------\n"));
	/* Print out led field*/
	MSG(("\n----------------------------------------------------------------\n"));
	MSG(("     LED1Select    LED0Select    PulseStretch    BlinkRate\n"));
	MSG(("----------------------------------------------------------------\n"));
	for (portNum = 1; portNum < dev->numOfPorts; portNum++)
	{
		pointer = 0;
		retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &data0);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}

		pointer = 6;
		retVal = Peridot_LedCtrl_Read(dev, portNum, pointer, &data6);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Peridot_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}

		MSG(("P%X     0x%-7x      0x% -7x     0x%-7x      0x%-7x \n", portNum, ((data0 & 0xF0) >> 4), (data0 & 0xF), ((data6 & 0x70) >> 4), (data6 & 0x7)));
	}


	MSD_DBG_INFO(("Peridot_gledDump Exit.\n"));
	return MSD_OK;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Peridot_LedCtrl_Write
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT  portNum,
IN	MSD_U8	pointer,
IN	MSD_U16	data
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U8        hwPort;         /* the physical port number     */
    MSD_U8		 phyAddr;
    MSD_U16   count, tmpData;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    count = 10;
    tmpData = 1;
    while (tmpData == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum, phyAddr, PERIDOT_QD_REG_LED_CONTROL, 15, 1, &tmpData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
        if ((count--) == 0)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return MSD_FAIL;
        }
    }

    tmpData = (MSD_U16)((1 << 15) | ((pointer & 0x7) << 12) | (data & 0x7FF));

    retVal = msdSetAnyReg(dev->devNum, phyAddr, PERIDOT_QD_REG_LED_CONTROL, tmpData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->tblRegsSem);
    return retVal;
}

static MSD_STATUS Peridot_LedCtrl_Read
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT  portNum,
IN	MSD_U8	pointer,
OUT	MSD_U16	*data
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16            tmpData;
    int count = 0x10;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    do {
        retVal = msdGetAnyReg(dev->devNum, phyAddr, PERIDOT_QD_REG_LED_CONTROL, &tmpData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
        if ((count--) == 0)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return MSD_FAIL;
        }
    } while (tmpData & 0x8000);

    tmpData = (MSD_U16)(((pointer & 0x7) << 12) | 0);
    retVal = msdSetAnyReg(dev->devNum, phyAddr, PERIDOT_QD_REG_LED_CONTROL, tmpData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

    retVal = msdGetAnyReg(dev->devNum, phyAddr, PERIDOT_QD_REG_LED_CONTROL, &tmpData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

    *data = (MSD_U16)(tmpData & 0x7ff);
    msdSemGive(dev->devNum, dev->tblRegsSem);

    return retVal;
}