
#ifndef _IS_INCLUDED_TYPE_H
#define _IS_INCLUDED_TYPE_H

//клавіатура
#define kays_buf_size 4
struct STACK{
    unsigned char buf[kays_buf_size];//БУФЕР ДЛЯ НАТИСНУТИХ КЛАВІШ
    unsigned char index;
};
typedef struct STACK KAY_STACK;

struct REPEAT{
    unsigned short first;       //час до першого повторення
    unsigned short all_next;    //час до повторення кожного наступного
    unsigned short all_next_next;    //час до повторення кожного наступного
};
typedef struct REPEAT KAY_REPEAT;
//кінець клавіатури

//For management
struct list{
    char *ptr;              //Operands pointer
    unsigned int count;     //count of bytes
    struct list *next;      //pointer for next element of list
};
typedef struct list LIST;

struct rxtx{
    char pre_time   : 1;//таймаут між передачами використовується якщо відповідь була
    char start_tx   : 1;//початок передачі
    char end_tx     : 1;//кінець передачі
    char start_rx   : 1;//початок прийому
    char end_rx     : 1;//кінець прийому
    char timeout    : 1;//таймаут при прийомі
    char cs         : 1;//crc упакована
    char not_respond: 1;//час очікування відповіді вийшов
};
typedef struct rxtx RXTX;

struct RESPONSE_HEAD{
    char address;
    char code;
    char count;
};
typedef struct RESPONSE_HEAD RESPONSE_HEAD;

//управління меню
struct menu_man{
    int type;
    int change; //0-тільки відображується; 1-модифікується; 2-виконати пост-функцію по натисканню кнопки ввод;
    char *format;//текстовий рядок з форматом
    int *number;//на число-параметр
    union{
        int min;    //мінімальньне значення
        int*next_one;//для вказання наступного елементу меню якщо дане меню використовується як пароль-інтерфейс а розмір вписується в size
                     //для того щоб не втручалося обмеження знизу встановлювати знаковий біт |0x80000000
    };
    int max;    //максимальне значення
    int fract;//клькість знаків після коми
    void *next;//наступний претворит в sub*
    int size;//розмір наступного підменю

    char(*pre_funct)();
    void(*pre_enter)(char);
    char(*post_enter)();
    void(*up_funct)();
    void(*down_funct)();
    void(*left_funct)();
    void(*right_funct)();
};
typedef struct menu_man MENU_MAN;
//біти ознак
struct RUN_TIME_FLAG{
    unsigned int uart0_change    :1; //хоч один параметр було змінено
    unsigned int uart1_change    :1; //хоч один параметр було змінено
    unsigned int                 :30;
};
typedef struct RUN_TIME_FLAG RTF;
//параметри UARTs
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

//додаткові параметри для аналогового входу
typedef struct{
    union{
        struct{
            int volt;//напруга
            union{
                int curr;//струм
                int Ohm;//опір
            };
            union{
                int g;//витрата
                int t;
            };
        }r;
    };
    int A;//сам параметр
    int status;//обрив
}AIU_ADD;

//параметри вібратора
typedef struct VIBR{
    int dummy;// не використовується просто тупий регістр для вирівнювання
    int duty;// коефіцієнт вібрації 0-100%
    int T;//період роботи вібратора
    int t;//час вібрації протягом періоду
}VIBR;

// організація управління сервопривдом
typedef struct MEO{
    int out;//вихід 100.0%
    int setup;//завдання 100.0% 0xffff - ручний режим
    int max_time;//максимальний час руху з одного крайнього положення в інше 0.00 C один розряд - 10 мС
    int block_time;//час блокування пред зміною напрямку руху 0.00 C один розряд - 10 мС
}MEO;

//ДЛЯ ПЛАТИ АНАЛОГОВОГО ВХОДУ
typedef struct{
        int sts;//статус     біти 6,7 - входи
        int adc;//швидкий код ацп три байти
        int ADC;//повільний код три байти
        int temperature;//температура з датчика температури
        union{
            VIBR vibr;//управління вібратором
                //int dummy;// не використовується просто тупий регістр для вирівнювання
                //int duty;// коефіцієнт вібрації 0-100%
                //int T;//період роботи вібратора
                //int t;//час вібрації протягом періоду
            MEO meo;//управління засувкою
                //int out;//вихід 100.00%
                //int setup;//завдання 100.00% 0xffff - ручний режим
                //int max_time;//максимальний час руху з одного крайнього положення в інше 0.00 C один розряд - 10 мС
                //int block_time;//час блокування пред зміною напрямку руху 0.00 C один розряд - 10 мС
        }d;
        int cmd;//команда     0-читати все 1-писати уставки 2-обнулити лічильник
		int mult;//тип пристрою час руху мео
        int offset;//зміщення  три байти
        int gain;//підсилення  три байти
        int Kg;//коеф.підсилення
		int K;//кількість вибірок за якими робиться усереднення
}AIU;

