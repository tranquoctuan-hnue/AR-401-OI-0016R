#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include <errno.h>
#include <stdint.h>
#include <sys/select.h>
// #include <asm-generic/termbits.h>

int UART_BaudtoSpeed(int baundrate);
UART_ResType UART_Config(int uart_fd, int speed, int flow);
UART_ResType UART_SetReadmin(int uart_fd, int min_size);

void *pthread_readUART_init(void *argc);
void *pthread_readUART_deinit(uart_t *uart);

int UART_BaudtoSpeed(int baudrate)
{
    switch (baudrate)
    {
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
     default:
        return (int)UART_SpeedErr;
    }
    return (int)UART_Res_NoErr;
}


// Hàm thiết lập UART
unsigned char UART_Init(uart_t *uart)
{
    unsigned char status;
    int fd = open((char *)uart->path, O_RDWR | O_NOCTTY | O_NDELAY); // Open in non-blocking read/write mode
    uart->fd = fd;

    uart->pack_status.enable_read = true;
    pthread_create(&uart->pthread_read_id, NULL, pthread_readUART_init, uart);
	pthread_mutex_init(&uart->mutex_cks, NULL);

    if (fd == -1)
    {
        printf("Open UART Device File Error\n");
        status = (unsigned char) UART_Res_OpenErr;
        return (unsigned char)UART_Res_NoErr;
    }

    struct termios options;

    int baud_rate = UART_BaudtoSpeed(uart->baudrate);
    if (baud_rate == UART_SpeedErr)
    {
        printf("Invalid baudrate\n");
        status = (unsigned char)UART_SpeedErr;
        return (unsigned char)UART_Res_NoErr;
    }

    UART_ResType rmin= UART_SetReadmin(fd, uart->read_minsize);
    if (rmin != UART_Res_NoErr){
        printf("UART Config Readmin Fail\n");
        return (unsigned char)rmin;
    }
    UART_ResType conf = UART_Config(fd, baud_rate, uart->ctrl_flow);
    if (conf != UART_Res_NoErr){
        printf("UART Config Fail\n");
        return (unsigned char)conf;
    }
}

unsigned char UART_Deinit(uart_t *uart)
{
    // int uart_filestream = open((char *)uart->path, O_RDWR );
    if( uart->fd){
        close(uart->fd);
    }
    pthread_mutex_destroy(&uart->mutex_cks);
	pthread_join(uart->pthread_read_id , (void *)pthread_readUART_deinit(uart));
    return (unsigned char)UART_Res_NoErr;
}

// Hàm truyền dữ liệu qua UART
UART_ResType UART_Write(uart_t *uart, UART_DataType *data, int leng)
{
    if (uart->fd < 0){
        return UART_Res_OpenErr;
    }
    int rval = write(uart->fd, data, leng); // Transmit file content
    if (rval < 0){
        printf("Write Uart errno: %d\n", errno);
        return UART_Res_WriteErr;
    }
    return UART_Res_NoErr;
}

int UART_Read(uart_t *uart, UART_DataType *data, int leng)
{
    int uart_fd = uart->fd;
    if (uart_fd < 0){
        return 0;
    }
    int recvbytes = 0;
    int maxfd = uart_fd + 1;
    int index = 0;
    /* set the timeout as 1 ms*/
    struct timeval timeout = {1, 0};
    fd_set readSet;

    FD_ZERO(&readSet);
    FD_SET(uart_fd, &readSet);
    if (select(maxfd, &readSet, NULL, NULL, &timeout) > 0){
        if (FD_ISSET(uart_fd, &readSet)){
            recvbytes = read(uart_fd, data, leng);
        }
    }    return recvbytes;
}

void *pthread_readUART_init(void *argc)
	{
	    uart_t *uart = (uart_t*)argc;
        
		while (uart->pack_status.enable_read == true)
		{
			if( UART_Read(uart, uart->pack_status.readpack_str, PACKET_LENG) > 0) {
                printf("UART_Read: \n");
				for(int i = 0; i < PACKET_LENG; i++)
				{
					printf("%02X ", uart->pack_status.readpack_str[i]);
				}
				printf("\n");
				memset(uart->pack_status.readpack_str, 0x00, PACKET_LENG);
			}
            else if (UART_Read(uart, uart->pack_status.readpack_str, PACKET_LENG) < 0){
                printf("UART RX error\n");
            }
            else {
                printf("no data UART RX   test commit\n"); 
		    }
        }
		printf("End pthread read uart\n");
		pthread_exit(NULL);
	}
void *pthread_readUART_deinit(uart_t *uart)
	{
		uart->pack_status.enable_read = false;
		memset(uart->pack_status.readpack_str, 0x00, strlen(uart->pack_status.readpack_str));
	}

UART_ResType UART_SetReadmin(int uart_fd, int min_size)
{
    struct termios ti;

    if (tcgetattr(uart_fd, &ti) < 0){
        printf("Can't Get Port Settings\n");
        return UART_PortErr;
    }

    /* set the minimimum no. of chracters to read in each
     * read call.
     */
    ti.c_cc[VMIN] = min_size;
    /* set "read timeout between characters" as 100 ms.
     * read returns either when the specified number of chars
     * are received or the timout occurs */
    ti.c_cc[VTIME] = 0; /* 0-255 equal 0-25.5s ex:  1 = 0.1s */

    if (tcsetattr(uart_fd, TCSANOW, &ti)< 0){
        /* handle printf */
        printf("Can't Set Port Settings");
        return UART_ConfErr;
    }
    return UART_Res_NoErr;
}

UART_ResType UART_Config(int uart_fd, int speed, int flow)
{
    struct termios ti;
    bzero(&ti, sizeof(ti));

    tcflush(uart_fd, TCIOFLUSH);
    if (tcgetattr(uart_fd, &ti) < 0){
        printf("Can't Get Port Settings\n");
        return UART_PortErr;
    }
    cfmakeraw(&ti);
    /* enable the receiver and set local mode */
    ti.c_cflag |= (CLOCAL | CREAD);

    /* set no parity, stop bits, data bits */
    ti.c_cflag &= ~PARENB;
    ti.c_cflag &= ~CSTOPB;
    /* set character size as 8 bits */
    ti.c_cflag &= ~CSIZE;
    ti.c_cflag |= CS8;
    /* Raw input mode, sends the raw and unprocessed data  ( send as it is) */
    ti.c_lflag &= ~(ICANON | ECHO | ISIG);
    /* Raw output mode, sends the raw and unprocessed data  ( send as it is).
        * If it is in canonical mode and sending new line char then CR
        * will be added as prefix and send as CR LF
        */
    ti.c_oflag = ~OPOST;

    if (flow)
        ti.c_cflag |= CRTSCTS;
    else
        ti.c_cflag &= ~CRTSCTS;
    /* set baud rates */
    cfsetospeed(&ti, speed);
    cfsetispeed(&ti, speed);

    if (tcsetattr(uart_fd, TCSANOW, &ti) < 0){
        printf("Can't Config Port Settings\n");
        return UART_ConfErr;
    }
    tcflush(uart_fd, TCIOFLUSH);

    return UART_Res_NoErr;
}