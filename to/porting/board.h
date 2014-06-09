/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
/// \dir
/// !Purpose
///
/// Definition and functions for using AT91SAM9G45-related features, such
/// has PIO pins, memories, etc.
///
/// !Usage
/// -# The code for booting the board is provided by board_cstartup.S and
///    board_lowlevel.c.
/// -# For using board PIOs, board characteristics (clock, etc.) and external
///    components, see board.h.
/// -# For manipulating memories (remapping, SDRAM, etc.), see board_memories.h.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \unit
/// !Purpose
///
/// Definition of AT91SAM9G45-EK characteristics, AT91SAM9G45-dependant PIOs and
/// external components interfacing.
///
/// !Usage
/// -# For operating frequency information, see "SAM9G45-EK - Operating frequencies".
/// -# For using portable PIO definitions, see "SAM9G45-EK - PIO definitions".
/// -# Several USB definitions are included here (see "SAM9G45-EK - USB device").
/// -# For external components definitions, see "SAM9G45-EK - External components".
/// -# For memory-related definitions, see "SAM79260-EK - Memories".
//------------------------------------------------------------------------------

#ifndef BOARD_H
#define BOARD_H


//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#ifdef BYHX_ASystem_Core 
#ifndef __ASSEMBLY__
#include "printers.h"
#endif
#endif
//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/*XXX to fix*/
#define FAST_DATA_SECTION
#define FAST_FUNC_SECTION
#define SLOW_DATA_SECTION

//------------------------------------------------------------------------------
/// \page "SAM9G45-EK - Board Description"
/// This page lists several definition related to the board description.
///
/// !Definitions
/// - BOARD_NAME

/// Name of the board.
#define BOARD_NAME              "BYHX-ASystem-Core"
#ifndef BYHX_ASystem_CoreBoard
/// Board definition.
#define BYHX_ASystem_CoreBoard
#endif

//#define HW_TO_BE_DEFINED

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM9G45-EK - Operating frequencies"
/// This page lists several definition related to the board operating frequency
/// (when using the initialization done by board_lowlevel.c).
///
/// !Definitions
/// - BOARD_MAINOSC
/// - BOARD_MCK

/// Frequency of the board main oscillator.
#define BOARD_MAINOSC           12000000

/// Master clock frequency (when using board_lowlevel.c).
// ((12MHz / DIVA / PLLADIV2 / MDIV) * (MULA+1)
//#define BOARD_MCK               ((unsigned long)((BOARD_MAINOSC / 3 / 2 / 3) * 200 )) // 133MHz
#define BOARD_MCK               ((unsigned long)((BOARD_MAINOSC / 3 / 2 *200 /3))) // 133MHz
//#define BOARD_MCK               ((unsigned long)((BOARD_MAINOSC / 3 / 2 / 4) * 200 )) // 100MHz

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// IRQ PRI. definitions
#define IRQ_PRI_UDPHS 5
#define IRQ_PRI_US3 1
#define IRQ_PRI_US2 1
#define IRQ_PRI_SYSC 4
#define IRQ_PRI_TWI0 2
#define IRQ_PRI_TWI1 2
#define IRQ_PRI_PORT 3
#ifdef EPSON_4H
#define IRQ_PRI_PWM 6
#define IRQ_PRI_TC 3
#define IRQ_PRI_DMA 7
#else
#define IRQ_PRI_PWM 7
#define IRQ_PRI_TC 3
#define IRQ_PRI_DMA 6
#endif

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// \page "SAM9RL-EK - USB device"
/// This page lists constants describing several characteristics (controller
/// type, D+ pull-up type, etc.) of the USB device controller of the chip/board.
///
/// !Constants
/// - BOARD_USB_UDPHS
/// - BOARD_USB_PULLUP_INTERNAL
/// - BOARD_USB_NUMENDPOINTS
/// - BOARD_USB_ENDPOINTS_MAXPACKETSIZE
/// - BOARD_USB_ENDPOINTS_BANKS
/// - BOARD_USB_BMATTRIBUTES

/// Chip has a UDPHS controller.
#define BOARD_USB_UDPHS

/// Indicates the D+ pull-up is internal to the USB controller.
#define BOARD_USB_PULLUP_INTERNAL

/// Number of endpoints in the USB controller.
#define BOARD_USB_NUMENDPOINTS                  7

/// Returns the maximum packet size of the given endpoint.
#define BOARD_USB_ENDPOINTS_MAXPACKETSIZE(i)    ((i == 0) ? 64 : 1024)

/// Returns the number of FIFO banks for the given endpoint.
#define BOARD_USB_ENDPOINTS_BANKS(i)            ((i == 0) ? 1 : ((i == 1) || (i == 2)) ? 2 : 3)

/// USB attributes configuration descriptor (bus or self powered, remote wakeup)
//#define BOARD_USB_BMATTRIBUTES                  USBConfigurationDescriptor_SELFPOWERED_RWAKEUP
#define BOARD_USB_BMATTRIBUTES                  USBConfigurationDescriptor_SELFPOWERED_NORWAKEUP
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM9G45-EK - PIO definitions"
/// This pages lists all the pio definitions contained in board.h. The constants
/// are named using the following convention: PIN_* for a constant which defines
/// a single Pin instance (but may include several PIOs sharing the same
/// controller), and PINS_* for a list of Pin instances.
///
/// !DBGU
/// - PINS_DBGU
///
/// !LEDs
/// - PIN_LED_0
/// - PIN_LED_1
/// - PINS_LEDS
/// - LED_POWER
/// - LED_DS1
///
/// !Push buttons
/// - PIN_PUSHBUTTON_1
/// - PIN_PUSHBUTTON_2
/// - PINS_PUSHBUTTONS
/// - PUSHBUTTON_BP1
/// - PUSHBUTTON_BP2
/// - JOYSTICK_LEFT
/// - JOYSTICK_RIGHT
///
/// !USART0
/// - PIN_USART0_RXD
/// - PIN_USART0_TXD
///
/// !SPI0
/// - PIN_SPI0_MISO
/// - PIN_SPI0_MOSI
/// - PIN_SPI0_SPCK
/// - PINS_SPI0
/// - PIN_SPI0_NPCS0
/// - PIN_SPI0_NPCS1
///
/// !SSC
/// - PINS_SSC_TX
///
/// !USB
/// - PIN_USB_VBUS
///
/// !MCI
/// - PINS_MCI
///
/// !TWI
/// - PINS_TWI0
///
/// !TSADC
/// - PINS_TSADCC