//додаткові параметри для аналогового входу, які зберігаються в пульті
typedef struct{
    union{
        int g_4;//витрата при 4 мА
        int type;//тип термопари термоопору
    };
    union{
        int g_20;//витрата при 20 мА
        int comp;//наявність термокомпенсації
    };
}AIU_R;

//ДЛЯ ПЛАТИ АНАЛОГОВОГО ВИХОДУ
typedef struct{
    unsigned int sts,
        current,//струм
		out;//стан виходів
}AOU;

//для масштабної сітки
typedef struct{
    int big;//кількість великих штрихів
    int small;//кількість маленьких штрихів
}SCAL;

//Для організації сигналізації попереджувальної і аварійної
typedef struct{
    int up_lim;
    int down_lim;
    int max;
    int min;
}LIM;


typedef struct{
    int n; // вказівник на останнє місце запису
    int last_out;//попереднє значення
    int Sp; // задана точка для безударної зміни
    short buf[1000];
}PID_run;

typedef struct{
    int Sp;//робоча точка
    int Kp;// коеф. пропорційності
    int Ti;
    int Td;
    int Ts;//час квантування
    int Range;//діапазон зміни регульованого параметру
    int Out_Range;//повний діапазон зміни вихідного сигналу
    int Speed;//швидкість зміни завдання
    int Out_min;//мінімальне значення вихідного сигналу
    int Out_max;//максимальне значення вихідного сигналу
    int Alarm_h;// аварія вехня межа
    int Alarm_l;// аварія нижня межа
    int on_off;//регулятор включений \ виключений
    union{
        int res;//резерв для регулятора струму
        int cascad;//для регулятора повітря режим проботи за витратою газу
        int TC;//вхідна термопара для регулятора газу зона горіння або зона фідера  - вибір
    };
    int reserv[2];//резерв щоб наступні парараметри не зміщувались
}PID;

typedef struct{
    //два параметри резерв
    int reserv_pg;//щільність газу
    int reserv_pa;//щільність повітря
    int kg;//коеф. перерахунку витрати газу
    int ka;//коеф. перерахунку витрати повітря
    int K;//коефіцієнт співідношення газу і повітря
    int auto_g;//автоматичний реж. газ
    int auto_a;//автоматичний реж. повітря
}GASAIR;

//для виведення фільєри на режим
typedef struct{
    int auto_on; //включення виключення автоматичної виводки
    //виводка з холодного
    int t1,t2,t3;//тривалість етапу
    int c1,c2;// струми
    //виводка до 5 хв
    int t5_1,t5_2;
    int c5;
    //виводка до 1 хв.
    int t_1;
    //зворотня виводка
    int t_b1,t_b2,t_b3;
    int c_b1,c_b2;
}RAMP;

//для управління вібратором для збереження в пульті з наступним засиланням у плату вібратора при включенні
typedef struct{
    int duty;
    int T;
    int t;
}VBR;

//структура управління роботою
typedef struct{
    int flag;//ознака що зерно згенеровано
    int seed;//зерно
    int ans[2];//відповідь
    int req;//запит
    int time;//час роботи
    int flag_ok;//ознака нормальної роботи
    int etap;//для етапів активізації
    int time_lim;//часова межа
}SEQ;

//всі параметри меню знаходяться тут
struct PARAMETRS{
    UART UART0;
    UART UART1;
    int snap_time;//часовий інтревал запису у архів
    int archive_addr;//адреса для перегляду архіву
    int password;//пароль
    int alarm_mask;//маска для аварій якщо біт 1 - аврія відключена
    AIU_R aiu_r[13];//додаткові параметри для обчислень
    int full[14]; //повна шкала для графіків
    int nill[14]; //низ шкали для графіків
    SCAL scal[14];//для масштабної сітки лівої правої нижньо
    LIM lim[11]; //попередження і аварії
    SEQ seq;//для управлління ліцензіями

    VBR vibr;//параметри вібратора
    PID pid[4]; //під регулятори
    GASAIR G_A;//параметри співідношення газу і повітря
    RAMP R;//для виводки
    int touch;//включити тачпанель

};
typedef struct PARAMETRS PARAMETRS;


