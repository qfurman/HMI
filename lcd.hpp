
#ifndef _IS_INCLUDED_LCD_HPP
#define _IS_INCLUDED_LCD_HPP

#ifndef bool
    #define bool char
#endif

#ifndef false
    #define false 0
#endif

#ifndef  true
    #define true 1
#endif


extern const char *fnt_ptr;     //активний шрифт
#define set_font(font_name) fnt_ptr = &font_name[0]

extern char layer;              //активний прошарок для виводу тексту 0,1,2
#define set_layer(layer_name) layer = layer_name

/*****************************************************************************/
#define lcd_set_display()   lcd_put_command(0x59);lcd_put_data(display);

typedef struct
{
    unsigned  char fc   :2; //cursor
    unsigned  char fp0  :2; //layer0
    unsigned  char fp1  :2; //layer1
    unsigned  char fp2  :2; //layer2
}disp;

typedef struct
{
    unsigned  char mx   :2; //composition
    unsigned  char dm1  :1; //mode
    unsigned  char dm2  :1; //mode
    unsigned  char ov   :1; //overlay
    unsigned  char un   :3; //unused
}ovlay;

extern"C" union
{
    char display;
    disp disp_bit;
};

extern"C" union
{
    char overlay;
    ovlay ovlay_bit;
};

/*****************************************************************************/
extern char lcd_buf[9600];
/*****************************************************************************/
extern "C"void lcd_put_command(char command);
extern "C"void lcd_put_data(char data);
extern "C"void lcd_cls(void);
extern "C"void lcd_init(void);
extern "C"void lcd_ready(void);
extern "C"void lcd_busy(void);
extern "C"void lcd_draw_pix(int x,int y,char layer,char color);
extern "C"void lcd_write_byte(char byte,int x,int y,char layer);
extern "C"void lcd_line(int x0,int y0,int x1,int y1,char layer,char color);
extern "C"void lcd_write_memory(int x,int y,char layer,const char *data,int row,int column);
extern "C"void lcd_all_screen(char layer,const char *data);
extern "C"void lcd_put_text(int row,int column,char const *str);
extern "C"void lcd_write_array(int column,int row,char layer,const char *data,int hight,int width);

extern "C"void lcd_buf_put_text(int row,int column,char const *str,char overlay,bool inv);
extern "C"void lcd_buf_write_array(int column,int row, const char *data,int hight,int width, char overlay,bool inv);
extern "C"void lcd_buf_draw_pix(int x, int y, bool color);
extern "C"void lcd_buf_write_byte(char byte, int x, int y, char overlay);
extern "C"void lcd_buf_line(int x0,int y0,int x1,int y1,bool color);
extern "C"void lcd_buf_fill_window(int row, int column, int hight, int width, char data, char overlay,bool inv);
extern "C"void lcd_buf_window(int row, int column, int hight, int width);

#endif //_IS_INCLUDED_LCD_H
