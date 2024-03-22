/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* Oak_msdApiInternal.h
*
* DESCRIPTION:
*       API Prototypes for Switch Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef Oak_msdApiInternal_h
#define Oak_msdApiInternal_h

#ifdef __cplusplus
extern "C" {
#endif


/* This macro checks for an broadcast mac address     */
#define OAK_IS_BROADCAST_MAC(mac) (((mac).arEther[0] == 0xFF) && ((mac).arEther[1] == 0xFF) && ((mac).arEther[2] == 0xFF) && ((mac).arEther[3] == 0xFF) && ((mac).arEther[4] == 0xFF) && ((mac).arEther[5] == 0xFF))

/* definition for Trunking */
#define OAK_IS_TRUNK_ID_VALID(_dev, _id)    (((_id) < 32U) ? 1 : 0)


/* Convert port number to smi device address */
#define OAK_MSD_CALC_SMI_DEV_ADDR(_dev, _portNum)  \
	((_dev)->baseRegAddr + (_portNum))
/*
 * Typedef: enum MSD_ATU_OPERARION
 *
 * Description: Defines the different ATU and VTU operations
 *
 * Fields:
 *      FLUSH_ALL           - Flush all entries.
 *      FLUSH_UNLOCKED      - Flush all unlocked entries in ATU.
 *      LOAD_PURGE_ENTRY    - Load / Purge entry.
 *      GET_NEXT_ENTRY      - Get next ATU or VTU entry.
 *      FLUSH_ALL_IN_DB     - Flush all entries in a particular DBNum.
 *      FLUSH_UNLOCKED_IN_DB - Flush all unlocked entries in a particular DBNum.
 *      SERVICE_VIOLATONS   - sevice violations of VTU
 *
 */
typedef enum
{
    OAK_FLUSH_ALL = 1,        /* for both atu and vtu */
    OAK_FLUSH_NONSTATIC,        /* for atu only */
    OAK_LOAD_PURGE_ENTRY,    /* for both atu and vtu */
    OAK_GET_NEXT_ENTRY,        /* for both atu and vtu */
    OAK_FLUSH_ALL_IN_DB,    /* for atu only */
    OAK_FLUSH_NONSTATIC_IN_DB,    /* for atu only */
    OAK_SERVICE_VIOLATIONS     /* for vtu only */
} OAK_MSD_ATU_OPERATION, OAK_MSD_VTU_OPERATION;

/*
* Define max num of queues
*/
#define OAK_MAX_NUM_OF_QUEUES 8U

#ifdef __cplusplus
}
#endif

#endif /* __Oak_msApiInternal_h */