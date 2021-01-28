

#define modbufsize 256

extern union{
    char modbuf[modbufsize];

    struct{
        char address;
        char function;
        char exeption;
    }exception;//повідомлення про виключну ситуацію

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }read_req;//читання Запит

    struct{
        char address;
        char function;
        char byte_count;
    }read_resp;//читання відповідь

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_req;//запис одного регістра Запит

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_resp;//запис одного регістра відповідь

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
        unsigned char byte_count;
        unsigned short first;
    }write_mult_req;//запис одного регістра Запит

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }write_mult_resp;//запис одного регістра відповідь

}buf;

extern bool all;//писати всі в т ч таймери
extern bool request_type;//тип читати чи писати
extern bool w_s_error;//помилка

extern struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//девайси що не відповідають належним чином
    unsigned short err_cntr;//лічильник помилок

}aum;

extern char device;
extern MS ms;//для збереження у фоновій програмі даних отриманих по інтерфейсі

void modbus_uart0_init(void);
void modbus_uart1_init(void);
void slave();
void modbus_receive_init();

void transmit_arch(char *src, int count);

void modbus_0();

