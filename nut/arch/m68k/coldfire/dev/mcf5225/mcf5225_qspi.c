
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/event.h>
#include <dev/irqreg.h>
#include <dev/board.h>
#include <dev/mcf5225_qspi.h>
#include <sys/timer.h>
#include <arch/m68k.h>

#include <sys/atom.h>

typedef int  t_spi_ret;

static uint32_t tx_counter, rx_counter;
static uint8_t *p_tx_data, *p_rx_data;
static uint32_t size_data;

static HANDLE mutex_tran_start, mutex_tran_end;

void QspiInterrupt(void *arg);

/*
 * Auxiliary common functions
 */
static inline void FillBuff(uint8_t *p_src, uint32_t len)
{
	uint32_t i;

    for (i = 0; i < len; i++ ){
    	MCF_QSPI_QAR = QSPI_COMMAND_ADDRESS + i;
        MCF_QSPI_QDR = MCF_QSPI_QDR_DATA(MCF_QSPI_QDR_BITSE
        		| MCF_QSPI_QDR_CONT);
		//fill data
		MCF_QSPI_QAR = QSPI_TRANSMIT_ADDRESS + i;

        MCF_QSPI_QDR = (p_src == NULL) ? 0xFF : p_src[i];
        tx_counter++;
    }
}

static inline void SetupXmit(uint32_t len)
{
	//setup start and end queue pointers
	MCF_QSPI_QWR = (MCF_QSPI_QWR&MCF_QSPI_QWR_CSIV) |
			MCF_QSPI_QWR_NEWQP(0) | MCF_QSPI_QWR_ENDQP(len - 1);
}

static inline void StartXmit(void)
{
	// start transmit data
	MCF_QSPI_QDLYR |= MCF_QSPI_QDLYR_SPE;
}

/*
** qspi_tx1
**
** Transmit 1 byte
**
** Input:
**   uid   - unit ID
**   val   - value to send
** Return:
**   QSPI_...
*/
t_spi_ret qspi_tx1 ( uint8_t uid, uint8_t val )
{
	// wait on mutex 'start transmit'
	NutEventWait(&mutex_tran_start, NUT_WAIT_INFINITE);

	tx_counter = 0;
	rx_counter = 0;

	FillBuff(&val, 1);

    MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
    MCF_QSPI_QIR &= ~MCF_QSPI_QIR_SPIFE;

    SetupXmit(1);
    StartXmit();

    while (!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))	// polling
    	;/* Empty Body */

//    MCF_QSPI_QIR |= (MCF_QSPI_QIR_SPIF | MCF_QSPI_QIR_SPIFE);

    // release mutex 'start transmit'
    NutEventPost(&mutex_tran_start);

    return QSPI_SUCCESS;
}

/*
** qspi_tx
**
** Transmit 'len' number of bytes
**
** Input:
**   uid   - unit ID
**   p_src - pointer to source buffer
**   len   - number of bytes to transmit
** Return:
**   QSPI_...
*/
t_spi_ret qspi_tx ( uint8_t uid, uint8_t * p_src, uint32_t len )
{
	// wait on mutex 'start transmit'
	NutEventWait(&mutex_tran_start, NUT_WAIT_INFINITE);

	tx_counter = 0;
	rx_counter = 0;

    if (len > 16) // interrupt method
    {
        // save source data pointer to interrupt TX buffer
        p_tx_data = p_src;
        // save destination data pointer to interrupt RX buffer
        p_rx_data = NULL;
        // save length of receive data to interrupt data size
        size_data = len;

        // initialization of interrupt 'end of queue'
        MCF_QSPI_QIR |= (MCF_QSPI_QIR_SPIF | MCF_QSPI_QIR_SPIFE);

       	// limited transmit data length
        len = (len > 16) ? 16 : len;

        // transmit only first 16 bytes of TX data
		FillBuff(p_src, len);

        SetupXmit(len);
        // start transmit data
        StartXmit();

        // wait on mutex 'end transmit'
        NutEventWait(&mutex_tran_end, NUT_WAIT_INFINITE);

    }
    else // polling method
    {
        FillBuff(p_src, len);

        MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
        MCF_QSPI_QIR &= ~MCF_QSPI_QIR_SPIFE;

        SetupXmit(len);
        StartXmit();

        while (!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))
            ; /* Empty Body */

//        MCF_QSPI_QIR |= (MCF_QSPI_QIR_SPIF | MCF_QSPI_QIR_SPIFE);
    }
	// release mutex 'start transmit'
	NutEventPost(&mutex_tran_start);

    return QSPI_SUCCESS;
}

