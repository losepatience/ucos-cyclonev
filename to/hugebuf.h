#ifndef _HUGEBUF_H_
#define _HUGEBUF_H_
#include "board.h"
#include "job.h"

extern INT32U BOARD_DDRAM_SIZE  ;

#define RESERVED_MEMSIZE   0x200000
#define HUGEBUF_START_ADDR (AT91C_DDR2 + RESERVED_MEMSIZE)
#ifdef RECORD_MOTION_MOVEMENT
#define HUGEBUF_END_ADDR   (AT91C_DDR2 + (BOARD_DDRAM_SIZE/2) - RESERVED_MEMSIZE)
#else
#define HUGEBUF_END_ADDR   (AT91C_DDR2 + BOARD_DDRAM_SIZE - RESERVED_MEMSIZE)
#endif
#define HUGEBUF_SIZE 	(HUGEBUF_END_ADDR - HUGEBUF_START_ADDR)  //0x3C00000, 60M Bytes

//cheney: FPGA DMA use DAM Channel 1.
//cheney: USB DMA use itself private DMA channel.
#define FPGA_DMA_CHANNEL_NUM DMA_CHANNEL_1
#define USB_OUT_EP  2
#define USB_IN_EP  6
#define FPGADMA_WORKING ((DMA_GetChannelStatus() & (DMA_ENA << FPGA_DMA_CHANNEL_NUM)) == (DMA_ENA << FPGA_DMA_CHANNEL_NUM))
#define USBDMA_WORKING ((AT91C_BASE_UDPHS->UDPHS_DMA[USB_OUT_EP].UDPHS_DMASTATUS & AT91C_UDPHS_CHANN_ENB) == AT91C_UDPHS_CHANN_ENB)

//for FPGA DMA
#define DMA_TRANSFER_WIDTH 2
#define DMA_TRANSFER_WIDTH_SETTING 1 //half-word

//For burst mode
#define FPGADMA_TRANS_COUNT 		512
#define FPGADMA_BYTES_PER_TRANS	2
#define HUGEBUF_ALIGN_SIZE (FPGADMA_TRANS_COUNT*FPGADMA_BYTES_PER_TRANS) //1024 bytes, Huge buffer operation unit, must be 4 bytes aligned

void HugBuf_InitReceiveSize();

void HugeBuf_StartOneReceive(void);//Start one SX2 DMA1 transfer to memory
void HugeBuf_CompleteOneReceive(void);
void HugeBuf_StartOneSendEP6(void); 
void HugeBuf_CompleteOneSendEP6(void);

INT8U HugeBuf_GetInfHead(DataHeaderType *header);
INT32S HugeBuf_GetSize(void);
void HugeBuf_CancelJob(void);
INT16U * HugeBuf_GetCodeAddr(INT32S len);
void HugeBuf_FlushCurBand(void);
void HugeBuf_Reset(void);

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
INT8U* HugeBuf_GetRecAddr(void);
void HugeBuf_PutSize(INT32U Size);
#endif

#endif
