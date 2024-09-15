#define HEAD     0x7E

typedef enum {
    GET_DEV = 0x12,
    SET_DEV = 0x20,
} cmd_id_e;

typedef struct {
    char head;
    unsigned char len;
    unsigned char dest_id;
    unsigned char cmd_id;
    unsigned char xor;
    unsigned char sum;
} Get_Dev_Msg_Send_t;

typedef struct {
    char head;
    unsigned char len;

} Get_Dev_Msg_Echo_t;

typedef struct {
    char head;
    unsigned char len;
    unsigned char dest_id;
    unsigned char cmd_id;
    unsigned char xor;
    unsigned char sum;
} Set_Dev_Msg_Send_t;

typedef struct {
    char head;
    unsigned char len;
    unsigned char dest_id;
    unsigned char cmd_id;
    unsigned char xor;
    unsigned char sum;
} Set_Dev_Msg_Echo_t;

char* encode_msg_and_send(cmd_id_e cmd_id);