/*
** qspi_rx
**
** receive 'len' number of bytes
**
** Input:
**   uid   - unit ID
**   p_dst - pointer to destination buffer
**   len   - number of bytes to receive
** Return:
**   QSPI_...
*/
t_spi_ret qspi_rx ( uint8_t uid, uint8_t * p_dst, uint32_t len )
{
	uint8_t j;

	// wait on mutex 'start transmit'
	NutEventWait(&mutex_tran_start, NUT_WAIT_INFINITE);

	tx_counter = 0;
	rx_counter = 0;

    if (len > 16) // interrupt method
    {

        // save source data pointer to interrupt TX buffer
        p_tx_data = NULL;
        // save destination data pointer to interrupt RX buffer
        p_rx_data = p_dst;
        // save length of receive data to interrupt data size
        size_data = len;

        // initialization of interrupt 'end of queue'
        MCF_QSPI_QIR |= (MCF_QSPI_QIR_SPIF | MCF_QSPI_QIR_SPIFE);

       	// limited transmit data length
        len = (len > 16) ? 16 : len;

        FillBuff(NULL, len);

        SetupXmit(len);
        // start transmit data
        StartXmit();

        // wait on mutex 'end transmit'
        NutEventWait(&mutex_tran_end, NUT_WAIT_INFINITE);
    }
    else // polling method
    {
        FillBuff(NULL, len);

        MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
        MCF_QSPI_QIR &= ~MCF_QSPI_QIR_SPIFE;

        SetupXmit(len);
        StartXmit();

        while (!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))
            ; /* Empty Body */

        MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS;
        for (j=0; j < len; j++) {
            p_dst[j] = MCF_QSPI_QDR;
        }

        MCF_QSPI_QIR |= (MCF_QSPI_QIR_SPIF | MCF_QSPI_QIR_SPIFE);
    }
	// release mutex 'start transmit'
	NutEventPost(&mutex_tran_start);

    return QSPI_SUCCESS;
}

/*
** qspi_cs_lo
**
** Set chip select low.
**
** Input:
**   uid   - unit ID
*/
void qspi_cs_lo ( uint8_t uid )
{
	MCF_GPIO_PORTQS &= ~(MCF_GPIO_PORTQS_PORTQS3);
}


/*
** qspi_cs_hi
**
** Set chip select high.
**
** Input:
**   uid   - unit ID
*/
void qspi_cs_hi ( uint8_t uid )
{
	MCF_GPIO_PORTQS |= MCF_GPIO_PORTQS_PORTQS3;
}

/*
** qspi_lock
**
** Lock the SPI for the specific unit. This can
** be useful if multiple units are attached to the
** same SPI bus.
**
** Input:
**   uid   - unit ID
*/
void qspi_lock ( uint8_t uid )
{
	return;
}


/*
** qspi_unlock
**
** Unlock the SPI for the specific unit. This can
** be useful if multiple units are attached to the
** same SPI bus.
**
** Input:
**   uid   - unit ID
*/
void qspi_unlock ( uint8_t uid )
{
	return;
}


/*
** qspi_set_baudrate
**
** Set baudrate.
**
** Input:
**   uid   - unit ID
**   br    - baudrate in Hz
** Return:
**   QSPI_...
*/
t_spi_ret qspi_set_baudrate ( uint8_t uid, uint32_t br )
{
	uint16_t baudRate;

	baudRate = ( NutGetCpuClock() / (2 * br) );	/* 80M/(2*BaudRate) */
	baudRate = (baudRate < 2) ? 2 : baudRate;		// set min possible baudrate
	baudRate = (baudRate > 255) ? 255 : baudRate;	// set max possible baudrate
    if ((baudRate > 1 ) && (baudRate < 256)){
        MCF_QSPI_QMR = (MCF_QSPI_QMR & ~MCF_QSPI_QMR_BAUD(0xFF)) | MCF_QSPI_QMR_BAUD((uint8_t)baudRate);

        return QSPI_SUCCESS;
    }
    else
        return QSPI_ERROR;
}