#if defined( EPSON_BOTTOM_BOARD_V3 ) || defined( EPSON_BOTTOM_BOARD_V2_1 )
#define PINS_DBGU  {0, 0, 0, 0, 0}

#if defined(TRACE_DBGU) || defined(TRACE_USART_1) || defined(TRACE_USART_2)
#error "DebugTrace need TXD, but it is used for EPSON_BOTTOM_BOARD_V3"
#endif

#else
/// List of all DBGU pin definitions.
#define PINS_DBGU  {(1<<12) | (1<<13), AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#endif

//LED default state is dark.
/// LED #0 pin definition.
#define PIN_LED_0   {1 << 31, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
/// LED #1 pin definition.
#define PIN_LED_1   {1 << 30, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
/// LED #0 pin definition.
#define PIN_LED_2   {1 << 29, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
/// LED #1 pin definition.
#define PIN_LED_3   {1 << 28, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
/// List of all LED definitions.
#define PINS_LEDS   PIN_LED_0, PIN_LED_1, PIN_LED_2, PIN_LED_3
#define PINS_LEDS_NUM 4
/// Dark/Light LED.
#define LIGHT_LED(pin)  PIO_Clear(pin)
#define DARK_LED(pin)   PIO_Set(pin)
#define LED_IS_LIGHT(pin) (PIO_Get(pin)==0)
#define LED_IS_DRAK(pin) (PIO_Get(pin)==1)

// when LED_0 is light, BOOT is running. After it run, the LED_0 is dark.
// If LED_0 is twinkling, it happen an error, and LED1-3 means an ERROR CODE.
/// Boot-Run LED index.
#define LED_BOOT_RUN 0  
#define LED_D15      0
#define LED_D16      1
#define LED_D17      2
#define LED_D18      3

#define PIN_GENERIC_INPUT {1 << 11, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}

/// FPGA PIN definition
/// FPGA Configure PINs
#define BOARD_PINS_FPGA_CONFIG  PIN_FPGA_CONF_DONE, PIN_FPGA_nSTATUS, \
  PIN_FPGA_DATA0, PIN_FPGA_nCONFIG, PIN_FPGA_DCLK
#define PIN_FPGA_CONF_DONE  {1 << 15, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#define PIN_FPGA_nSTATUS    {1 << 16, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#define PIN_FPGA_DATA0      {1 << 17, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_FPGA_nCONFIG    {1 << 29, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_FPGA_DCLK       {1 << 30, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}

#ifdef HEAD_XAAR382
#define PIN_FPGA_ARM_YPULSE     {1 << 24, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#else
#define PIN_FPGA_ARM_YPULSE     {1 << 24, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#endif
#if defined( EPSON_BOTTOM_BOARD_V3) || defined(FPGA_MAINTAIN_COOR) || defined( EPSON_BOTTOM_BOARD_V2_1 )|| defined(EPSON_PRT_FLASH)
/// FPGA Expand IO PINs
#define BOARD_PINS_FPGA_EXPAND  PIN_FPGA_POSITION_IRQ, PIN_FPGA_EXPAND_IO_IRQ, PIN_FPGA_ARM_YPULSE
#define PIN_FPGA_POSITION_IRQ   {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#define PIN_FPGA_EXPAND_IO_IRQ  {1 << 22, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#else
/// FPGA Motion Status PINs
#define BOARD_PINS_FPGA_MOTION  PIN_FPGA_ARM_XDIR, PIN_FPGA_ARM_XPULSE, PIN_FPGA_ARM_YDIR, PIN_FPGA_ARM_YPULSE
#define PIN_FPGA_ARM_XDIR   {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#define PIN_FPGA_ARM_XPULSE {1 << 22, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#define PIN_FPGA_ARM_YDIR   {1 << 23, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}
/// FPGA Debug PINs
#define BOARD_PINS_FPGA_DEBUG   PIN_FPGA_DEBUG0, PIN_FPGA_DEBUG1
#define PIN_FPGA_DEBUG0     PIN_FPGA_ARM_XDIR
#define PIN_FPGA_DEBUG1     PIN_FPGA_nIRQ
#endif
/// FPGA Clock Input PINs
#define BOARD_PINS_FPGA_CLOCK   PIN_FPGA_CLKOUT0, PIN_FPGA_CLKOUT1
#define PIN_FPGA_CLKOUT0    {(unsigned int)1 << 31, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_FPGA_CLKOUT1    {(unsigned int)1 << 31, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/// FPGA EBI Data Bus PINs
#define BOARD_PINS_FPGA_EBI_BUS   PIN_FPGA_nCS, PIN_FPGA_nXDREQ, /*PIN_FPGA_nFGPA_RDY, */ PIN_FPGA_nRST, PIN_FPGA_nHALFEMPTY
#define PIN_FPGA_nCS      {1 << 11, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_FPGA_nXDREQ     {1 << 19, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT}
//XDACK, FPGA doesn't implement it. So we use this pin as half empty.
//#define PIN_FPGA_nXDACK     {1 << 27, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
//cheney: PIN_FPGA_nFGPA_RDY is used as UV control signal from DSP , I remove it from BOARD_PINS_FPGA_EBI_BUS.
//#define PIN_FPGA_nFGPA_RDY  {1 << 15, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_FPGA_nRST       {1 << 29, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_FPGA_nHALFEMPTY {1 << 27, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT} 
/// FPGA nFIRE Pin
#define PIN_FPGA_nFIRE      {1 << 28, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
/// FPGA nIRQ_FPGA Pin
#define PIN_FPGA_nIRQ       {1 << 18, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT} 


