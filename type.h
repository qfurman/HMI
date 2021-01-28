
#ifndef _IS_INCLUDED_TYPE_H
#define _IS_INCLUDED_TYPE_H

//���������
#define kays_buf_size 4
struct STACK{
    unsigned char buf[kays_buf_size];//����� ��� ���������� ���²�
    unsigned char index;
};
typedef struct STACK KAY_STACK;

struct REPEAT{
    unsigned short first;       //��� �� ������� ����������
    unsigned short all_next;    //��� �� ���������� ������� ����������
    unsigned short all_next_next;    //��� �� ���������� ������� ����������
};
typedef struct REPEAT KAY_REPEAT;
//����� ���������

//For management
struct list{
    char *ptr;              //Operands pointer
    unsigned int count;     //count of bytes
    struct list *next;      //pointer for next element of list
};
typedef struct list LIST;

struct rxtx{
    char pre_time   : 1;//������� �� ���������� ��������������� ���� ������� ����
    char start_tx   : 1;//������� ��������
    char end_tx     : 1;//����� ��������
    char start_rx   : 1;//������� �������
    char end_rx     : 1;//����� �������
    char timeout    : 1;//������� ��� ������
    char cs         : 1;//crc ���������
    char not_respond: 1;//��� ���������� ������ ������
};
typedef struct rxtx RXTX;

struct RESPONSE_HEAD{
    char address;
    char code;
    char count;
};
typedef struct RESPONSE_HEAD RESPONSE_HEAD;

//��������� ����
struct menu_man{
    int type;
    int change; //0-����� ������������; 1-������������; 2-�������� ����-������� �� ���������� ������ ����;
    char *format;//��������� ����� � ��������
    int *number;//�� �����-��������
    union{
        int min;    //���������� ��������
        int*next_one;//��� �������� ���������� �������� ���� ���� ���� ���� ��������������� �� ������-��������� � ����� ��������� � size
                     //��� ���� ��� �� ���������� ��������� ����� ������������� �������� �� |0x80000000
    };
    int max;    //����������� ��������
    int fract;//������� ����� ���� ����
    void *next;//��������� ��������� � sub*
    int size;//����� ���������� ������

    char(*pre_funct)();
    void(*pre_enter)(char);
    char(*post_enter)();
    void(*up_funct)();
    void(*down_funct)();
    void(*left_funct)();
    void(*right_funct)();
};
typedef struct menu_man MENU_MAN;
//��� �����
struct RUN_TIME_FLAG{
    unsigned int uart0_change    :1; //��� ���� �������� ���� ������
    unsigned int uart1_change    :1; //��� ���� �������� ���� ������
    unsigned int                 :30;
};
typedef struct RUN_TIME_FLAG RTF;
//��������� UARTs
struct UART{
    int bitrate;
    int word_length;
    int stop_bit;
    int parity_enable;
    int parity_select;
    int simplex;
    int wait_respond;
    int timeout;
};
typedef struct UART UART;

//�������� ��������� ��� ����������� �����
typedef struct{
    union{
        struct{
            int volt;//�������
            union{
                int curr;//�����
                int Ohm;//���
            };
            union{
                int g;//�������
                int t;
            };
        }r;
    };
    int A;//��� ��������
    int status;//�����
}AIU_ADD;

//��������� ��������
typedef struct VIBR{
    int dummy;// �� ��������������� ������ ����� ������ ��� �����������
    int duty;// ���������� ������� 0-100%
    int T;//����� ������ ��������
    int t;//��� ������� �������� ������
}VIBR;

// ���������� ��������� ������������
typedef struct MEO{
    int out;//����� 100.0%
    int setup;//�������� 100.0% 0xffff - ������ �����
    int max_time;//������������ ��� ���� � ������ ��������� ��������� � ���� 0.00 C ���� ������ - 10 ��
    int block_time;//��� ���������� ���� ����� �������� ���� 0.00 C ���� ������ - 10 ��
}MEO;

//��� ����� ����������� �����
typedef struct{
        int sts;//������     ��� 6,7 - �����
        int adc;//������� ��� ��� ��� �����
        int ADC;//�������� ��� ��� �����
        int temperature;//����������� � ������� �����������
        union{
            VIBR vibr;//��������� ���������
                //int dummy;// �� ��������������� ������ ����� ������ ��� �����������
                //int duty;// ���������� ������� 0-100%
                //int T;//����� ������ ��������
                //int t;//��� ������� �������� ������
            MEO meo;//��������� ��������
                //int out;//����� 100.00%
                //int setup;//�������� 100.00% 0xffff - ������ �����
                //int max_time;//������������ ��� ���� � ������ ��������� ��������� � ���� 0.00 C ���� ������ - 10 ��
                //int block_time;//��� ���������� ���� ����� �������� ���� 0.00 C ���� ������ - 10 ��
        }d;
        int cmd;//�������     0-������ ��� 1-������ ������� 2-�������� ��������
		int mult;//��� �������� ��� ���� ���
        int offset;//�������  ��� �����
        int gain;//���������  ��� �����
        int Kg;//����.���������
		int K;//������� ������ �� ����� �������� �����������
}AIU;

