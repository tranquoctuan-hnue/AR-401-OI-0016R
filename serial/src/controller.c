#include "msg.h"
#include "uart.h"

bool ternimate_proc = false;

void controller_stop_process() {
    ternimate_proc = true;
}

void *pthread_read(void *argc)
{
    while (!ternimate_proc)
    {
        read_and_decode_msg();
        usleep(100);
    }

    printf("End pthread read uart\n");
    pthread_exit(NULL);
}

void *pthread_send(void *argc)
{
    while (!ternimate_proc) {
        char cmd_id = getc();
        printf("--Received a GET request")
        encode_msg_and_send(cmd_id);
        usleep(100);
    }
    pthread_exit(NULL);
}