/// DSP PIN definition
/// USART3 TXD pin definition.
#define PIN_USART3_TXD  {1 << 8, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// USART3 RXD pin definition.
#define PIN_USART3_RXD  {1 << 9, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define BOARD_PINS_DSP_USART        PIN_USART3_TXD, PIN_USART3_RXD
#define BOARD_DSP_USART_BASE        AT91C_BASE_US3
#define BOARD_ID_DSP_USART          AT91C_ID_US3
///DSP External IRQ pin. For UV_PRINT case, it use as UV_CONTROL.
#define PIN_DSP_EINT     {1 << 6, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_DSP_UV_CONTROL  PIN_DSP_UV_CONTROL_1, PIN_DSP_UV_CONTROL_2
#define PIN_DSP_UV_CONTROL_1  PIN_DSP_EINT
#define PIN_DSP_UV_CONTROL_2  {1 << 15, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_DEFAULT}

/// LVDS&Head Board PIN definition
#define PIN_LVDS_LOCK_EINT     {1 << 26, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_PULLUP}
/// USART2 TXD pin definition.
#define PIN_USART2_TXD  {1 << 6, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// USART2 RXD pin definition.
#define PIN_USART2_RXD  {1 << 7, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define BOARD_PINS_HEADBOARD_USART        PIN_USART2_TXD, PIN_USART2_RXD
#define BOARD_HEADBOARD_USART_BASE        AT91C_BASE_US2
#define BOARD_ID_HEADBOARD_USART          AT91C_ID_US2

/// Output pin to headboard
#define PIN_HEADBOARD_OUT     {(unsigned int)0x1 << 31, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
/// Input pin from headboard. total 4 pins.
#define BOARD_PINS_HEADBOARD_IN     {0x78000000, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}


//common input pins
#define BOARD_PINS_GIN PIN_GIN_1, PIN_GIN_2
#define PIN_GIN_1 {1 << 11, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_GIN_2 {1 << 30, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}

/// Mother board interface PIN definition.
/// it has two type definitions, one is as normal ports, the another is as peripheral ports.
/// here, only list peripheral definitions
#if defined( EPSON_BOTTOM_BOARD_V3 ) || defined( EPSON_BOTTOM_BOARD_V2_1 )
/// PWM PINs
#define BOARD_PINS_MB_PWM PIN_PWM0, PIN_PWM1, PIN_PWM2, PIN_PWM3
#define PIN_PWM0  {1 << 24, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_PWM1  {1 << 25, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_PWM2  {1 << 31, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_PWM3  {1 << 25, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define BOARD_PINS_MB_PWM_INIT PIN_PWM0_INIT, PIN_PWM1_INIT, PIN_PWM2_INIT, PIN_PWM3_INIT
#define PIN_PWM0_INIT  {1 << 24, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PWM1_INIT  {1 << 25, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PWM2_INIT  {1 << 31, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PWM3_INIT  {1 << 25, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define BOARD_PINS_MB_DC_CTRL   PIN_DC_1_CTRL_IO, PIN_DC_2_CTRL_IO
#define PIN_DC_1_CTRL_IO    {1 << 13, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT} //keep low to reset DC driver chip 8412
#define PIN_DC_2_CTRL_IO    {1 << 16, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT} //keep low to reset DC driver chip 8412
#define BOARD_PINS_MB_DC_STAT   PIN_DC_OTW, PIN_DC_FAULT
#define PIN_DC_FAULT    {1 << 26, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} //keep low to reset DC driver chip 8412
#define PIN_DC_OTW    {1 << 27, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} //keep low to reset DC driver chip 8412
//#define PIN_DC_OTW			{1 << 5, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT} 
//#define PIN_DC_FAULT		{1 <<12, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT} 

#endif

/// USART PINs
/// USART0 PINs
#define BOARD_PINS_MB_USART0  PIN_USART0_TXD, PIN_USART0_RXD
#define PIN_USART0_TXD  {1 << 19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// USART0 RXD pin definition.
#define PIN_USART0_RXD  {1 << 18, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// USART1 PINs
#define BOARD_PINS_MB_USART1_FULL  PIN_USART1_TXD, PIN_USART1_RXD, PIN_USART1_RTS, PIN_USART1_CTS
#define BOARD_PINS_MB_USART1  PIN_USART1_TXD, PIN_USART1_RXD
/// USART1 TXD pin definition.
#define PIN_USART1_TXD  {1 <<  4, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// USART1 RXD pin definition.
#define PIN_USART1_RXD  {1 <<  5, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// USART1 RTS pin definition.
#define PIN_USART1_RTS  {1 << 16, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT}
/// USART1 CTS pin definition.
#define PIN_USART1_CTS  {1 << 17, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT}
/// USART2 PINs, share HEADBOARD USART
#define BOARD_PINS_MB_USART2  BOARD_PINS_HEADBOARD_USART
/// DBG Unit port.
#define BOARD_PINS_MB_DBGU PINS_DBGU
/// TWI PINs
#define BOARD_PINS_MB_TWI1 PIN_TWD1, PIN_TWCK1
#define PIN_TWD1  {1 << 10, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_TWCK1 {1 << 11, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}

