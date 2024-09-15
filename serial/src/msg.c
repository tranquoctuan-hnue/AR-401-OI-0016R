#include "msg.h"
#include "uart.h"

char tx_send_msg[512] = {0};
char rx_recv_msg[512] = {0};
int tx_cmd_id;

void calculate_XOR_and_SUM(unsigned char *msg, int length, unsigned char *XOR, unsigned char *SUM) {
    *XOR = 0xFF;
    *SUM = 0;
    for (int i = 2; i < length - 2; i++) {
        *XOR ^= msg[i];
        *SUM += msg[i];
    }
    *SUM += *XOR;
    *SUM &= 0xFF;
}

int encode_msg(cmd_id_e cmd_id) {
    unsigned char XOR, SUM;
    switch (cmd_id) {
        case GET_DEV:
        Get_Dev_Msg_Send_t *msg = (Get_Dev_Msg_Send_t *)tx_send_msg;
        msg->head = HEAD;
        msg->len = 9;       


        calculate_XOR_and_SUM(tx_send_msg, msg->len + 2 , &XOR, &SUM);
        return msg->len + 2;

        break;
        case SET_DEV:
        break;
        default:
        return 0;

    }

    return 0;
}

int decode_msg(cmd_id_e cmd_id) {
    unsigned char XOR, SUM;
    switch (cmd_id) {
        case GET_DEV:
        Get_Dev_Msg_Echo_t *msg = (Get_Dev_Msg_Echo_t *)tx_send_msg;
        msg->head = HEAD;
        msg->len = 9;       


        calculate_XOR_and_SUM(tx_send_msg, msg->len + 2 , &XOR, &SUM);
        return msg->len + 2;
        tx_cmd_id = cmd_id;

        break;
        case SET_DEV:
        break;
        default:
        return 0;

    }

    return 0;
}

bool encode_msg_and_send(cmd_id_e cmd_id) {
    int len = ecode_msg(cmd_id);
    if (len > 0) {
        UART_Write(tx_send_msg, len);
        return true;
    }

    return false;
}

int read_and_decode_msg() {
    int len = UART_Read(rx_recv_msg);
    if (len > 0) {
        int cmd_id = tx_cmd_id;
        decode_msg(cmd_id);
    }
}

