#include "os_includes.h"
#include "status.h"
#include "global.h"
#include "HugeBuf.h"
#include "miscellance.h"
#include "CommPipe.h"

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)

#define CACHE_BUFF_SIZE 0x1000
#define PIPEHEAD_SIZE   4
#define ALIGN_SIZE  4
#define MAX_CMD_SIZE 11//(USBEndpointDescriptor_MAXBULKSIZE_HS - PIPEHEAD_SIZE)

//all command will align to 4byte edge.
static INT32U CacheBuff_INT[(CACHE_BUFF_SIZE + MAX_CMD_SIZE)/sizeof(INT32U)] SLOW_DATA_SECTION;// compiler assure that CacheBuff_INT is 4byte-aligned.
static INT8U* pRec, *pSend,*CacheBuff;
static INT16S DataSize, BufferSize;
static INT8U bIsCommRun;
static INT8U bIsInit = False;
//AT91PS_UDPHS_DMA * pDMA_nextBuf_DESC;

void CommPipe_USBWriteCallback (void *pArg,
		unsigned char status,
		unsigned int transferred,
		unsigned int remaining);

INT8U InitCommPipe(INT8U channelNumber)
{
	pRec = pSend = CacheBuff = (INT8U*)CacheBuff_INT;
	DataSize = 0;
	BufferSize = CACHE_BUFF_SIZE - sizeof(INT8U);
	bIsCommRun = False;

	USBD_AbortDataWrite(USB_IN_EP);

	bIsInit = True;
	return True;
}

void CommPipe_StartOneSendEP6()
{
	OS_CPU_SR cpu_sr;

	OS_ENTER_CRITICAL();
	if(DataSize > 0)
	{
		bIsCommRun = True;
		USBD_Write( USB_IN_EP, pSend, *(INT16U*)pSend, CommPipe_USBWriteCallback, 0);
	}
	else
		bIsCommRun = False;
	OS_EXIT_CRITICAL();	
}

void CommPipe_CompleteOneSendEP6()
{
	OS_CPU_SR cpu_sr;
	INT8U err;

	OS_ENTER_CRITICAL();	
	BufferSize += *(INT16U*)pSend;
	DataSize -= *(INT16U*)pSend;
	pSend += *(INT16U*)pSend;
	if(pSend >= CacheBuff+CACHE_BUFF_SIZE)
		pSend -= CACHE_BUFF_SIZE;
	bIsCommRun = False;
	OSFlagPost(mix_FLAG_GRP, COMM_PIPE_COMPLETED,OS_FLAG_SET, &err);
	OS_EXIT_CRITICAL();	
}

void CommPipe_USBDMA_EP6_DoneInterrupt(void)
{	
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();	
	CommPipe_CompleteOneSendEP6();

	CommPipe_StartOneSendEP6();			
	OS_EXIT_CRITICAL();
}


void CommPipe_USBWriteCallback (void *pArg,
		unsigned char status,
		unsigned int transferred,
		unsigned int remaining)
{
	//if(status == USBD_STATUS_SUCCESS)
	{
		CommPipe_USBDMA_EP6_DoneInterrupt();
	}
	//else
	bIsCommRun = False;

}

INT8U PushCommPipeData(INT8U channelNO, INT8U *pBuff, INT16U size, INT8U IsBlockModel)
{
	OS_CPU_SR  cpu_sr;
	INT8U err;
	INT8U ret = True;
	INT16U full_len = (size + ALIGN_SIZE -1)/ALIGN_SIZE*ALIGN_SIZE + PIPEHEAD_SIZE;
	INT16U data_len = (size + ALIGN_SIZE -1)/ALIGN_SIZE*ALIGN_SIZE;
	INT8U *pBufEnd = CacheBuff+CACHE_BUFF_SIZE;

	if(!bIsInit)
		return False;

	if(size > MAX_CMD_SIZE)
		return False;

	if(!IS_ONLINE) 
	{
		if(bIsCommRun)
		{
		}
		InitCommPipe(3);
		return False;
	}

	do{
		OS_ENTER_CRITICAL();
		if(BufferSize < full_len)
		{
			ret = False;
		}
		else
		{
			ret = True;
			*(INT16U*)pRec = (INT16U)full_len;
			*(INT16U*)(pRec + sizeof(INT16U))= (INT16U)channelNO;
			pRec += PIPEHEAD_SIZE;
			if(pRec == CacheBuff+CACHE_BUFF_SIZE)
				pRec = CacheBuff;
			memcpy(pRec, pBuff, size);
			if(pRec + data_len >= pBufEnd)
			{
				if(pRec + data_len > pBufEnd)
					memcpy(CacheBuff, pBuff + (pBufEnd - pRec), size - (pBufEnd - pRec));
				pRec = pRec + data_len - CACHE_BUFF_SIZE;
			}
			else
				pRec += data_len;
			BufferSize -= full_len;
			DataSize += full_len;
			if(bIsCommRun == False)
			{
				CommPipe_StartOneSendEP6();			
			}
		}
		OS_EXIT_CRITICAL();	

		if(!ret && IsBlockModel)
		{
			OSFlagPend(mix_FLAG_GRP, COMM_PIPE_COMPLETED, OS_FLAG_WAIT_SET_ANY | OS_FLAG_CONSUME, 0, &err);
		}
		else
			break;
	}while(1);

	return ret;
}

#endif