#if !(defined(EPSON_BOTTOM_BOARD_V3) || defined( EPSON_BOTTOM_BOARD_V2_1 ))
/// ANALOG Input PINs
#define BOARD_PINS_MB_ANALOG_IN PIN_GPAD5, PIN_GPAD6, PIN_GPAD7
/// PIN_GPAD4 share PWM0. 
/// Here, we configure them as normal input ports, 
///   ANALOG Input feature is enabled by TSADCC_CHER, Not PIO Controller.
#ifndef UV_PRINTER
#define PIN_GPAD4 {1 << 24, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
#endif
#define PIN_GPAD5 {1 << 25, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
#define PIN_GPAD6 {1 << 26, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
#define PIN_GPAD7 {1 << 27, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
#endif
/// TC PINs, already used as UV ports. see customized port.
#define BOARD_PINS_MB_TC PIN_CLK3, PIN_CLK4, PIN_CLK5, PIN_TIOA3, PIN_TIOB3, \
    PIN_TIOA4, PIN_TIOB4, PIN_TIOA5, PIN_TIOB5 
#define PIN_CLK3  {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_CLK4  {1 << 3, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_CLK5  {1 << 9, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT} 
#define PIN_TIOA3 {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_TIOB3 {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_TIOA4 {1 << 4, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_TIOB4 {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_TIOA5 {1 << 7, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT} 
#define PIN_TIOB5 {1 << 8, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_B, PIO_DEFAULT} 
/// Touch Panel PINs
#define BOARD_PINS_MB_TOUCHPANEL PIN_TSAD0, PIN_TSAD1, PIN_TSAD2, PIN_TSAD3
#define PIN_TSADTRG  {1 << 28, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT} 
/// Here, we configure them as normal input ports, 
///   ANALOG Input feature is enabled by TSADCC_CHER, Not PIO Controller.
#define PIN_TSAD0 {1 << 20, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
#define PIN_TSAD1 {1 << 21, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
#define PIN_TSAD2 {1 << 22, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
#define PIN_TSAD3 {1 << 23, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
/// SPI PINs
#define BOARD_PINS_MB_SPI PIN_SPI0_MISO, PIN_SPI0_MOSI, PIN_SPI0_SPCK, PIN_SPI0_NPCS0
#define PIN_SPI0_MISO   {1 << 0, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI0_MOSI   {1 << 1, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI0_SPCK   {1 << 2, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI0_NPCS0  {1 << 3, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
/// SPI Slave Device CS pin share the USART0
#define PIN_SPI0_NPCS1  {1 << 18, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_SPI0_NPCS2  {1 << 19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/// LCD PINs
#define BOARD_PINS_MB_LCD PIN_LCD_DATABUS24, PIN_LCD_CONTROL
/// LCD Data Bus, two bus width: 16bits, or 24bits.
#define PIN_LCD_DATABUS24  {0x7FFFFF80, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_LCD_DATABUS16  {0x7C7E7C00, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT}
/// LCD Control PINs
#define PIN_LCD_CONTROL  {0x7F, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT}
/// SSC Port PINs, already used as UV ports. see customized port.
#define BOARD_PINS_MB_SSC PINS_SSC0
#define PINS_SSC0 {0x3F, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT} 
#define BOARD_PINS_MB_nXBACK PINS_PD15
#define PINS_PD15 {1 << 15, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT} 


/// Mother board interface customized PIN definition.
#if !(defined(EPSON_BOTTOM_BOARD_V3) || defined( EPSON_BOTTOM_BOARD_V2_1 ))
// use PWM0 as Vacuum-Cleaner control pin
#ifdef OLD_MOTHER_BOARD 
#define CLEANER_CLOSE_LEVEL PIO_OUTPUT_0
#else 
#define CLEANER_CLOSE_LEVEL PIO_OUTPUT_1
#endif
#define MB_CUST_PIN_CLEANER {1 << 24, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, CLEANER_CLOSE_LEVEL, PIO_DEFAULT}
#endif
// reserved output pin, connected with PIN_HEADBOARD_OUT, and share same pin with PIN_PWM1.
// please use PIN_HEADBOARD_OUT

#if defined( EPSON_BOTTOM_BOARD_V2 ) && defined(MICOLOR_AUTOFUCTION) && defined(UV_PRINTER)
#error "under EPSON_BOTTOM_BOARD_V2,  can't support MICOLOR_AUTOFUCTION and UV_PRINTER on the same time"
#endif

#if defined( EPSON_BOTTOM_BOARD_V2_1) && (defined(MICOLOR_AUTOFUCTION) || defined(UV_PRINTER))
#error "under EPSON_BOTTOM_BOARD_V2_1,  can't support MICOLOR_AUTOFUCTION or UV_PRINTER"
#endif

#if defined( EPSON_BOTTOM_BOARD_V3 ) ||defined( EPSON_BOTTOM_BOARD_V2_1 )
//TC part, for Stepper
#define MB_CUST_PINS_STEPPER PIN_STEPPER1_RESET, PIN_STEPPER2_RESET, PIN_STEPPER1_PULSE_INIT, PIN_STEPPER1_DIR, \
    PIN_STEPPER2_PULSE_INIT, PIN_STEPPER2_DIR
#define PIN_STEPPER1_RESET  {1 << 4, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_STEPPER2_RESET  {1 << 5, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT} 
#ifdef EPSON_STEPPER_ERROR
#define PIN_STEPPER1_PULSE  PIN_TIOA3
#define PIN_STEPPER1_DIR    {1 << 4, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_STEPPER2_PULSE  PIN_TIOB3
#define PIN_STEPPER2_DIR    {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_STEPPER1_PULSE_INIT  {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_STEPPER2_PULSE_INIT  {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#else
#define PIN_STEPPER1_PULSE  PIN_TIOA3
#define PIN_STEPPER1_DIR    {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_STEPPER2_PULSE  PIN_TIOA4
#define PIN_STEPPER2_DIR    {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_STEPPER1_PULSE_INIT  {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_STEPPER2_PULSE_INIT  {1 << 4, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#endif

