
#include <pthread.h>
#include <stdbool.h>

#ifndef __UART_H__
#define __UART_H__
#ifdef __cplusplus

		

extern "C" {
#endif

#define PACKET_LENG     (64) // 64 bytes 

	typedef  unsigned char PackLenType ;
	typedef  unsigned char PackDatType ;

	typedef  unsigned char UART_DataType;
	
	typedef enum
	{
		UART_Res_NoErr = 0,
		UART_Res_OpenErr, 			// Uart file device error
		UART_Res_WriteErr,        // Write error
		UART_PortErr,         // Open port error
		UART_SpeedErr,        // Set speed baudrate error
		UART_ConfErr,         // Config uart error
	}UART_ResType;

	typedef struct pack
	{
		bool enable_read;
		PackDatType readpack_str[PACKET_LENG];

	}pack_t;

	typedef struct uart
	{
		int 	baudrate;		
		int 	read_minsize;
		int 	ctrl_flow;
		UART_DataType	*path;
		int  	fd ;
		pack_t			pack_status;
		pthread_mutex_t 	mutex_cks;
		pthread_t 			pthread_read_id;
	}uart_t;

	/**
	 * @UART_Init-----------------------------------------------------------------
	 * @brief Use to initialize uart
	 * @param[in] uart: uart will be initialized
 	 * @return error code:  @ref UART_ResType
	 * @note: Always call this function before use uart.
	 *  ---------------------------------------------------------------------------------
	 */
	unsigned char UART_Init(uart_t *uart);
	
	/**
	 * @UART_Deinit-----------------------------------------------------------------
	 * @brief Use to de-initialize uart
	 * @param[in] uart:  uart will be de-initialized
 	 * @return error code:  @ref UART_ResType
	 * @note: Always call this function after use uart.
	 *  ---------------------------------------------------------------------------------
	 */
	unsigned char UART_Deinit(uart_t *uart);


	/**
	 * @UART_Write-----------------------------------------------------------------
	 * @brief Use to transmit data to uart
	 * @param[in] uart: uart be used to write data
	 * @param[in] data: content of data will be written into uart.
	 * @param[in] leng: leng of data will be written.
	 * @return error code: @ref UART_ResType.
	 */
	UART_ResType UART_Write(uart_t *uart, UART_DataType *data, int leng);

	/**
	 * @UART_Read-----------------------------------------------------------------
	 * @brief Use to read data from uart
	 * @param[in] uart: uart be used to read data
	 * @param[in] data: read data from uart then save into data.
	 * @param[in] leng: read leng byte from uart.
	 * @return the number bytes was read from uart
	 */
	int UART_Read(uart_t *uart, UART_DataType *data, int leng);
	/**
	 * @DFU_GetPack-----------------------------------------------------------------
	 * @brief Use to get content of packet into dfu->pack_status.readpack_str
	 * @param[in] dfu: dfu will be used
	 * @param[in] packet: get content data from packet
	 * @param[in] leng: leng bytes of packet
 	 * @return error code:  @ref DFU_PackResType
	 *  ---------------------------------------------------------------------------------
	 */
	// UART_PackResType UART_GetPack(uart_t *uart, PackDatType *packet, PackLenType leng);
#ifdef __cplusplus
}
#endif
#endif
