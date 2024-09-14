#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "uart.h"
#include <unistd.h>
#define HEAD     0x7E

static const char *short_options = "u:b:i:o:f:";
static struct option long_options[] =
{
    {"uart",  required_argument, 0, 'u'},
    {"baud",  required_argument, 0, 'b'},
    {"nodeID",  required_argument, 0, 'i'},
    {"on",  required_argument, 0, 'o'},
    {"off",  required_argument, 0, 'f'},
    {0, 0, 0, 0}
};

static char *usage = "Usage: ./uart [option] [argument] ... [-] [arg] ..\
                      \nOptions and arguments:\
                      \n\t-u, --uart    : uart device file\t(ex: /dev/ttyS2)\
                      \n\t-b, --baud    : baudrate speed\t\t(ex: 115200)\
                      \n\t-i, --nodeID  : destination Node ID\t\t(ex: 01)\
                      \n\t-o, --on      : set on relay\t\t(ex: 115200)\
                      \n\t-f, --off     : set off relay";


unsigned short create_bitmask(char *relay_range) {
    unsigned short bitmask = 0;
    int start_relay, end_relay;
    char *token;

    token = strtok(relay_range, "-");
    start_relay = atoi(token);
    token = strtok(NULL, "-");
    if (token) {
        end_relay = atoi(token);
    } else {
        end_relay = start_relay;
    }

    for (int i = start_relay; i <= end_relay; i++) {
        bitmask |= (1 << (i - 1));
    }
    return bitmask;
}

void calculate_XOR_and_SUM(unsigned char *message, int length, unsigned char *XOR, unsigned char *SUM) {
    *XOR = 0xFF;
    *SUM = 0;
    for (int i = 2; i < length - 2; i++) {
        *XOR ^= message[i];
        *SUM += message[i];
    }
    *SUM += *XOR;
    *SUM &= 0xFF;
}

void print_buffer(unsigned char *buffer, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{

    const char *uart_device = NULL;
    int baud_rate = 115200;
    unsigned char DID = 0x01;
    unsigned short on_bitmask = 0, off_bitmask = 0;
    int length = 0;
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'u':
                uart_device = optarg;
                break;
            case 'b':
                baud_rate = atoi(optarg);
                break;
            case 'i':
                DID = (unsigned char)strtol(optarg, NULL, 16);
                break;
            case 'o':
                on_bitmask = create_bitmask(optarg);
                break;
            case 'f':
                off_bitmask = create_bitmask(optarg);
                break;
            default:
                printf("Usage: ./uart [options]\n");
                return -1;
        }
    }


    if (uart_device == NULL || baud_rate == -1 || DID == -1 )
    {
        printf("%s\n", usage);
        return -1;
    }

    unsigned char tx_buffer[11];
    tx_buffer[0] = HEAD;                 // Head
    tx_buffer[1] = 0x09;                 // Length (from DID to SUM)
    tx_buffer[2] = DID;                  // DID
    tx_buffer[3] = 0x17;                 // CTL
    tx_buffer[4] = 0x00;                 // CMD
    tx_buffer[5] = (on_bitmask >> 8) & 0xFF;   // Data0 (On Relay High byte)
    tx_buffer[6] = on_bitmask & 0xFF;    // Data1 (On Relay Low byte)
    tx_buffer[7] = (off_bitmask >> 8) & 0xFF;  // Data2 (Off Relay High byte)
    tx_buffer[8] = off_bitmask & 0xFF;   // Data3 (Off Relay Low byte)

    unsigned char XOR, SUM;
    calculate_XOR_and_SUM(tx_buffer, 11 , &XOR, &SUM);
    tx_buffer[9] = XOR;                  // XOR
    tx_buffer[10] = SUM;                 // SUM
    
    printf("tx_buffer: ");
    print_buffer(tx_buffer, sizeof(tx_buffer));
    
    uart_t uart;
    uart.baudrate = baud_rate;
    uart.path = (UART_DataType *)uart_device;

    if (UART_Init(&uart) != UART_Res_NoErr)
    {
        printf("Failed to initialize UART\n");
        return -1;
    }
    
    if (UART_Write(&uart, tx_buffer, sizeof(tx_buffer)) != UART_Res_NoErr) {
        printf("Failed to write to UART\n");
        return -1;
    }

    // unsigned char rx_buffer[256];
    // int rx_len = UART_Read(&uart, (UART_DataType *)rx_buffer, sizeof(rx_buffer));
    // if (rx_len < 0)
    //         {
    //             //An error occured
    //         printf("UART RX error\n");
    //         }
    //     else if (rx_len == 0)
    //         {
    //                 //No data waiting
    //         printf("no data UART RX   test commit\n");
    //         }
    //     else
    //         {
    //             printf("read data: ");
    //             for (int i = 0; i < rx_len; i++) {
    //                 printf("%02X ", rx_buffer[i]);
    //             }
    //             printf("\n");

    //         }
 
    return 0;
}