//all epson sensor is to get by FPGA expand IO.
#ifdef UV_PRINTER
//all UV sensor is to get by FPGA expand IO.
#define MB_CUST_PINS_UV_CTRL  PIN_UV_CTRL_LP1_ON, PIN_UV_CTRL_LP2_ON, PIN_UV_CTRL_LP1_HIGHPOWER, PIN_UV_CTRL_LP1_HIGHPOWER_1, \
  PIN_UV_CTRL_LP2_HIGHPOWER, PIN_UV_CTRL_LP2_HIGHPOWER_1, PIN_UV_CTRL_SHUTTER_1_OPEN, PIN_UV_CTRL_SHUTTER_2_OPEN
#define PIN_UV_CTRL_LP1_ON      {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_LP2_ON      {1 << 5, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_SHUTTER_1_OPEN  {1 << 23, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_SHUTTER_2_OPEN  {1 << 21, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_LP1_HIGHPOWER   {1 << 22, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_LP2_HIGHPOWER   {1 << 20, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_LP1_HIGHPOWER_1   {1 << 31, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_LP2_HIGHPOWER_1   {1 << 9, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#endif
#if 0
#elif defined( EPSON_BOTTOM_BOARD_V2 ) && defined(MICOLOR_AUTOFUCTION)
#define PIN_EPSON_SENSORS   PIN_EPSON_LACK_OF_MEDIA, PIN_EPSON_MEDIA_FIXED, PIN_EPSON_COVER, PIN_EPSON_LOAD_MEDIA
#define PIN_EPSON_LACK_OF_MEDIA     {(unsigned int)1 << 31, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_EPSON_MEDIA_FIXED       {1 << 25, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EPSON_COVER             {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EPSON_LOAD_MEDIA        {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#endif
#else

#if defined(MICOLOR_AUTOFUCTION)
#define PIN_EPSON_SENSORS   PIN_EPSON_LACK_OF_MEDIA, PIN_EPSON_MEDIA_FIXED, PIN_EPSON_COVER, PIN_EPSON_LOAD_MEDIA
#define PIN_EPSON_LACK_OF_MEDIA     {(unsigned int)1 << 31, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_EPSON_MEDIA_FIXED       {1 << 25, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EPSON_COVER             {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EPSON_LOAD_MEDIA        {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#endif

#ifdef SUPPORT_MOTOR_CONTROL_ONLY_STEP

#define PIN_STEPPER1_PULSE {1 << 20, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT} 

#define PIN_STEPPER1_DIR    {1 << 26, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}

#define PIN_STEPPER2_PULSE  PIN_TIOA3
#define PIN_STEPPER2_DIR {1 << 27, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}   

#define PIN_STEPPER1_PULSE_INIT  {1 << 5, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_STEPPER2_PULSE_INIT  {1 << 4, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

#define PIN_STEPPER3_PULSE  {1 << 22, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_STEPPER3_DIR    {1 << 18, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

#define PIN_STEPPER4_PULSE  {1 << 21, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_PERIPH_A, PIO_DEFAULT}

#define PIN_STEPPER4_DIR    {1 << 19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#endif
#ifdef UV_PRINTER
// UV Pins
#define MB_CUST_PINS_UV_STAT PIN_UV_STAT_LP1_SH_CL_I, PIN_UV_STAT_LP2_SH_OP_I, \
  PIN_UV_STAT_LP2_SH_CL_I, PIN_UV_STAT_LP1_SH_OP_I, \
  PIN_UV_STAT_UV_RDY,  PIN_UV_STAT_LP1_FAULT, PIN_UV_STAT_LP2_FAULT
#define PIN_UV_STAT_LP1_SH_CL_I  {(unsigned int)1 << 31, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_UV_STAT_LP2_SH_OP_I  {1 << 25, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_UV_STAT_LP2_SH_CL_I  {1 << 2, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_UV_STAT_LP1_SH_OP_I  {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#if defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)||defined(MANUFACTURER_DYSS)
#define PIN_UV_STAT_UV_RDY       {1 << 28, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#else
#define PIN_UV_STAT_UV_RDY       {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#endif
#define PIN_UV_STAT_LP1_FAULT    {1 << 4, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_UV_STAT_LP2_FAULT    {1 << 9, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 

#if defined( MANUFACTURER_FULGENCY_FLAT_UV)&&defined(OLD_UV)
#define MB_CUST_PINS_UV_CTRL  PIN_UV_CTRL_LP1_ON, PIN_UV_CTRL_LP2_ON, PIN_UV_CTRL_LP1_HIGHPOWER, \
  PIN_UV_CTRL_LP2_HIGHPOWER, PIN_UV_CTRL_SHUTTER_1_OPEN, PIN_UV_CTRL_SHUTTER_2_OPEN,FULGENCY_UV_OUTPUT_J19,FULGENCY_UV_OUTPUT_J22
#else
#define MB_CUST_PINS_UV_CTRL  PIN_UV_CTRL_LP1_ON, PIN_UV_CTRL_LP2_ON, PIN_UV_CTRL_LP1_HIGHPOWER, \
  PIN_UV_CTRL_LP2_HIGHPOWER, PIN_UV_CTRL_SHUTTER_1_OPEN, PIN_UV_CTRL_SHUTTER_2_OPEN
#endif

#if !defined(MANUFACTURER_DYSS)&&!defined(OLD_UV)
#define PIN_UV_CTRL_LP1_ON      {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_LP1_HIGHPOWER   {1 << 24, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_LP2_ON      {1 << 23, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_LP2_HIGHPOWER   {1 << 25, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#endif

