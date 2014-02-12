#ifndef _DEV_SCIH8_H_
#define _DEV_SCIH8_H_

/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*
 * $Log$
 * Revision 1.2  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 *
 */

#include <sys/device.h>
#include <dev/uart.h>

/*!
 * \file dev/scih8.h
 * \brief H8/3068F on-chip SCI definitions.
 */

/*!
 * \addtogroup xgUartH8
 */
/*@{*/

#define UART_MF_RTSSENSE    0x00000001UL        /*!< DCE input, low on port bit is +12V, which means ON. */
#define UART_MF_CTSCONTROL  0x00000002UL        /*!< DCE output. */
#define UART_MF_DTRSENSE    0x00000004UL        /*!< DCE input. */
#define UART_MF_DSRCONTROL  0x00000008UL        /*!< DCE output. */
#define UART_MF_DCDCONTROL  0x00000010UL        /*!< DCE output. */

#define UART_MF_RTSCONTROL  0x00000020UL        /*!< DTE output. */
#define UART_MF_CTSSENSE    0x00000040UL        /*!< DTE input. */
#define UART_MF_DTRCONTROL  0x00000080UL        /*!< DTE output. */
#define UART_MF_DSRSENSE    0x00000100UL        /*!< DTE input. */
#define UART_MF_DCDSENSE    0x00000200UL        /*!< DTE input. */

#define UART_MF_SENSEMASK   0x0345      /*!< Handshake sense mask. */
#define UART_MF_CONTROLMASK 0x00BC      /*!< Handshake control mask. */

#define UART_MF_XONXOFF     0x00000400UL        /*!< Software handshake. */

#define UART_MF_LOCALECHO   0x00010000UL        /*!< Should be used in stream, not device. */
#define UART_MF_COOKEDMODE  0x00020000UL        /*!< Should be used in stream, not device. */

#define UART_MF_NOBUFFER    0x00100000UL        /*!< No buffering. */
#define UART_MF_LINEBUFFER  0x00200000UL        /*!< Line buffered. */
#define UART_MF_BUFFERMASK  0x00300000UL        /*!< Masks buffering mode flags. */


#define UART_SF_RTSOFF      0x00000001UL        /*!< Set RTS line is off. */
#define UART_SF_CTSOFF      0x00000002UL        /*!< Set CTS line is off. */
#define UART_SF_DTROFF      0x00000004UL        /*!< Set DTR line is off. */
#define UART_SF_DSROFF      0x00000008UL        /*!< Set DSR line is off. */
#define UART_SF_DCDOFF      0x00000010UL        /*!< Set DCD line is off. */

#define HDLC_SF_FLUSH       0x00001000UL        /*!< Waiting for next HDLC flag. */
#define HDLC_SF_ESCAPED     0x00002000UL        /*!< Next character escaped. */

#define UART_SF_TXDISABLED  0x00000040UL        /*!< Transmitter disabled. */
#define UART_SF_RXDISABLED  0x00000080UL        /*!< Receiver disabled. */


#define UART_HS_DCERTSCTS   0x00000003UL        /*!< RTS/CTS DCE handshake. */
#define UART_HS_DCEFULL     0x0000001FUL        /*!< Full DCE handshake. */

#define UART_HS_DTERTSCTS   0x00000060UL        /*!< RTS/CTS DTE handshake. */
#define UART_HS_DTEFULL     0x000003E0UL        /*!< Full DTE handshake. */

#define UART_HS_XONXOFF     0x00000400UL        /*!< Software handshake. */


/*!
 * UART device control block type.
 */
typedef struct _UARTDCB UARTDCB;

/*!
 * \struct _UARTDCB uartavr.h dev/uartavr.h
 * \brief UART device control block structure.
 */
struct _UARTDCB {
    /*! \brief Baudrate selector.
     *
     * See the ATmega datasheet for further details.
     */
    uint8_t dcb_baudSelect;

    /*! \brief Read timeout.
     */
    uint32_t dcb_rtimeout;

    /*! \brief Write timeout.
     */
    uint32_t dcb_wtimeout;

    /*! \brief Queue of threads waiting for output buffer empty.
     *
     * Threads are added to this queue when calling UartAvrFlush().
     */
    HANDLE dcb_tx_rdy;

    /*! \brief Queue of threads waiting for a character in the input buffer.
     *
     * Threads are added to this queue when calling UartAvrInput().
     */
    HANDLE dcb_rx_rdy;

    /*! \brief Mode flags.
     */
    uint32_t dcb_modeflags;
};

/*@}*/

/*
 * SCIs (USARTs) available in the H8/3068F.
 *
 * The list should be adjusted for other H8 devices
 *
 */
extern NUTDEVICE devSci0;
extern NUTDEVICE devSci1;
extern NUTDEVICE devSci2;


#endif                          /* #ifndef _DEV_SCIH8_H_ */