//�������� ��������� ��� ����������� �����, �� ����������� � �����
typedef struct{
    union{
        int g_4;//������� ��� 4 ��
        int type;//��� ��������� ����������
    };
    union{
        int g_20;//������� ��� 20 ��
        int comp;//�������� ����������������
    };
}AIU_R;

//��� ����� ����������� ������
typedef struct{
    unsigned int sts,
        current,//�����
		out;//���� ������
}AOU;

//��� ��������� ����
typedef struct{
    int big;//������� ������� �������
    int small;//������� ��������� �������
}SCAL;

//��� ���������� ����������� ��������������� � �������
typedef struct{
    int up_lim;
    int down_lim;
    int max;
    int min;
}LIM;


typedef struct{
    int n; // �������� �� ������ ���� ������
    int last_out;//�������� ��������
    int Sp; // ������ ����� ��� ��������� ����
    short buf[1000];
}PID_run;

typedef struct{
    int Sp;//������ �����
    int Kp;// ����. �������������
    int Ti;
    int Td;
    int Ts;//��� �����������
    int Range;//������� ���� ������������� ���������
    int Out_Range;//������ ������� ���� ��������� �������
    int Speed;//�������� ���� ��������
    int Out_min;//�������� �������� ��������� �������
    int Out_max;//����������� �������� ��������� �������
    int Alarm_h;// ����� ����� ����
    int Alarm_l;// ����� ����� ����
    int on_off;//��������� ��������� \ ����������
    union{
        int res;//������ ��� ���������� ������
        int cascad;//��� ���������� ������ ����� ������� �� �������� ����
        int TC;//������ ��������� ��� ���������� ���� ���� ������ ��� ���� ������  - ����
    };
    int reserv[2];//������ ��� ������� ����������� �� ����������
}PID;

typedef struct{
    //��� ��������� ������
    int reserv_pg;//�������� ����
    int reserv_pa;//�������� ������
    int kg;//����. ����������� ������� ����
    int ka;//����. ����������� ������� ������
    int K;//���������� ����������� ���� � ������
    int auto_g;//������������ ���. ���
    int auto_a;//������������ ���. ������
}GASAIR;

//��� ��������� ������� �� �����
typedef struct{
    int auto_on; //��������� ���������� ����������� �������
    //������� � ���������
    int t1,t2,t3;//��������� �����
    int c1,c2;// ������
    //������� �� 5 ��
    int t5_1,t5_2;
    int c5;
    //������� �� 1 ��.
    int t_1;
    //�������� �������
    int t_b1,t_b2,t_b3;
    int c_b1,c_b2;
}RAMP;

//��� ��������� ��������� ��� ���������� � ����� � ��������� ���������� � ����� �������� ��� ��������
typedef struct{
    int duty;
    int T;
    int t;
}VBR;

//��������� ��������� �������
typedef struct{
    int flag;//������ �� ����� �����������
    int seed;//�����
    int ans[2];//�������
    int req;//�����
    int time;//��� ������
    int flag_ok;//������ ��������� ������
    int etap;//��� ����� ����������
    int time_lim;//������ ����
}SEQ;

//�� ��������� ���� ����������� ���
struct PARAMETRS{
    UART UART0;
    UART UART1;
    int snap_time;//������� �������� ������ � �����
    int archive_addr;//������ ��� ��������� ������
    int password;//������
    int alarm_mask;//����� ��� ����� ���� �� 1 - ���� ���������
    AIU_R aiu_r[13];//�������� ��������� ��� ���������
    int full[14]; //����� ����� ��� �������
    int nill[14]; //��� ����� ��� �������
    SCAL scal[14];//��� ��������� ���� ��� ����� ������
    LIM lim[11]; //������������ � ����
    SEQ seq;//��� ���������� ��������

    VBR vibr;//��������� ��������
    PID pid[4]; //�� ����������
    GASAIR G_A;//��������� ����������� ���� � ������
    RAMP R;//��� �������
    int touch;//�������� ���������

};
typedef struct PARAMETRS PARAMETRS;


//---²������Ͳ �������
struct VIRTUAL_TIMER
{
    unsigned int  tick;//����� �� ����
    unsigned char event;
};
typedef struct VIRTUAL_TIMER VIRTUAL_TIMER;
//---²������Ͳ ������� ʲ����

struct FLASH_ADDR{
    unsigned int addr    : 19;//19 bit max address - 7ffff
    unsigned int         : 13;
};
typedef struct FLASH_ADDR FLASH_ADDR;

