
#ifndef _PSP_SPI_H_
#define _PSP_SPI_H_

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define QSPI_TRANSMIT_ADDRESS 	0x00
#define QSPI_RECEIVE_ADDRESS 	0x10
#define QSPI_COMMAND_ADDRESS 	0x20

// number of bits to be transferred for each entry in the queue
#define QSPI_TRANS_SIZE		8

// default qspi boud rate in Hz
#define QSPI_BAUD_RATE		400000

#define PSP_SPI_SUCCESS 0
#define PSP_SPI_ERROR   1

typedef int  t_spi_ret;

t_spi_ret psp_spi_tx1 ( uint8_t uid, uint8_t val );
t_spi_ret psp_spi_tx ( uint8_t uid, uint8_t * p_src, uint32_t len );
t_spi_ret psp_spi_rx ( uint8_t uid, uint8_t * p_dst, uint32_t len );
void psp_spi_cs_lo ( uint8_t uid );
void psp_spi_cs_hi ( uint8_t uid );
void psp_spi_lock ( uint8_t unit );
void psp_spi_unlock ( uint8_t unit );
t_spi_ret psp_spi_set_baudrate ( uint8_t uid, uint32_t br );
t_spi_ret psp_spi_get_baudrate ( uint8_t uid, uint32_t * p_br );
t_spi_ret psp_spi_init ( uint8_t uid );
t_spi_ret psp_spi_start ( uint8_t uid );
t_spi_ret psp_spi_stop ( uint8_t uid );
t_spi_ret psp_spi_delete ( uint8_t uid );



#ifdef __cplusplus
}
#endif

#endif /* _PSP_SPI_H_ */

