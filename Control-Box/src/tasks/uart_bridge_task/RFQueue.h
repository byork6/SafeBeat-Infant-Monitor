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

#ifndef RFQUEUE_H
#define RFQUEUE_H

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_data_entry.h)

/* The Data Entries API */
// typedef struct {
//     uint8_t* pCurrEntry;
//     uint8_t* pLastEntry;
// } dataQueue_t;

/*
 * Size of the queue data entries (in bytes)
 * Entry size = 2 bytes of entry length + entry header + entry data
 */
#define RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(numEntries, dataSize, appendedBytes) \
    ((numEntries) * (2 + sizeof(rfc_dataEntryGeneral_t) + (dataSize) + (appendedBytes)))

/*
 * Defines the data queue
 *
 * Input:   pQueue      - Pointer to queue
 *          pBuffer     - Pointer to allocated buffer
 *          bufferSize  - Buffer size
 *          numEntries  - Number of entries
 *          entrySize   - Size of each entry
 * Return:  0 - Success, <0 - Error
 */
int8_t RFQueue_defineQueue(dataQueue_t *pQueue, uint8_t *pBuffer, uint16_t bufferSize, uint8_t numEntries, uint16_t entrySize);

/*
 * Get next entry pointer
 *
 * Input:   pQueue      - Pointer to queue
 * Return:  Pointer to next queue entry
 */
rfc_dataEntryGeneral_t* RFQueue_getNextEntry();

/*
 * Get data entry pointer
 *
 * Input:   pQueue      - Pointer to queue
 * Return:  Pointer to next queue entry
 */
rfc_dataEntryGeneral_t* RFQueue_getDataEntry();

/*
 * Move to next entry
 *
 * Input:   pQueue      - Pointer to queue
 * Return:  none
 */
void RFQueue_nextEntry();

#endif