struct DUMP{
    FLASH_ADDR flash;//������
    char buf[28];//����� ���� ������ �������� ��� �� ����
};
typedef struct DUMP DUMP;

#pragma pack()
typedef struct {
    unsigned int time;
    unsigned char Ggas;//������� ����
    unsigned char Gair;//������� ������
    unsigned short Tgasiv;//���������� ���� �� ��������
    unsigned short Tsum;//����������� �����
    unsigned short Tfider;//����������� ������
    unsigned short curr;//����� �������
    unsigned short Vbr;//����� �������
}REC;


//�����˲��� �������� ���������� ��ղ��
struct A_MNG{
    unsigned int flash_addr;//������ � ���� ���� ����� ���������� ��������� �����
    unsigned int rec_count;//����� �������� � ��������� �����
    union{
        REC rec[16];//��� ������
        char rec_buf[256];
        int rec_ibuf[64];
    };
};

struct TIME_ARCH{
    unsigned int sec     : 6;
    unsigned int sec_ov  : 2;
    unsigned int min     : 6;
    unsigned int min_ov  : 2;
    unsigned int hour    : 5;
    unsigned int hour_ov : 3;
    unsigned int day     : 3;
    unsigned int day_ov  : 5;
};
typedef struct TIME_ARCH TIME_ARCH;

struct ARCH_VIEW{
    int addr;//������ � ���� ���� 256
    union{
        unsigned int start_channal;//���������� �����
        unsigned int channal;//����� ��� �������
    };
    int cursor;//������� ������� �� �������

    REC rec[15][16];//�� ������ ������� ������� ������� 260 � 198 24*11 = 264
};
typedef struct ARCH_VIEW ARCH_VIEW;
//������ ��������� � ������� ��������� a_mng
#define a_mng_rmtrn_addr 0x400

#pragma pack(1)
struct ALARM_RECORD{
    char year;
    char month;
    char date;
    char hour;
    char min;
    char sec;
    char number;//����� ����
    char type;//��� ����
};
typedef struct ALARM_RECORD ALARM_RECORD;

struct one_alarm{
    char number;
    char type;
};
typedef struct one_alarm one_alarm;

#pragma pack()


//���������� ����������
struct C_XY{
    unsigned int x;
    unsigned int y;
};
typedef struct C_XY C_XY;

//��� ��������
typedef struct{
    char cmd0;//�������
    char dummy0;//������ ����
    char cmd1;//�������
    char dummy1;//������ ����
    char cmd2;//�������
    char dummy2;//������ ����
    char cmd3;//�������
    char dummy3;//������ ����
    char cmd4;//�������
    char dummy4;//������ ����
    char cmd5;//�������
    char dummy5;//������ ����
    char dummy6;//������ ����
}SPI_TOUCH_TX;

typedef struct{
    char dummy0;//�������
    char hi0;//������ ����
    char lo0;//�������
    char hi1;//������ ����
    char lo1;//�������
    char hi2;//������ ����
    char lo2;//�������
    char hi3;//������ ����
    char lo3;//�������
    char hi4;//������ ����
    char lo4;//�������
    char hi5;//������ ����
    char lo5;//�������
}SPI_TOUCH_RX;


/*
previous_error = 0
I = 0
start:
  error = setpoint - actual_position
  P = Kp * error
  I = I + Ki * error * dt
  D = (Kd / dt) * (error - previous_error)
  output = P + I + D
  previous_error = error
  wait(dt)
  goto start
*/

//������� ���� � ������
typedef struct{
    int Qg;//������� ����
    int Qa;//������� ������
    int T_RG;// ������� �������� (�����������) ��� ���������� ������� ����
}QGA;

// ��� ��������� ������� �� �����
typedef struct{
    bool power;//�������� ���������
    bool last_power;//������� ������ �������� �����
    bool on;//������ ���� �� ������� ��������
    bool last_on; //��������� ������ ���� �� ������� ��������
    char regime;//����� �������, �������-�����- ������
    char phase;//���� ������ �������
    char b_phase;//���� ������ �����������
    unsigned int time;//���, �� ������������ �� ���� �����
    unsigned int time_down;//��� �������� ����� ������� ��������
    unsigned int ramp;//�������� ����� ������� �������
    unsigned int down_ramp;//�������� ����� ��������� ������� �������
}FLR;

// ��� ���������� ����� � ����������
typedef struct{
    int *addr;//��������� ������ ���������
    char count;//������� ����� ���������
}MS;//mobus slave

/* ������� �������, ���� ������ ������ �� ��������������� � ������ ���������*/
typedef struct{
    unsigned short min;   //min. ��������
    unsigned short max;   //max. ��������
    int *adr;// ������ ���������
}MMA;



#endif
