
/*****************************************************************************/
/*****************************************************************************/
struct sheet_seq
{
    char num;
    char seq[20];
};
extern struct sheet_seq sh_seq;
/*****************************************************************************/
/*****************************************************************************/
class text
{
    public:
    char row;               // рядок
    char column;            // стовбець
    const char *fnt_ptr;    // шрифт
    char const *str;        // текст    
    
    text(char row_, char column_, const char *fnt_ptr_,char const *str_);
};
/*****************************************************************************/
/*****************************************************************************/
class caption
{ 
    public:
                            // координати верхнього лівого кута клавіші
    char x,y;               // 0 <= x <= 39, 0 <= y <= 239
    char width,hight;       // ширина і висота в байтах    
    char const *pict;       // вказівник на внутрішній напис у клавіші
    
    caption(char x_,char y_,char width_,char hight_,char const *pict_);
};
/*****************************************************************************/
/*****************************************************************************/
class control_keys:public caption
{
    public:
    static bool touch_block;    //блокує клавіатуру після закриття вікна 
    static bool need_for_change;
    static bool out_from_change;//введення закінчено
    static bool was_1s; //1 секунда минула
    void(*func_ptr)();
    
    control_keys(void(*func_ptr_)(),char x_,char y_,char width_,char hight_,char const *pict_);
    void processing();  // обробка клавіш
    static void close();
    static void ok();
    static void up();
    static void down();
    static void right();
    static void left();
    static void close_window();
    static void zero();
    static void one();
    static void two();
    static void three();
    static void four();
    static void five();
    static void six();
    static void seven();
    static void eight();
    static void nine();
    static void button_pres(char charact);
    static bool time_snap();
    static void prepare_input();    
};
/*****************************************************************************/
/*****************************************************************************/
// клас числове поле
class num_fild
{
    public:
    char row,column;    // координати верхнього лівого кута row = 0..239 column = 0..39
    char const*font;    // шрифт
    bool change;        // дозвіл змінювати
    bool select;        // вибір
    bool time_f;        // час
    char const *format; // формат числа
    static char out_str[40];   // рядок для виведення числа
    int count;          // кількість символів в рядку
    double number;      // параметер
    static double*back; // введене число
    static char len;    // ширина поля для вводу
    static char offset; // зміщення поля для вводу 
    char*(*func_pre)(); // обробник до модифікації
    char*(*func_post)();// обробник після модифікації 
    static double*ptr_number;//використовується в статичних функціях
    
    num_fild(char*(*func_pre_)(), char*(*func_post_)(), char row_, char column_, char const*font_, bool change_, const char*format_, double number_, bool time_f_= false);
    void contour();     // генерація контуру
    void text();        // вписує текст
    void highlight(bool flag);  // виділення
    bool hit_test();    // перевірка натискання
    void copy_out();    // копіювання рядка для введення
    static char*no_change();//do nothing
    static char*month();// заміна номера місяця на назву
    static char*month_back();//змінює місяць 
    static char*time_();// для виведення часу
    static char*time_back();//запиує час назад
};
/*****************************************************************************/
/*****************************************************************************/
class sheet_keys:public caption
{
    public:
    //bool chacked;           // залипання
    static char sheet;      // номер активної сторінки
      
    sheet_keys(char x_,char y_,char width_,char hight_,char const *pict_);
};
/*****************************************************************************/
/*****************************************************************************/
typedef void form_funct(void);
typedef form_funct *form_f;

class form
{
    public:
    char sheet;             // номер активної сторінки
    num_fild*param;         // параметри
    char param_;            // кількість параметрів
    control_keys*button;    // кнопки
    char button_;           // кількість кнопок
    form_f funct_ptr;       // функції учасники
    char funct;             // кількість функцій учасників
    int rows;
    int columns;
    int hights;
    int widths;
    text*txt;               // текст
    char txt_;              // кількість рядків тексту

    form(char sheet_, num_fild*param_, char param__, control_keys*button_, char button__, form_f funct_ptr_=0, char funct_=0, int rows_=0, int columns_=0, int hights_=0, int widths_=0, text*_txt=0, char txt__=0);
    static void refresh(form*_form);    // оновлення
};
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