//---ВІРТУАЛЬНІ ТАЙМЕРИ
struct VIRTUAL_TIMER
{
    unsigned int  tick;//відліки до нуля
    unsigned char event;
};
typedef struct VIRTUAL_TIMER VIRTUAL_TIMER;
//---ВІРТУАЛЬНІ ТАЙМЕРИ КІНЕЦЬ

struct FLASH_ADDR{
    unsigned int addr    : 19;//19 bit max address - 7ffff
    unsigned int         : 13;
};
typedef struct FLASH_ADDR FLASH_ADDR;

struct DUMP{
    FLASH_ADDR flash;//адреса
    char buf[28];//буфер куди будуть читатися дані із флеш
};
typedef struct DUMP DUMP;

#pragma pack()
typedef struct {
    unsigned int time;
    unsigned char Ggas;//витрата газу
    unsigned char Gair;//витрата повітря
    unsigned short Tgasiv;//темератуга зазів що відходять
    unsigned short Tsum;//температура суміші
    unsigned short Tfider;//температура фідера
    unsigned short curr;//струм фільєри
    unsigned short Vbr;//період вібрації
}REC;


//УПРАВЛІННЯ ПРОЦЕСОМ ФОРМУВАННЯ АРХІВУ
struct A_MNG{
    unsigned int flash_addr;//адреса у флеш куди треба записувати наступний запис
    unsigned int rec_count;//номер підзапису в поточному записі
    union{
        REC rec[16];//самі записи
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
    int addr;//адреса у флеш крок 256
    union{
        unsigned int start_channal;//початковий канал
        unsigned int channal;//канал для графіків
    };
    int cursor;//позиція курсору на графіку

    REC rec[15][16];//на графік виділено область розміром 260 х 198 24*11 = 264
};
typedef struct ARCH_VIEW ARCH_VIEW;
//адреса розміщення у рамтроні структури a_mng
#define a_mng_rmtrn_addr 0x400

#pragma pack(1)
struct ALARM_RECORD{
    char year;
    char month;
    char date;
    char hour;
    char min;
    char sec;
    char number;//номер аварії
    char type;//тип аварії
};
typedef struct ALARM_RECORD ALARM_RECORD;

struct one_alarm{
    char number;
    char type;
};
typedef struct one_alarm one_alarm;

#pragma pack()


//координати натискання
struct C_XY{
    unsigned int x;
    unsigned int y;
};
typedef struct C_XY C_XY;

//для тачпанелі
typedef struct{
    char cmd0;//команда
    char dummy0;//просто байт
    char cmd1;//команда
    char dummy1;//просто байт
    char cmd2;//команда
    char dummy2;//просто байт
    char cmd3;//команда
    char dummy3;//просто байт
    char cmd4;//команда
    char dummy4;//просто байт
    char cmd5;//команда
    char dummy5;//просто байт
    char dummy6;//просто байт
}SPI_TOUCH_TX;

typedef struct{
    char dummy0;//команда
    char hi0;//просто байт
    char lo0;//команда
    char hi1;//просто байт
    char lo1;//команда
    char hi2;//просто байт
    char lo2;//команда
    char hi3;//просто байт
    char lo3;//команда
    char hi4;//просто байт
    char lo4;//команда
    char hi5;//просто байт
    char lo5;//команда
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

//витрата газу і повітря
typedef struct{
    int Qg;//витрата газу
    int Qa;//витрата повітря
    int T_RG;// виміряне значення (температура) для регулятора витрати газу
}QGA;

// для виведення фільєри на режим
typedef struct{
    bool power;//наявність електрики
    bool last_power;//останній статус наявності мережі
    bool on;//ознака того що фільєра включена
    bool last_on; //попередня ознака того що фільєра включена
    char regime;//режим виводки, холодна-тепла- гаряча
    char phase;//фаза режиму виводки
    char b_phase;//фаза режиму охолодження
    unsigned int time;//час, що відраховується до кінця етапу
    unsigned int time_down;//час протягом якого фільєра вимкнута
    unsigned int ramp;//включити режим виводки фільєри
    unsigned int down_ramp;//включити режим зворотньої виводки фільєри
}FLR;

// для організації обміну з компютером
typedef struct{
    int *addr;//початкова адреса параметрів
    char count;//кількість підряд параметрів
}MS;//mobus slave

/* діапазон значень, якщо обидва нульові то записзаборонено і адреса параметра*/
typedef struct{
    unsigned short min;   //min. значення
    unsigned short max;   //max. значення
    int *adr;// адреса параметра
}MMA;



#endif
