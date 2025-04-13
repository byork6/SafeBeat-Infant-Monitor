/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/***** Includes *****/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "RFQueue.h"

/***** Defines *****/

/***** Type declarations *****/

/***** Variable declarations *****/
static dataQueue_t* pDataQueue = NULL;

/***** Function definitions *****/
int8_t RFQueue_defineQueue(dataQueue_t *pQueue, uint8_t *pBuffer, uint16_t bufferSize, uint8_t numEntries, uint16_t entrySize)
{
    uint8_t i;
    uint16_t totalSize = 0;
    uint8_t *pEntryBuffer = pBuffer;
    rfc_dataEntry_t *pEntry;

    // Check input parameters
    if (pQueue == NULL || pBuffer == NULL || bufferSize == 0 || numEntries == 0 || entrySize == 0)
    {
        return -1;
    }

    // Calculate total size needed
    totalSize = numEntries * (entrySize + sizeof(rfc_dataEntry_t));

    // Check if buffer is large enough
    if (totalSize > bufferSize)
    {
        return -1;
    }

    // Set the data queue
    pDataQueue = pQueue;

    // Initialize the data queue
    pDataQueue->pCurrEntry = pEntryBuffer;
    pDataQueue->pLastEntry = NULL;

    // Initialize the data entries
    for (i = 0; i < numEntries; i++)
    {
        pEntry = (rfc_dataEntry_t *)pEntryBuffer;

        // Clear the data entry
        memset(pEntry, 0, sizeof(rfc_dataEntry_t) + entrySize);

        // Set data entry fields
        pEntry->status = DATA_ENTRY_PENDING;
        pEntry->config.type = DATA_ENTRY_TYPE_GEN;
        pEntry->config.lenSz = 0;
        pEntry->length = entrySize;

        // Set next pointer to next entry if not the last entry
        if (i == numEntries - 1)
        {
            pEntry->pNextEntry = pBuffer;
        }
        else
        {
            pEntry->pNextEntry = (uint8_t *)(pEntry) + sizeof(rfc_dataEntry_t) + entrySize;
        }

        // Move to next entry
        pEntryBuffer = (uint8_t *)pEntry->pNextEntry;
    }

    return 0;
}

rfc_dataEntryGeneral_t* RFQueue_getNextEntry()
{
    if (pDataQueue == NULL)
    {
        return NULL;
    }

    return (rfc_dataEntryGeneral_t*)pDataQueue->pCurrEntry;
}

rfc_dataEntryGeneral_t* RFQueue_getDataEntry()
{
    if (pDataQueue == NULL)
    {
        return NULL;
    }

    return (rfc_dataEntryGeneral_t*)pDataQueue->pCurrEntry;
}

void RFQueue_nextEntry()
{
    if (pDataQueue == NULL)
    {
        return;
    }

    // Move to next entry
    pDataQueue->pCurrEntry = ((rfc_dataEntry_t *)pDataQueue->pCurrEntry)->pNextEntry;
}