/*
** qspi_get_baudrate
**
** Get baudrate.
**
** Input:
**   uid   - unit ID
** Output:
**   p_br  - baudrate in Hz
** Return:
**   QSPI_...
*/
t_spi_ret qspi_get_baudrate ( uint8_t uid, uint32_t * p_br )
{
	uint8_t baud;

	baud = MCF_QSPI_QMR & 0xFF;
	*p_br = ( NutGetCpuClock() / (2 * baud) );

	return QSPI_SUCCESS;
}

/*
** qspi_init
**
** Init SPI port
**
** Input:
**   uid   - unit ID
** Return:
**   QSPI_...
*/
t_spi_ret qspi_init ( uint8_t uid )
{
	t_spi_ret ret = QSPI_SUCCESS;

	// CS0 to high
	MCF_GPIO_PORTQS |= MCF_GPIO_PORTQS_PORTQS3;

	MCF_GPIO_PQSPAR |= MCF_GPIO_PQSPAR_QSPI_DOUT_DOUT |
                          MCF_GPIO_PQSPAR_QSPI_DIN_DIN |
                          MCF_GPIO_PQSPAR_QSPI_CLK_CLK
                          /*| MCF_GPIO_PQSPAR_QSPI_CS0_CS0*/;

    // Port QS Data Direction pin 3 (QSPI_PCS0) as an output
    MCF_GPIO_DDRQS |= MCF_GPIO_DDRQS_DDRQS3;

    // Set as a Master always and set CPOL & CPHA
    MCF_QSPI_QMR = MCF_QSPI_QMR_MSTR /*| MCF_QSPI_QMR_CPHA | MCF_QSPI_QMR_CPOL */;
    // Set number of bits to be transferred for each entry in the queue
    MCF_QSPI_QMR |= MCF_QSPI_QMR_BITS(QSPI_TRANS_SIZE);

    // Set baud rate
    ret |= qspi_set_baudrate(0, QSPI_BAUD_RATE);

    // Use active low as default
    MCF_QSPI_QWR = MCF_QSPI_QWR_CSIV;

    // Set interrupt mode
    MCF_QSPI_QIR = (MCF_QSPI_QIR_WCEFB | MCF_QSPI_QIR_ABRTB |
                      MCF_QSPI_QIR_ABRTL | MCF_QSPI_QIR_SPIFE |
                      MCF_QSPI_QIR_WCEF | MCF_QSPI_QIR_ABRT |
                      MCF_QSPI_QIR_SPIF);

    // Register interrupt handler
    NutRegisterIrqHandler(&sig_QSPI_TF, QspiInterrupt, NULL);
   	NutIrqEnable(&sig_QSPI_TF);

   	// put mutex 'start transmit' into signaled state
   	NutEventPost(&mutex_tran_start);

    return ret;
}

/*
** qspi_start
**
** Start SPI port
**
** Input:
**   uid   - unit ID
** Return:
**   QSPI_...
*/
t_spi_ret qspi_start ( uint8_t uid )
{
  return QSPI_SUCCESS;
}

/*
** qspi_stop
**
** Stop SPI port
**
** Input:
**   uid   - unit ID
** Return:
**   QSPI_...
*/
t_spi_ret qspi_stop ( uint8_t uid )
{
  return QSPI_SUCCESS;
}

/*
** qspi_delete
**
** Delete SPI port
**
** Input:
**   uid   - unit ID
** Return:
**   QSPI_...
*/
t_spi_ret qspi_delete ( uint8_t uid )
{
  return QSPI_SUCCESS;
}

/*
 * QSPI ISR
 */
void QspiInterrupt(void *arg)
{
	uint32_t i;

   	MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS;
   	for (i = rx_counter; i < tx_counter; i++ ) {
   		if (p_rx_data != NULL) {
   			p_rx_data[rx_counter] = MCF_QSPI_QDR;
   		}
   		rx_counter++;
   	}

   	FillBuff((p_tx_data != NULL) ? &p_tx_data[tx_counter] : NULL, (size_data - tx_counter) > 16 ? 16 : (size_data - tx_counter));

    if (tx_counter == size_data)
    	SetupXmit((size_data)%16);

    if (rx_counter == size_data) {
    	// release mutex 'end transmit'
    	NutEventPostFromIrq(&mutex_tran_end);
    	return;
    }

    StartXmit();
}