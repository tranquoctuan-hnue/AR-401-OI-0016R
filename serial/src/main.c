#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "uart.h"
#include <unistd.h>

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



void print_buffer(unsigned char *buffer, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("received SIGINT\n");
        controller_stop_process();
    }

}

int main(int argc, char *argv[])
{

    const char *path = NULL;
    int baud_rate = 115200;
    unsigned char DID = 0x01;
    unsigned short on_bitmask = 0, off_bitmask = 0;
    int length = 0;
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'u':
                path = optarg;
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


    if (path == NULL || baud_rate == -1 || DID == -1 )
    {
        printf("%s\n", usage);
        return -1;
    }

    if (UART_Init(path, baud_rate) != UART_Res_NoErr)
    {
        printf("Failed to initialize UART\n");
        return -1;
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR)
    
    pthread_join(uart.pthread_read_fd, NULL);
    pthread_join(uart.pthread_send_fd, NULL);

    return 0;
}