#if defined( MANUFACTURER_DYSS)||defined(OLD_UV)
#define PIN_UV_CTRL_LP1_ON      {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_PULLUP}
#define PIN_UV_CTRL_LP1_HIGHPOWER   {1 << 24, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_PULLUP}
#define PIN_UV_CTRL_LP2_ON      {1 << 23, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_PULLUP}
#define PIN_UV_CTRL_LP2_HIGHPOWER   {1 << 25, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_PULLUP}
#define PIN_UV_CTRL_SHUTTER_1_OPEN  {1 << 26, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_PULLUP}
#define PIN_UV_CTRL_SHUTTER_2_OPEN  {1 << 27, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_PULLUP}
#ifdef MANUFACTURER_FULGENCY_FLAT_UV
#define FULGENCY_UV_OUTPUT_J19  {1 << 24, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define FULGENCY_UV_OUTPUT_J22  {1 << 31, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#endif
#elif defined( MANUFACTURER_SAIBO)||defined(AW_REVERSE_UV)||defined(MANUFACTURER_SAIBO_ROLL_UV)
#define PIN_UV_CTRL_SHUTTER_1_OPEN  {1 << 24, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_UV_CTRL_SHUTTER_2_OPEN  {1 << 31, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}//J22
#else
#define PIN_UV_CTRL_SHUTTER_1_OPEN  {1 << 24, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_UV_CTRL_SHUTTER_2_OPEN  {1 << 31, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//J22
#endif

#ifdef MANUFACTURER_DOCAN_UV
#define DOCAN_UV_MOTION_OUTPUT1  {1 << 24, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define DOCAN_UV_MOTION_OUTPUT2  {1 << 31, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#endif

#endif //UV_PRINTER

#endif //defined( EPSON_BOTTOM_BOARD_V3 )

#ifdef PG_KEY_BOARD //Not used.
#define MB_CUST_PINS_PG_KB  {0x7BF8000, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#endif

// Bell Pins
#define MB_CUST_PIN_BELL PINS_PD15

#ifdef EPSON_BOTTOM_BOARD_V3
#define MB_CUST_PINS_HEATS_INIT  PIN_HEAT1_PWM_INIT, PIN_HEAT2_PWM_INIT
#define PIN_HEAT1_PWM_INIT   {1 << 7, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_HEAT2_PWM_INIT   {1 << 8, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_HEAT3_PWM_INIT   {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_HEAT1_PWM   PIN_TIOA5
#define PIN_HEAT2_PWM   PIN_TIOB5
#define PIN_HEAT3_PWM   PIN_TIOB4	//share with stepper2 PWM.

#elif defined( EPSON_BOTTOM_BOARD_V2)
//cleaning pin
#define MB_CUST_PINS_CLEANING  PIN_CLEAN_RELEASE_1, PIN_CLEAN_SUCK_1
#define PIN_CLEAN_RELEASE_1   {1 << 7, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_CLEAN_SUCK_1      {1 << 8, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#elif defined( EPSON_BOTTOM_BOARD_V2_1)
#else
// Pump Drive Chip enable
#define MB_CUST_PINS_PUMP_DRIVER_EN  PIN_PUMP_DRIVER_1_EN, PIN_PUMP_DRIVER_2_EN
#define PIN_PUMP_DRIVER_1_EN  {1 << 7, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_PUMP_DRIVER_2_EN  {1 << 8, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}
#endif

#if defined (EPSON_BOTTOM_BOARD_V3) ||defined( EPSON_BOTTOM_BOARD_V2_1 )
#define MB_CUST_PINS_MOTION_SENSOR	PIN_MOTION_XORIGIN, PIN_MOTION_XLIMIT, PIN_MOTION_YORIGIN, PIN_MOTION_YLIMIT, \
	PIN_MOTION_ZORIGIN, PIN_MOTION_ZLIMIT, PIN_MOTION_CORIGIN, PIN_MOTION_CLIMIT
#define PIN_MOTION_XORIGIN	{1 << 7, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_MOTION_XLIMIT	{1 << 8, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_MOTION_YORIGIN	{1 << 9, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_MOTION_YLIMIT	{1 <<15, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_MOTION_ZORIGIN	{1 <<16, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_MOTION_ZLIMIT	{1 <<23, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_MOTION_CORIGIN	{1 <<24, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_MOTION_CLIMIT	{1 <<25, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#endif

#ifdef IIC_Key_Board_LCD
#ifdef LCD_MODULE_ZX19264M1A
#ifndef EPSON_BOTTOM_BOARD_V2_1
#define MB_CUST_PINS_KB_LCD PIN_KB_LCD_BUSY, PIN_KB_LCD_REQ,PIN_KB_LCD_RW, \
  PIN_KB_LCD_LE,
 #else
 #define MB_CUST_PINS_KB_LCD PIN_KB_LCD_BUSY, PIN_KB_LCD_REQ,PIN_KB_LCD_DATA_D0,PIN_KB_LCD_RW, \
  PIN_KB_LCD_LE,
 #endif
#else
#define MB_CUST_PINS_KB_LCD PIN_KB_LCD_nLED, PIN_KB_LCD_EN, PIN_KB_LCD_RW, \
  PIN_KB_LCD_RS, PIN_KB_LCD_CS2, PIN_KB_LCD_CS1, PIN_KB_LCD_DATA_BUS
#endif
#if defined (EPSON_BOTTOM_BOARD_V3) ||defined (EPSON_BOTTOM_BOARD_V2_1)
#define PIN_KB_LCD_nLED  {1 << 5, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_KB_LCD_RW    {1 << 3, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_CS3   {1 << 6, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#ifdef LCD_MODULE_ZX19264M1A
#define PIN_KB_LCD_RESET   {1 << 4, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//EN
#define PIN_KB_LCD_BUSY   {0X400, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}//D_0
#define PIN_KB_LCD_DATA_D1_7   {0xE7800, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//D1~7
#define PIN_KB_LCD_DATA_D0   {1 << 0, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//CS1
#define PIN_KB_LCD_REQ    {1 << 2, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//RS
#define PIN_KB_LCD_LE   {1 << 1, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//CS2
#else
#define PIN_KB_LCD_EN    {1 << 4, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_RS    {1 << 2, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_CS2   {1 << 1, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_CS1   {1 << 0, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_DATA_BUS   {0xE7C00, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#endif

