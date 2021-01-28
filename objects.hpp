
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
    char row;               // �����
    char column;            // ��������
    const char *fnt_ptr;    // �����
    char const *str;        // �����    
    
    text(char row_, char column_, const char *fnt_ptr_,char const *str_);
};
/*****************************************************************************/
/*****************************************************************************/
class caption
{ 
    public:
                            // ���������� ��������� ����� ���� ������
    char x,y;               // 0 <= x <= 39, 0 <= y <= 239
    char width,hight;       // ������ � ������ � ������    
    char const *pict;       // �������� �� �������� ����� � ������
    
    caption(char x_,char y_,char width_,char hight_,char const *pict_);
};
/*****************************************************************************/
/*****************************************************************************/
class control_keys:public caption
{
    public:
    static bool touch_block;    //����� ��������� ���� �������� ���� 
    static bool need_for_change;
    static bool out_from_change;//�������� ��������
    static bool was_1s; //1 ������� ������
    void(*func_ptr)();
    
    control_keys(void(*func_ptr_)(),char x_,char y_,char width_,char hight_,char const *pict_);
    void processing();  // ������� �����
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
// ���� ������� ����
class num_fild
{
    public:
    char row,column;    // ���������� ��������� ����� ���� row = 0..239 column = 0..39
    char const*font;    // �����
    bool change;        // ����� ��������
    bool select;        // ����
    bool time_f;        // ���
    char const *format; // ������ �����
    static char out_str[40];   // ����� ��� ��������� �����
    int count;          // ������� ������� � �����
    double number;      // ���������
    static double*back; // ������� �����
    static char len;    // ������ ���� ��� �����
    static char offset; // ������� ���� ��� ����� 
    char*(*func_pre)(); // �������� �� �����������
    char*(*func_post)();// �������� ���� ����������� 
    static double*ptr_number;//��������������� � ��������� ��������
    
    num_fild(char*(*func_pre_)(), char*(*func_post_)(), char row_, char column_, char const*font_, bool change_, const char*format_, double number_, bool time_f_= false);
    void contour();     // ��������� �������
    void text();        // ����� �����
    void highlight(bool flag);  // ��������
    bool hit_test();    // �������� ����������
    void copy_out();    // ��������� ����� ��� ��������
    static char*no_change();//do nothing
    static char*month();// ����� ������ ����� �� �����
    static char*month_back();//����� ����� 
    static char*time_();// ��� ��������� ����
    static char*time_back();//����� ��� �����
};
/*****************************************************************************/
/*****************************************************************************/
class sheet_keys:public caption
{
    public:
    //bool chacked;           // ���������
    static char sheet;      // ����� ������� �������
      
    sheet_keys(char x_,char y_,char width_,char hight_,char const *pict_);
};
/*****************************************************************************/
/*****************************************************************************/
typedef void form_funct(void);
typedef form_funct *form_f;

class form
{
    public:
    char sheet;             // ����� ������� �������
    num_fild*param;         // ���������
    char param_;            // ������� ���������
    control_keys*button;    // ������
    char button_;           // ������� ������
    form_f funct_ptr;       // ������� ��������
    char funct;             // ������� ������� ��������
    int rows;
    int columns;
    int hights;
    int widths;
    text*txt;               // �����
    char txt_;              // ������� ����� ������

    form(char sheet_, num_fild*param_, char param__, control_keys*button_, char button__, form_f funct_ptr_=0, char funct_=0, int rows_=0, int columns_=0, int hights_=0, int widths_=0, text*_txt=0, char txt__=0);
    static void refresh(form*_form);    // ���������
};
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

