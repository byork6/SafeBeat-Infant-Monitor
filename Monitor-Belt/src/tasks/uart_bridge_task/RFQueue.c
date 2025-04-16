/* Application Header files */ 
#include "../../common/common.h"
#include DeviceFamily_constructPath(driverlib/rf_data_entry.h)

/* Receive entry pointer to keep track of read items */
rfc_dataEntryGeneral_t* readEntry;

//*****************************************************************************
//
//! Get the current dataEntry
//!
//! \return rfc_dataEntry*
//
//*****************************************************************************
rfc_dataEntryGeneral_t*
RFQueue_getDataEntry()
{
  return (readEntry);
}

//*****************************************************************************
//
//! Move to next dataEntry
//!
//! \return None
//
//*****************************************************************************
uint8_t
RFQueue_nextEntry()
{
  /* Set status to pending */
  readEntry->status = DATA_ENTRY_PENDING;

  /* Move read entry pointer to next entry */
  readEntry = (rfc_dataEntryGeneral_t*)readEntry->pNextEntry;

  return (readEntry->status);
}

//*****************************************************************************
//
//! Define a queue
//!
//! \param dataQueue is a pointer to the queue to use
//! \param buf is the prealocated byte buffer to use
//! \param buf_len is the number of preallocated bytes
//! \param numEntries are the number of dataEntries to split the buffer into
//! \param length is the length of data in every dataEntry
//!
//! \return uint8_t
//
//*****************************************************************************
uint8_t
RFQueue_defineQueue(dataQueue_t *dataQueue, uint8_t *buf, uint16_t buf_len, uint8_t numEntries, uint16_t length)
{

  if (buf_len < (numEntries * (length + RF_QUEUE_DATA_ENTRY_HEADER_SIZE + RF_QUEUE_QUEUE_ALIGN_PADDING(length))))
  {
    /* queue does not fit into buffer */
    return (1);
  }

  /* Padding needed for 4-byte alignment? */
  uint8_t pad = 4-((length + RF_QUEUE_DATA_ENTRY_HEADER_SIZE)%4);

  /* Set the Data Entries common configuration */
  uint8_t *first_entry = buf;
  int i;
  for (i = 0; i < numEntries; i++)
  {
    buf = first_entry + i * (RF_QUEUE_DATA_ENTRY_HEADER_SIZE + length + pad);
    ((rfc_dataEntry_t*)buf)->status        = DATA_ENTRY_PENDING;        // Pending - starting state
    ((rfc_dataEntry_t*)buf)->config.type   = DATA_ENTRY_TYPE_GEN;       // General Data Entry
    ((rfc_dataEntry_t*)buf)->config.lenSz  = 0;                         // No length indicator byte in data
    ((rfc_dataEntry_t*)buf)->length        = length;                    // Total length of data field

    ((rfc_dataEntryGeneral_t*)buf)->pNextEntry = &(((rfc_dataEntryGeneral_t*)buf)->data)+length+pad;
  }
  /* Make circular Last.Next -> First */
  ((rfc_dataEntry_t*)buf)->pNextEntry = first_entry;

  /* Create Data Entry Queue and configure for circular buffer Data Entries */
  dataQueue->pCurrEntry = first_entry;
  dataQueue->pLastEntry = NULL;

  /* Set read pointer to first entry */
  readEntry = (rfc_dataEntryGeneral_t*)first_entry;

  return (0);
}