#elif defined (EPSON_BOTTOM_BOARD_V2)
#define PIN_KB_LCD_nLED  {1 << 20, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_RW    {1 << 18, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#ifdef LCD_MODULE_ZX19264M1A
#define PIN_KB_LCD_RESET   {1 << 19, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//EN
#define PIN_KB_LCD_BUSY   {1 << 7, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}//D_0
#define PIN_KB_LCD_DATA_D1_7   {0x7F00, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//D1~7
#ifdef DYSS_UV_CLEAN
#define PIN_DYSS_UV_CLEAN_J10   {1 << 13, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#define PIN_DYSS_UV_CLEAN_J12   {1 << 14, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#elif defined(MANUFACTURER_TATE_EPSON_UV)&&defined(RIPSTAR_FLAT_EX)
#define PIN_TATE_FLAT_STOP_MOVE   {1 << 13, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#elif defined(FULGENCY_FUN)
#define PIN_FULGENCY_WASTE_INK   {1 << 13, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#endif
#define PIN_KB_LCD_DATA_D0   {1 << 15, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//CS1
#define PIN_KB_LCD_REQ    {1 << 17, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//RS
#define PIN_KB_LCD_LE   {1 << 16, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}//CS2
#else
#define PIN_KB_LCD_EN    {1 << 19, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_RS    {1 << 17, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_CS2   {1 << 16, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_CS1   {1 << 15, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_DATA_BUS   {0x7F80, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#endif
#else
// Not completed for this LCD usage.
#define PIN_KB_LCD_nLED  {1 << 28, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_EN    {1 << 27, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_RW    {1 << 26, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_RS    {1 << 25, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_CS2   {1 << 24, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_CS1   {1 << 23, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_KB_LCD_DATA_BUS   {0x7F8000, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#endif
#endif

#define MB_CUST_PINS_KB_CTRL PIN_KB_CTRL_KB_IRQ, PIN_KB_CTRL_KB_RST
#define PIN_KB_CTRL_KB_IRQ {1 << 28, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT} 
#define PIN_KB_CTRL_KB_RST {1 << 3, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}


#ifdef EPSON_BOTTOM_BOARD_V3

#define MB_CUST_PINS_INK_SUPPLY_5C {0x1F, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define MB_CUST_PINS_INK_SUPPLY_6_7 PIN_INK_SUPPLY_CTRL6, PIN_INK_SUPPLY_CTRL7
#define PIN_INK_SUPPLY_CTRL1 {1 << 0, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL2 {1 << 1, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL3 {1 << 2, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL4 {1 << 3, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL5 {1 << 4, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL6 {1 << 12, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_INK_SUPPLY_CTRL7 {1 << 17, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL8 {0, 0, 0, 0, 0}  

#elif defined(EPSON_BOTTOM_BOARD_V2)

#define MB_CUST_PINS_INK_SUPPLY_6C {0x3F, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define MB_CUST_PINS_INK_SUPPLY_7_8 PIN_INK_SUPPLY_CTRL7, PIN_INK_SUPPLY_CTRL8
#define PIN_INK_SUPPLY_CTRL1 {1 << 0, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL2 {1 << 5, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL3 {1 << 3, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL4 {1 << 4, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL5 {1 << 2, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL6 {1 << 1, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL7 {1 << 17, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL8 {1 << 16, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  

#elif defined(EPSON_BOTTOM_BOARD_V2_1)

#define MB_CUST_PINS_INK_SUPPLY_6C {0x1F, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define MB_CUST_PINS_INK_SUPPLY_7_15 PIN_INK_SUPPLY_CTRL2,PIN_INK_SUPPLY_CTRL7, PIN_INK_SUPPLY_CTRL8,PIN_INK_SUPPLY_CTRL9,PIN_INK_SUPPLY_CTRL10,\
	PIN_INK_SUPPLY_CTRL11,PIN_INK_SUPPLY_CTRL12,PIN_INK_SUPPLY_CTRL13,PIN_INK_SUPPLY_CTRL14,PIN_INK_SUPPLY_CTRL15
#define PIN_INK_SUPPLY_CTRL1 {1 << 0, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL2 {1 << 30, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL3 {1 << 3, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL4 {1 << 4, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL5 {1 << 2, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL6 {1 << 1, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL7 {1 << 17, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL8 {1 << 12, AT91C_BASE_PIOB, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL9 {1 << 7, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL10 {1 << 18, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL11 {1 << 20, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT} 
#define PIN_INK_SUPPLY_CTRL12 {1 << 21, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT} 
#define PIN_INK_SUPPLY_CTRL13 {1 << 22, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT} 
#define PIN_INK_SUPPLY_CTRL14 {1 << 8, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT} 
#define PIN_INK_SUPPLY_CTRL15 {1 << 19, AT91C_BASE_PIOE, AT91C_ID_PIOD_E, PIO_OUTPUT_0, PIO_DEFAULT} 
#else

#define MB_CUST_PINS_INK_SUPPLY_6C {0x3F, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  
#define MB_CUST_PINS_INK_SUPPLY_7_8 PIN_INK_SUPPLY_CTRL7, PIN_INK_SUPPLY_CTRL8
#define PIN_INK_SUPPLY_CTRL1 {1 << 0, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL2 {1 << 5, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL3 {1 << 3, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL4 {1 << 4, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL5 {1 << 2, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL6 {1 << 1, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL7 {1 << 17, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  
#define PIN_INK_SUPPLY_CTRL8 {1 << 16, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}  

#endif

#define PIN_USB_VBUS    {1 << 14, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT}


