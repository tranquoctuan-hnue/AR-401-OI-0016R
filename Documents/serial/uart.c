#include <stdio.h>
#include <unistd.h>             //Used for UART
#include <fcntl.h>              //Used for UART
#include <termios.h>		//Used for UART


int main(int argv, char * argc[])
{
    // Setting Up The UART
    int uart0_filestream = -1;
    
    // OPEN THE UART
    uart0_filestream = open("/dev/ttyS3", O_RDWR | O_NOCTTY | O_NDELAY); // Open in non-blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Error - Unable to open UART. Ensure it is not in use by another application\n");
        return -1;
    }
    
    // CONFIGURE THE UART
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    cfsetispeed(&options, B9600);  // Set baud rate
    cfsetospeed(&options, B9600);  // Set baud rate
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // Set baud rate, 8-bit characters, ignore modem control lines, enable receiver
    options.c_iflag = IGNPAR;      // Ignore framing/parity errors
    options.c_oflag = 0;          // No output processing
    options.c_cc[VMIN] = 1; //read() will return after receiving 1 character
    options.c_cc[VTIME] = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);
    // Transmitting Hex Bytes 
    // unsigned char tx_buffer[] = {0x7E, 0x09, 0x01, 0x17, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xE9, 0xFF};
	unsigned char tx_buffer[] = {0x7E, 0x04, 0x01, 0x18, 0xE6, 0xFF};

    if (uart0_filestream != -1)
    {
        int count = write(uart0_filestream, tx_buffer, sizeof(tx_buffer)); // Filestream, bytes to write, number of bytes to write
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("Sent %d bytes\n", count);
        }
    }
    close(uart0_filestream);

    uart0_filestream = open("/dev/ttyS3", O_RDWR | O_NOCTTY | O_NDELAY); // Open in non-blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Error - Unable to open UART. Ensure it is not in use by another application\n");
        return -1;
    }

    if (uart0_filestream != -1)
    {
        unsigned char rx_buffer[256];
        int rx_length = 0;

        while (1) // Try 10 times before timing out
        {
            rx_length = read(uart0_filestream, (void*)rx_buffer, 255); // Read up to 255 characters
            if (rx_length < 0)
            {
                printf("UART RX error\n");
            }
            else if (rx_length == 0)
            {
                printf("No data received, retrying...\n");
            }
            else
            {
                // Bytes received
                rx_buffer[rx_length] = '\0'; // Null-terminate the string
                printf("%i bytes read: ", rx_length);
                for (int i = 0; i < rx_length; i++)
                {
                    printf("%02X ", rx_buffer[i]);
                }
                printf("\n");
                break; // Exit loop once data is received
            }
			usleep(1000);
        }
    }
    // Closing the UART if no longer needed
    close(uart0_filestream);
 
    return 0;
}