/// TWI  version 3xx
#define TWI_V3XX
/// TWI pins definition.
#define PINS_TWI0  {(1<<20) | (1<<21), AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

#ifndef ek_board_sim
/// DDR2 BA2 PIN, reserved for the future large-content DDR2 chip.
#define BOARD_DDR2_BA2_PIN     {1 << 26, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#else
/// DDR2 BA2 PIN, reserved for the future large-content DDR2 chip.
#define BOARD_DDR2_BA2_PIN     {0,0,0,0,0}
#endif
/// DDRAM bus width.
#define BOARD_DDRAM_BUSWIDTH    16

//#ifdef HW_TO_BE_DEFINED
#define SRAM_BUSWIDTH           16
//#endif

/// List of all SRAM pins definitions.
#if  (SRAM_BUSWIDTH == 32)
//data bus 16-31
#define PINS_SRAM_D16_31              {0xFFFF0000, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}
#else
//data bus 16-31
#define PINS_SRAM_D16_31              {0, 0, 0, 0, 0}
#endif
#define BOARD_SRAM_CS_PIN       {1 << 13, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT}

#ifndef ek_board_sim
//we select K9F5608U0D as Nand flash chip.
#define NF_CHIP_K9F5608U0D
#endif
/// Nandflash controller peripheral pins definition.
#define PINS_NANDFLASH          BOARD_NF_CE_PIN, BOARD_NF_RB_PIN, BOARD_NF_WP_PIN
/// Nandflash chip enable pin definition.
#define BOARD_NF_CE_PIN         {1 << 14, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#ifndef ek_board_sim
/// Nandflash ready/busy pin definition.
#define BOARD_NF_RB_PIN         {1 << 8, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_INPUT, PIO_PULLUP}
/// Nandflash write protect pin definition.
#define BOARD_NF_WP_PIN         {1 << 22, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#else
/// Nandflash ready/busy pin definition.
#define BOARD_NF_RB_PIN         {0, 0, 0, 0, 0}
/// Nandflash write protect pin definition.
#define BOARD_NF_WP_PIN         {0, 0, 0, 0, 0}
#endif

//need not config it, because default state is A21/NANDALE and A22/NANDCLE.
#define BOARD_NF_ALE_PIN        {1 << 4,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT} //? need testing if it is PIO_PULLUP.
#define BOARD_NF_CLE_PIN        {1 << 5,  AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT} //? need testing if it is PIO_PULLUP.

//Cheney: It seemed that CPU doesn't do any special operation for NAND Flash ALE and CLE.
//  Because ALE and CLE share A21 and A22, A21 is high means to send adress,
//  and A22 is high means to send cmd.
/// Address for transferring command bytes to the nandflash.
#define BOARD_NF_COMMAND_ADDR   0x40400000
/// Address for transferring address bytes to the nandflash.
#define BOARD_NF_ADDRESS_ADDR   0x40200000
/// Address for transferring data bytes to the nandflash.
#define BOARD_NF_DATA_ADDR      0x40000000

//Cheney: AT91SAM9G45 supports Hardware ECC.
//  however, AT91SAM9G45 only support 8 bits bus when 1 ecc per 256 bytes. 
//But, I can't run this HW ECC. So i comment this macro.
//#define HARDWARE_ECC_AT91SAM9G45

///// Address for transferring command bytes to the norflash.
//#define BOARD_NORFLASH_ADDR     0x10000000
///// Default NOR bus size after power up reset
//#define BOARD_NORFLASH_DFT_BUS_SIZE 16
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "SAM9G45-EK- Individual chip definition"
/// This page lists the definitions related to different chip's definition
/// located in the board.h file for the SAM9G45-EK.
#define BOARD_RTC_ID                AT91C_ID_SYS

/// Twi eeprom
#define BOARD_ID_TWI_EEPROM         AT91C_ID_TWI0
#define BOARD_BASE_TWI_EEPROM       AT91C_BASE_TWI0
#define BOARD_PINS_TWI_EEPROM       PINS_TWI0

#ifdef IIC_Key_Board_LCD
#if defined(EPSON_LCD) 
/// Twi keyboard
#define BOARD_ID_TWI_Keyboard       AT91C_ID_TWI1
#define BOARD_BASE_TWI_Keyboard     AT91C_BASE_TWI1
#define BOARD_PINS_TWI_Keyboard     BOARD_PINS_MB_TWI1
#endif
#endif

/// SECURITY eeprom
#define BOARD_PINS_SEC_EEPROM       BOARD_SEC_EEPROM_I2CSDA_PIN, BOARD_SEC_EEPROM_I2CSCL_PIN
#define BOARD_SEC_EEPROM_I2CSDA_PIN {1 << 12, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}
#define BOARD_SEC_EEPROM_I2CSCL_PIN {1 << 13, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_DEFAULT}

/// Protected eeprom 
#ifndef ek_board_sim
#define BOARD_PINS_PROTECTED_EEPROM_OUT_1 {1 << 14, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_OUTPUT_1, PIO_PULLUP | PIO_OPENDRAIN}
#define BOARD_PINS_PROTECTED_EEPROM {1 << 14, AT91C_BASE_PIOD, AT91C_ID_PIOD_E, PIO_INPUT, PIO_DEFAULT}
#else
#define BOARD_PINS_PROTECTED_EEPROM {0,0,0,0,0}
#endif

///// USART
//#define BOARD_PIN_USART_RXD        PIN_USART0_RXD
//#define BOARD_PIN_USART_TXD        PIN_USART0_TXD
//#define BOARD_PIN_USART_CTS        PIN_USART0_CTS
//#define BOARD_PIN_USART_RTS        PIN_USART0_RTS
//#define BOARD_USART_BASE           AT91C_BASE_US0
//#define BOARD_ID_USART             AT91C_ID_US0
////------------------------------------------------------------------------------


#endif //#ifndef BOARD_H

